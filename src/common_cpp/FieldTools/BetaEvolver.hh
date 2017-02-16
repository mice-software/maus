/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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
 */

#ifndef BETAEVOLVER_HH
#define BETAEVOLVER_HH

#include <vector>

class BTFieldConstructor;

namespace MAUS {

// Class to integrate beta function through BTFields (using gsl numerical RK4 or other)
class BetaEvolver {
 public:
  static std::vector<double> integrate(double target_z, double z_in, double beta_0,
                                       double alpha_0, double p, double step_size);
  static void rescale_step_4_lattice(double e2_us, double cc_us, double e1_us,
                                     double m2_us, double m1_us, double fc_us,
                                     double fc_ds, double m1_ds, double m2_ds,
                                     double e1_ds, double cc_ds, double e2_ds);
  static bool check_step_4_names();


 private:
  static int z_beta_differential_equation(double z, const double b[2],
                                          double dbdz[2], void* params);

  static BTFieldConstructor* _field;
  static int _maxNSteps;
  static double _p;
  static double _absoluteError;
  static double _relativeError;
};
}
#endif // ifndef BETAEVOLVER_HH

