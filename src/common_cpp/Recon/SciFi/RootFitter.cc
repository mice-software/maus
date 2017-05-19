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

// ROOT headers
#include "TLinearFitter.h"
#include "TMatrixTBase.h"
#include "TMath.h"
#include "Math/Functor.h"
#include "Fit/Fitter.h"

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
}
