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
#include "TMath.h"
#include "Math/Functor.h"
#include "Fit/Fitter.h"

// MAUS headers
#include "src/common_cpp/Recon/SciFi/RootFitter.hh"

namespace RootFitter {

bool FitCircle(const std::vector<double>& x, const std::vector<double>& y,
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
        // std::cerr << "i, x, y, dr and f: " << i << " " << x[i] << " " << y[i] << " " << dr << " " << f << std::endl;
    }
    return f;
  };

  // Wrap chi2 funciton in a function object for the fit
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
  circ.set_chisq(result.Chi2());
  result.GetCovarianceMatrix(cov_matrix);
  
  return true;
}
}
