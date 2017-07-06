/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <algorithm>

// ROOT headers
#include "TLinearFitter.h"
#include "TMatrixTBase.h"
#include "TMath.h"
#include "Math/Functor.h"
#include "Fit/Fitter.h"
#include "Minuit2/Minuit2Minimizer.h"

// MAUS headers
#include "src/common_cpp/Recon/SciFi/RootFitter.hh"

namespace RootFitter {

bool FitLineLinear(const std::vector<double>& x, const std::vector<double>& y,
                   const std::vector<double>& yerr, MAUS::SimpleLine& line, TMatrixD& cov_matrix) {

  TLinearFitter lf(1);
  lf.SetFormula("pol1");
  // Have to copy data into new elements as AssignData wants non-const arguments for some reason
  std::vector<double> x1 = x;
  std::vector<double> y1 = y;
  std::vector<double> yerr1 = yerr;
  lf.AssignData(x.size(), 1, &x1[0], &y1[0], &yerr1[0]);
  lf.Eval();

  TVectorD params;
  TVectorD errors;
  lf.GetParameters(params);
  lf.GetErrors(errors);

  line.set_c(params[0]);
  line.set_m(params[1]);
  line.set_c_err(errors[0]);
  line.set_m_err(errors[1]);
  line.set_chisq(lf.GetChisquare());
  lf.GetCovarianceMatrix(cov_matrix);

  return true;
}

bool FitCircleMinuit(const std::vector<double>& x, const std::vector<double>& y,
                     MAUS::SimpleCircle& circ, TMatrixD& cov_matrix) {

  auto Chi2Function = [&x, &y](const double *par) {
    // Minimisation function computing the sum of squares of residuals
    // looping over the points
    double f = 0.0;
    for (size_t i = 0; i < x.size(); i++) {
        double u = x[i] - par[0];
        double v = y[i] - par[1];
        double dr = par[2] - std::sqrt(u*u+v*v);
        f += dr*dr;
    }
    return f;
  };

  // Wrap chi2 function in a function object for the fit
  // 3 is the number of fit parameters (size of array par)
  ROOT::Math::Functor fcn(Chi2Function, 3);
  ROOT::Fit::Fitter fitter;
  double pStart[3] = {0, 0, 1};
  fitter.SetFCN(fcn, pStart);
  fitter.Config().ParSettings(0).SetName("x0");
  fitter.Config().ParSettings(1).SetName("y0");
  fitter.Config().ParSettings(2).SetName("R");

  // do the fit
  bool ok = fitter.FitFCN();
  if (!ok) {
    return ok;
  }
  const ROOT::Fit::FitResult & result = fitter.Result();
  result.Print(std::cout);

  // Create a circle object with the results
  circ = MAUS::SimpleCircle(result.Parameter(0), result.Parameter(1),
                            result.Parameter(2));
  circ.set_x0_err(result.Error(0));
  circ.set_y0_err(result.Error(1));
  circ.set_R_err(result.Error(2));
  // circ.set_chisq(result.Chi2());
  circ.set_chisq(result.MinFcnValue());
  circ.set_pvalue(result.Prob());
  result.GetCovarianceMatrix(cov_matrix);

  return true;
}

bool FitHelixMinuit(const std::vector<double>& x, const std::vector<double>& y,
                    const std::vector<double>& z, const double* pStart, MAUS::SimpleHelix& helix) {

  double x0 = x[0];
  double y0 = y[0];

  auto Chi2Function = [&x, &y, &z, &x0, &y0](const double *par) {
    // Minimisation function computing the sum of squares of residuals looping over the points
    double xc = par[0];
    double yc = par[1];
    double rad = par[2];
    double dsdz = par[3];
    double chisq = 0.0;
    for (size_t i = 0; i < x.size(); i++) {
        double theta = (z[i]*dsdz) / rad;
        double f = xc + (x0 - xc)*cos(theta) - (y0 - yc)*sin(theta);
        double g = yc + (y0 - yc)*cos(theta) + (x0 - xc)*sin(theta);
        double dx = f - x[i];
        double dy = g - y[i];
        double dchisq = (dx*dx) + (dy*dy);
        // std::cerr << "Chi2Function: " << xc << " " << yc << " " << rad << " " << dsdz  << " "
        //           << z[i] << " " << theta << " " << cos(theta) << " " << sin(theta) << " "
        //           << f << " " << x[i] << " " << dx << " "
        //           << g << " " << y[i] << " " << dy << " "
        //           << dchisq << "\n";
        chisq += dchisq;
    }
    // std::cerr << "Chi2Function: Final chisq: " << chisq << std::endl;
    return chisq;
  };

  // Scan the function to seed the minimiser
  double nsteps = 100.0;
  if (pStart[2] < 10.0) nsteps = 200.0;
  const double lower_limit = -1.0;
  const double upper_limit = 1.0;
  double step_size = (upper_limit - lower_limit) / nsteps;
  double min_chisq = 10000000000.0;
  double dsdz_seed = pStart[3];
  for (double seed = lower_limit; seed < upper_limit + step_size; seed += step_size) {
    const double params[4] = {pStart[0], pStart[1], pStart[2], seed};
    double resid = Chi2Function(params);
    if (resid < min_chisq) {
      min_chisq = resid;
      dsdz_seed = seed;
    }
  }
  // std::cerr << "dsdz_seed = " << dsdz_seed << ", with chisq = " << min_chisq << std::endl;

  // Perform the minimisation with ROOT::Math::Minimizer interface to MINUIT2, Migrad algorithm
  ROOT::Minuit2::Minuit2Minimizer min(ROOT::Minuit2::kMigrad);
  min.SetMaxFunctionCalls(1000000);
  min.SetMaxIterations(100000);
  min.SetTolerance(0.001);

  ROOT::Math::Functor fcn(Chi2Function, 4);
  min.SetFunction(fcn);

  double step[4] = {0.01, 0.01, 0.01, 0.01};

  // Set the variables to be minimized
  min.SetFixedVariable(0, "xc", pStart[0]);
  min.SetFixedVariable(1, "yc", pStart[1]);
  min.SetFixedVariable(2, "R",  pStart[2]);
  min.SetLimitedVariable(3, "dsdz", dsdz_seed, step[3], -1.0, 1.0);

  // Do the fit
  bool ok = min.Minimize();
  if (!ok) {
    // std::cerr << "Full helix fit did not converge\n";
    return ok;
  }
  const double *xs = min.X();
  const double *errors = min.Errors();

  // Calculate s0
  double s0 = xs[2] * tan((y0 - xs[1])/(x0 - xs[0]));

  // Pull out the covariance matrix
  TMatrixD cov(4, 4);
  for (size_t i = 0; i < 4; ++i) {
    for (size_t j = 0; j < 4; ++j) {
      cov(i, j) = min.CovMatrix(i, j);
    }
  }

  // std::cerr << "Full helix fit successfully converged\n";
  // std::cerr << "dsdz = " << xs[3] << " +/- " << errors[3] << std::endl;
  // std::cerr << "Chisq: " << min.MinValue() << std::endl;

  // Populate the helix
  helix = MAUS::SimpleHelix(xs[0], errors[0], \
                            xs[1], errors[1], \
                            xs[2], errors[2], \
                            xs[3], errors[3], \
                            s0, 0.0, -1.0, min.MinValue(), -1.0, -1.0, 1.0, cov);
  return true;

  // // Wrap chi2 function in a function object for the fit
  // // 3 is the number of fit parameters (size of array par)
  // ROOT::Math::Functor fcn(Chi2Function, 4);
  // ROOT::Fit::Fitter fitter;
  // double pStart[4] = {0, 0, 1, 0};
  // fitter.SetFCN(fcn, pStart);
  // fitter.Config().ParSettings(0).SetName("xc");
  // fitter.Config().ParSettings(1).SetName("yc");
  // fitter.Config().ParSettings(2).SetName("R");
  // fitter.Config().ParSettings(3).SetName("dsdz");
  //
  // fitter.Config().ParSettings(0).SetLimits(-200.0, 200.0);
  // fitter.Config().ParSettings(1).SetLimits(-200.0, 200.0);
  // fitter.Config().ParSettings(2).SetLimits(0.0, 150.0);
  // fitter.Config().ParSettings(3).SetLimits(-1.0, 1.0);
  //
  // fitter.Config().ParSettings(0).SetValue(0.0);
  // fitter.Config().ParSettings(1).SetValue(0.0);
  // fitter.Config().ParSettings(2).SetValue(100.0);
  // // fitter.Config().ParSettings(3).SetValue(0.5);
  // fitter.Config().ParSettings(0).Fix();
  // fitter.Config().ParSettings(1).Fix();
  // fitter.Config().ParSettings(2).Fix();
  // fitter.Config().ParSettings(3).Fix();

  // // do the fit
  // bool ok = fitter.FitFCN();
  // if (!ok) {
  //   std::cerr << "Full helix fit did not converge\n";
  //   return ok;
  // }
  //
  // std::cerr << "Chi2Function: " << xc << " " << yc << " " << rad << " " << dsdz  << " "
  //           << z[i] << " " << theta << " " << cos(theta) << " " << sin(theta) << " "
  //           << f << " " << x[i] << " " << dx << " "
  //           << g << " " << y[i] << " " << dy << " "
  //           << dchisq << "\n";ROOT::Fit::FitResult & result = fitter.Result();
  //
  // std::cerr << "Full helix fit successfully converged\n";
  // std::cerr << "dsdz = " << result.Parameter(3) << " +/- " << result.Error(3) << std::endl;
  // std::cerr << "Chisq: " << result.MinFcnValue() << std::endl;

  // result.Print(std::cerr);

  // // Create a helix object with the results
  // std::cerr << "Setting cov matrix...";
  // TMatrixD cov(4, 4);
  // result.GetCovarianceMatrix(cov);
  // std::cerr << " done\n";
  //
  // // Calculate s0
  // double s0 = result.Parameter(2) * tan((y0 - result.Parameter(1))/(x0 - result.Parameter(0)));

  // Fill the helix object to return the results
  // helix = MAUS::SimpleHelix(result.Parameter(0), result.Error(0), \
  //                           result.Parameter(1), result.Error(1), \
  //                           result.Parameter(2), result.Error(2), \
  //                           result.Parameter(3), result.Error(3), \
  //                           s0, 0.0, result.MinFcnValue(), 0.0, result.Prob(), cov);
}
}
