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
 *
 */

#include <sstream>
#include <vector>
#include <string>

#include "gsl/gsl_odeiv.h"
#include "gsl/gsl_errno.h"

#include "src/legacy/BeamTools/BTFieldConstructor.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/FieldTools/BetaEvolver.hh"

namespace MAUS {

BTFieldConstructor* BetaEvolver::_field = NULL;
double BetaEvolver::_p = 200.;
int BetaEvolver::_maxNSteps = 10000;
double BetaEvolver::_absoluteError = 1e-3;
double BetaEvolver::_relativeError = 1e-3;

bool BetaEvolver::check_step_4_names() {
  _field = Globals::GetMCFieldConstructor();
  BTFieldGroup* mag_field = static_cast<BTFieldGroup*>(_field->GetMagneticField());
  std::string names[] = {
      "EndCoil2_US",
      "CenterCoil_US",
      "EndCoil1_US",
      "MatchCoil2_US",
      "MatchCoil1_US",
      "FocusCoil_US",
      "FocusCoil_DS",
      "MatchCoil1_DS",
      "MatchCoil2_DS",
      "EndCoil1_DS",
      "CenterCoil_DS",
      "EndCoil2_DS",
  };
  if (mag_field->GetFields().size() != 12)
      throw(Exceptions::Exception(Exceptions::recoverable,
                                  "Failed to find the correct number of fields",
                                  "BetaEvolver::rescale_step_4_lattice"));
  for (size_t i = 0; i < 12; ++i) {
      std::string a_name = mag_field->GetFields()[i]->GetName();
      if (a_name != names[i])
          throw(Exceptions::Exception(Exceptions::recoverable,
                "Field name mismatch; expected "+names[i]+" but found "+a_name,
                "BetaEvolver::rescale_step_4_lattice"));
  }
  return true;
}

void BetaEvolver::rescale_step_4_lattice(double e2_us, double cc_us, double e1_us,
                                         double m2_us, double m1_us, double fc_us,
                                         double fc_ds, double m1_ds, double m2_ds,
                                         double e1_ds, double cc_ds, double e2_ds) {
  _field = Globals::GetMCFieldConstructor();
  BTFieldGroup* mag_field = static_cast<BTFieldGroup*>(_field->GetMagneticField());
  mag_field->SetScaleFactor(0, e2_us);
  mag_field->SetScaleFactor(1, cc_us);
  mag_field->SetScaleFactor(2, e1_us);
  mag_field->SetScaleFactor(3, m2_us);
  mag_field->SetScaleFactor(4, m1_us);
  mag_field->SetScaleFactor(5, fc_us);
  mag_field->SetScaleFactor(6, fc_ds);
  mag_field->SetScaleFactor(7, m1_ds);
  mag_field->SetScaleFactor(8, m2_ds);
  mag_field->SetScaleFactor(9, e1_ds);
  mag_field->SetScaleFactor(10, cc_ds);
  mag_field->SetScaleFactor(11, e2_ds);
}

std::vector<double> BetaEvolver::integrate(double target_z, double z_in,
                                           double beta_0, double alpha_0,
                                           double p, double step_size) {
  _field = Globals::GetMCFieldConstructor();
  _p = p;
  const gsl_odeiv_step_type * T = gsl_odeiv_step_rk4;
  gsl_odeiv_step    * step    = gsl_odeiv_step_alloc(T, 3);
  gsl_odeiv_control * control = gsl_odeiv_control_y_new(_absoluteError, _relativeError);
  gsl_odeiv_evolve  * evolve  = gsl_odeiv_evolve_alloc(3);

  // Probably not the most efficient, but only does it once per integrate call
  _absoluteError = (*MAUS::Globals::GetInstance()->GetConfigurationCards())
                                    ["field_tracker_absolute_error"].asDouble();
  _relativeError = (*MAUS::Globals::GetInstance()->GetConfigurationCards())
                                    ["field_tracker_relative_error"].asDouble();

  double z  = z_in;
  gsl_odeiv_system    system  = {z_beta_differential_equation, NULL, 3, NULL};

  double h = step_size;
  int    nsteps = 0;
  double b[] = {beta_0, alpha_0, 0.};
  while (fabs(z-target_z) > 1e-6) {
    nsteps++;
    int status =  gsl_odeiv_evolve_apply(evolve, control, step, &system, &z, target_z, &h, b);
    if (status != GSL_SUCCESS) {
      throw(Exceptions::Exception(Exceptions::recoverable,
                                  "Failed during tracking",
                                  "BTTracker::integrate") );
      break;
    }
    if (nsteps > _maxNSteps) {
      std::stringstream ios;
      ios << "Killing beta with step size " << h << " at step " << nsteps
          << " of " << _maxNSteps << "\n"
          << "b: " << b[0] << " b': " << b[1] << std::endl;
      throw(Exceptions::Exception(Exceptions::recoverable, ios.str()+
            " Exceeded maximum number of steps\n", "BetaEvolver::integrate") );
      break;
    }
  }
  gsl_odeiv_evolve_free(evolve);
  gsl_odeiv_control_free(control);
  gsl_odeiv_step_free(step);
  std::vector<double> beta_out(2, 0);
  beta_out[0] = b[0]; // beta
  beta_out[1] = b[1]; // beta prime
  beta_out[2] = b[2]; // phi
  return beta_out;
}

int BetaEvolver::z_beta_differential_equation(double z, const double b[2],
                                              double dbdz[2], void* params) {
  double field[6] = {0., 0., 0., 0., 0., 0.};
  double xfield[4] = {0., 0., z, 0.};
  _field->GetFieldValue(xfield, field);
  double kappa = field[2]/2./_p*300.;
  // beta'
  dbdz[0] = b[1];
  // beta''
  dbdz[1] = (b[1]*b[1]-4*b[0]*b[0]*kappa*kappa+4)/2/b[0];
  dbdz[2] = 1./b[0];

  return GSL_SUCCESS;
}
}
