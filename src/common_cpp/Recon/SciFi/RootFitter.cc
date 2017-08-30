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
#include <cmath>
#include <functional>

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
                     const std::vector<double>& err,
                     MAUS::SimpleCircle& circ, TMatrixD& cov_matrix) {

  auto Chi2Function = [&x, &y, &err](const double *par) {
    // Minimisation function computing the sum of squares of residuals
    // looping over the points
    double f = 0.0;
    for (size_t i = 0; i < x.size(); i++) {
        double u = (x[i] - par[0]);
        double v = y[i] - par[1];
        double dr = par[2] - std::sqrt(u*u+v*v);
        f += (dr*dr) / (err[i]*err[i]);
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
                    const std::vector<double>& z, const double* pStart,
                    MAUS::SimpleHelix& helix, int handedness, double cut) {

  // Pull out x and y at the tracker reference station
  double x0 = x[0];
  double y0 = y[0];

  // This is a C++11 lambda function, which defines the quantity to be minimised by MINUIT,
  // using a sum over squared residuals looping over the seed spacepoints
  std::function<double(const double*)> //NOLINT(*)
      Chi2Function = [&x, &y, &z, &x0, &y0](const double *par) {
    double xc = par[0];
    double yc = par[1];
    double rad = par[2];
    double dsdz = par[3];
    double chisq = 0.0;
    for (size_t i = 0; i < x.size(); i++) {
        double theta = (z[i]*dsdz) / rad;
        double f = xc + (x0 - xc)*cos(theta) - (y0 - yc)*sin(theta); // x model prediction
        double g = yc + (y0 - yc)*cos(theta) + (x0 - xc)*sin(theta); // y model prediction
        double dx = f - x[i];
        double dy = g - y[i];
        double dchisq = (dx*dx) + (dy*dy);
        chisq += dchisq;
    }
    return chisq;
  };

  // Set up the minimiser - ROOT::Math::Minimizer interface to MINUIT2, Migrad algorithm
  ROOT::Minuit2::Minuit2Minimizer min(ROOT::Minuit2::kMigrad);
  min.SetMaxFunctionCalls(1000000);
  min.SetMaxIterations(100000);
  min.SetTolerance(0.001);
  ROOT::Math::Functor fcn(Chi2Function, 4);
  min.SetFunction(fcn);

  // Determine the approximate dsdz value which corresponds to the global minimum of the chisq func
  double dsdz_seed = LocateGlobalChiSqMinimum(pStart, handedness, cut, Chi2Function);

  // Set the variables to be minimized
  min.SetFixedVariable(0, "xc", pStart[0]);
  min.SetFixedVariable(1, "yc", pStart[1]);
  min.SetFixedVariable(2, "R",  pStart[2]);
  double step[4] = {0.01, 0.01, 0.01, 0.01};
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

  // Populate the helix
  std::vector<double> params = {xs[0], xs[1], xs[2], xs[3], s0};
  std::vector<double> errs = {errors[0], errors[1], errors[2], errors[3], 0.0};
  int zsize = static_cast<int>(z.size());
  std::vector<int> ndfs = {(2*zsize - 3), (2*zsize - 1), (2*zsize - 5)};
  std::vector<double> chisqs = {-1.0, min.MinValue(), -1.0};
  helix = MAUS::SimpleHelix(params, errs, ndfs, chisqs, -1.0, cov);
  return true;
}

double LocateGlobalChiSqMinimum(const double* pStart, int handedness, double cut,
                                std::function<double(const double*)> Chi2Function) { //NOLINT(*)
  double nsteps = 100.0; // How many pieces do we slice the chisq function into
  if (pStart[2] < 10.0) nsteps = 200.0; // Search more carefully when the radius is below 10mm
  double lower_limit = -1.0;  // dsdz bounds
  double upper_limit = 1.0;
  if (handedness == 1) {
    lower_limit = 0.0;
    upper_limit = 1.0;
  } else if (handedness == -1) {
    lower_limit = -1.0;
    upper_limit = 0.0;
  }
  double step_size = (upper_limit - lower_limit) / nsteps;
  double min_chisq = 10000000000.0; // Just a huge number to start
  double dsdz_seed = pStart[3];
  double threshold = 50.0; // min chisq for dsdz to be considered as candidate
  if (threshold < cut) threshold = cut; // Musn't have a lower value than the patrec level cut
  std::vector<double> candidate_seeds;
  for (double seed = lower_limit; seed < upper_limit + step_size; seed += step_size) {
    const double params[4] = {pStart[0], pStart[1], pStart[2], seed};
    double resid = Chi2Function(params);
    if (resid < min_chisq) {
      min_chisq = resid;
      dsdz_seed = seed;
    }
    // Can only look for other candidates if we have the expected dsdz sign (the handedness)
    if ((handedness == 1 || handedness == -1) && (resid < threshold)) {
      candidate_seeds.push_back(seed);
    }
  }
  // Check if we have other viable dsdz_seed values which are better based on proximity to 0
  // (in value of the actual seed, not the corresponding chisq). If no candidates pass the
  // threshold cut just stick with the smallest chisq value from above
  if (candidate_seeds.size() > 0) {
    double new_dsdz_seed = 2.0; // outside allowed range to start
    for (auto cs : candidate_seeds) {
      // If candidate value has the expected handedness
      if (((cs < 0.0) && (handedness < 0)) || ((cs > 0.0) && (handedness > 0))) {
          if (fabs(cs) < fabs(new_dsdz_seed)) new_dsdz_seed = cs;
      }
    }
    if (new_dsdz_seed <= 1.0) { // Check we are not still on new_dsdz_seed = 2.0
      dsdz_seed = new_dsdz_seed;
    }
  }
  return dsdz_seed;
}
}
