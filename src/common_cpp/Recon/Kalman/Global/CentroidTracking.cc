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
 * along with MAUS.  If not, see <http://   www.gnu.org/licenses/>.
 *
 * Copyright Chris Rogers, 2016
 */

#include <math.h>

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

#include "gsl/gsl_odeiv.h"
#include "gsl/gsl_errno.h"

#include "Geant4/G4Navigator.hh"
#include "Geant4/G4TransportationManager.hh"
#include "Geant4/G4VPhysicalVolume.hh"
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4Material.hh"

#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Utils/Squeak.hh"
#include "src/common_cpp/Simulation/GeometryNavigator.hh"
#include "src/common_cpp/Recon/Global/MaterialModelDynamic.hh"
#include "src/common_cpp/Recon/Global/MaterialModelAxialLookup.hh"
#include "src/common_cpp/Recon/Kalman/Global/ErrorTrackingControl.hh"
#include "src/common_cpp/Recon/Kalman/Global/ErrorTrackingControlLookup.hh"
#include "src/common_cpp/Recon/Kalman/Global/CentroidTracking.hh"

namespace MAUS {
namespace Kalman {
namespace Global {

std::stringstream CentroidTracking::tracking_fail;

const double CentroidTracking::_float_tolerance = 1e-15;

CentroidTracking* CentroidTracking::_tz_for_propagate = NULL;

CentroidTracking::CentroidTracking()
  : _field(Globals::GetMCFieldConstructor()), _charge(1.) {
}

CentroidTracking::TrackingSuccess CentroidTracking::Propagate(double x[8], double target_z) {
  double test = x[0]+x[1]+x[2]+x[3];
  if (test != test) {
      Squeak::mout(Squeak::warning) <<
            "Position Not A Number in CentroidTracking::Propagate" << std::endl;
      return failure;
  }
  _mass_squared = x[4]*x[4] - x[5]*x[5] - x[6]*x[6] - x[7]*x[7];
  if (_mass_squared != _mass_squared) {
      Squeak::mout(Squeak::warning) << "Momentum Not a Number in "
                                    << "CentroidTracking::Propagate" << std::endl;
      return failure;
  }
  if (_mass_squared < -_float_tolerance) {
      Squeak::mout(Squeak::warning) << "Negative mass in CentroidTracking::Propagate" << std::endl;
      return failure;
  }
  const gsl_odeiv_step_type * T = gsl_odeiv_step_rk4;
  gsl_odeiv_step    * step    = gsl_odeiv_step_alloc(T, 8);
  gsl_odeiv_control * control = NULL;
  if (_track_model == em_forwards_dynamic ||
      _track_model == em_backwards_dynamic) {
      if (_geometry == geant4) {
          control = ErrorTrackingControl::gsl_odeiv_control_et_new(
                                               _min_step_size, _max_step_size);
      } else if (_geometry == axial_lookup) {
          control = ErrorTrackingControlLookup::gsl_odeiv_control_lookup_et_new(
                                               _min_step_size, _max_step_size);
      }
  }
  gsl_odeiv_evolve  * evolve  = gsl_odeiv_evolve_alloc(8);
  gsl_odeiv_system    system  =
                            {CentroidTracking::EquationsOfMotion, NULL, 8, NULL};

  double z = x[3];
  _gsl_h = _min_step_size;
  if (z > target_z) {
      _gsl_h *= -1;  // stepping backwards...
      _charge = -1;
  } else {
      _charge = 1;  // stepping forwards...
  }
  int    nsteps = 0;
  _tz_for_propagate = this;
  tracking_fail.str("");  // clear the error stream
  while (fabs(z-target_z) > _float_tolerance) {
    nsteps++;
    int status =  gsl_odeiv_evolve_apply
                    (evolve, NULL, step, &system, &z, target_z, &_gsl_h, x);
    if (control != NULL) {
        control->type->hadjust(control->state, 0, 0, x, NULL, NULL, &_gsl_h);
    }

    if (nsteps > _max_n_steps) {
        tracking_fail << "Exceeded maximum number of steps\n";
        status = failure;
    }
    if (status != GSL_SUCCESS) {
        tracking_fail << "Killing tracking with step size " <<_gsl_h
            << " at step " << nsteps << " of " << _max_n_steps << "\n";
        print(tracking_fail, x);
    }
    if (status == failure) {
        Squeak::mout(Squeak::warning) << tracking_fail.str() << std::endl;
        return failure;
    }
  }
  gsl_odeiv_evolve_free(evolve);
  gsl_odeiv_control_free(control);
  gsl_odeiv_step_free(step);
  return success;
}

int CentroidTracking::EquationsOfMotion(double z,
                                     const double x[8],
                                     double dxdz[8],
                                     void* params) {
  if (fabs(x[7]) < _float_tolerance) {
    // z-momentum is 0
      tracking_fail << "pz " << x[7] << " was less than float tolerance "
                    << _float_tolerance << "\n";
      return GSL_FAILURE;
  }
  for (size_t i = 0; i < 8; ++i) {
      if (x[i] != x[i]) {
          tracking_fail << "Detected nan in input i: " << i
                        << " value: " << x[i] << "\n";
          print(tracking_fail, x);
          return GSL_FAILURE;
      }
  }
  if (_tz_for_propagate == NULL) {
      tracking_fail << "_tz_for_propagate was NULL\n";
      return GSL_FAILURE;
  }
  int em_success = EMEquationOfMotion(z, x, dxdz, params);
  if (em_success != GSL_SUCCESS) {
      tracking_fail << "EM transport failed\n";
      return em_success;
  }

  if (_tz_for_propagate->_eloss_model == no_eloss) {
      return GSL_SUCCESS;
  }

  int material_success = MaterialEquationOfMotion(z, x, dxdz, params);
  if (material_success != GSL_SUCCESS) {
      tracking_fail << "Material transport failed\n";
      return material_success;
  }
  return GSL_SUCCESS;
}

int CentroidTracking::EMEquationOfMotion(double z,
                                            const double x[8],
                                            double dxdz[8],
                                            void* params) {
  double field[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  double xfield[4] = {x[1], x[2], x[3], x[0]};
  _tz_for_propagate->_field->GetFieldValue(xfield, field);

  double direction = -1.;
  if ((_tz_for_propagate->_track_model == em_forwards_dynamic ||
      _tz_for_propagate->_track_model == em_forwards_static) &&
      _tz_for_propagate->_gsl_h < 0.) {
      // forwards track propagating in the backwards direction
      direction = -1.;
  } else if (_tz_for_propagate->_gsl_h > 0.) {
      // backwards track propagating in the forwards direction
      direction = +1.;
  }
  double dir = fabs(x[7])/x[7];
  double charge = _tz_for_propagate->_charge;
  double dtdz = x[4]/x[7];
  dxdz[0] = dtdz/c_l; // dt/dz
  dxdz[1] = x[5]/x[7]; // dx/dz = px/pz
  dxdz[2] = x[6]/x[7]; // dy/dz = py/pz
  dxdz[3] = 1.0; // dz/dz
  // dE/dz only contains electric field as B conserves energy, not relevant at
  // least in step 4 as all fields are static.
  dxdz[4] = (dxdz[1]*charge*field[3] + dxdz[2]*charge*field[4] +
             charge*field[5])*dir; // dE/dz
  // dpx/dz = q*c*(dy/dz*Bz - dz/dz*By) + q*Ex*dt/dz
  dxdz[5] = direction*charge*c_l*(dxdz[2]*field[2] - dxdz[3]*field[1])
            + charge*field[3]*dtdz*dir; // dpx/dz
  dxdz[6] = direction*charge*c_l*(dxdz[3]*field[0] - dxdz[1]*field[2])
            + charge*field[4]*dtdz*dir; // dpy/dz
  dxdz[7] = direction*charge*c_l*(dxdz[1]*field[1] - dxdz[2]*field[0])
            + charge*field[5]*dtdz*dir; // dpz/dz

  return GSL_SUCCESS;
}

int CentroidTracking::MaterialEquationOfMotion(double z,
                                                  const double x[8],
                                                  double dxdz[8],
                                                  void* params) {
    // Must be called after EMEquationOfMotion
    MaterialModel* material = NULL;
    if (_tz_for_propagate->_geometry == geant4) {
        material = new MaterialModelDynamic(x[1], x[2], x[3]);
    } else if (_tz_for_propagate->_geometry == axial_lookup) {
        material = new MaterialModelAxialLookup(x[1], x[2], x[3]);
    } else {
        throw Exceptions::Exception(Exceptions::recoverable,
                              "Did not recognise material model",
                              "CentroidTracking::MaterialEquationOfMotion");
    }
    if (material->GetMaterial() == NULL) { // material is not enabled
        delete material;
        return GSL_SUCCESS;
    }

    double energy = sqrt(x[4]*x[4]);
    double p = sqrt(x[5]*x[5]+x[6]*x[6]+x[7]*x[7]);
    double mass = sqrt(energy*energy-p*p);
    double charge = _tz_for_propagate->_charge;
    double dEdz = 0.;
    if (_tz_for_propagate->_eloss_model == bethe_bloch_forwards) {
        dEdz = material->dEdx(energy, mass, charge); // should be negative
    } else if (_tz_for_propagate->_eloss_model == bethe_bloch_backwards) {
        // should be negative; during tracking backwards dz is negative so dEdz
        // gets handled properly
        dEdz = -material->dEdx(energy, mass, charge);
    } else {
        dEdz = 0.; // no eloss
    }

    // E^2 = p^2+m^2; 2E dE/dz = 2p dp/dz
    double dpdz = energy/p*dEdz;
    dxdz[4] += dEdz;
    dxdz[5] += dpdz*x[5]/p;
    dxdz[6] += dpdz*x[6]/p;
    dxdz[7] += dpdz*x[7]/p;

    delete material;
    return GSL_SUCCESS;
}

void CentroidTracking::SetField(const BTField* field) {
    if (field == NULL) {
        _field = Globals::GetMCFieldConstructor();
    } else {
        _field = field;
    }
}

void CentroidTracking::SetGeometryModel(std::string geometry_model) {
    if (geometry_model == "geant4") {
        SetGeometryModel(geant4);
    } else if (geometry_model == "axial_lookup") {
        SetGeometryModel(axial_lookup);
    } else {
        throw Exceptions::Exception(Exceptions::recoverable,
                          "Failed to recognise geometry model '"+geometry_model
                          +"' Should be 'geant4' or 'axial_lookup'",
                          "CentroidTracking::SetGeometryModel");
    }
}

std::ostream& CentroidTracking::print(std::ostream& out, const double* x) {
    out << "t: " << x[0] << " pos: " << x[1] << " " << x[2] << " " << x[3] << "\n"
        << "E: " << x[4] << " mom: " << x[5] << " " << x[6] << " " << x[7] << std::endl;
    return out;
}

void CentroidTracking::SetMinStepSize(double min_step_size) {
    // step size is always positive (direction is determined automatically)
    min_step_size = fabs(min_step_size);
    if (min_step_size > _max_step_size) {
        throw Exceptions::Exception(Exceptions::recoverable,
                         "Min step size should be < max step size",
                         "CentroidTracking::SetMinStepSize");
    }
    _min_step_size = min_step_size;
}

void CentroidTracking::SetMaxStepSize(double max_step_size) {
    // step size is always positive (direction is determined automatically)
    max_step_size = fabs(max_step_size);
    if (_min_step_size > max_step_size) {
        throw Exceptions::Exception(Exceptions::recoverable,
                          "Min step size should be < max step size",
                          "CentroidTracking::SetMaxStepSize");
    }
    _max_step_size = max_step_size;
}

void CentroidTracking::SetTrackingModel(std::string tracking_model) {
    if (tracking_model == "em_forwards_dynamic") {
        SetTrackingModel(em_forwards_dynamic);
    } else if (tracking_model == "em_backwards_dynamic") {
        SetTrackingModel(em_backwards_dynamic);
    } else if (tracking_model == "em_forwards_static") {
        SetTrackingModel(em_forwards_static);
    } else if (tracking_model == "em_backwards_static") {
        SetTrackingModel(em_backwards_static);
    } else {
        throw Exceptions::Exception(
                            Exceptions::recoverable,
                            "Did not recognise tracking model "+tracking_model,
                            "CentroidTracking::SetTrackingModel");
    }
}

} // namespace Global
} // namespace Kalman
} // namespace MAUS

