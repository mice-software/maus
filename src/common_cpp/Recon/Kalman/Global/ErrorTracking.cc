#include <math.h>

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
#include "src/common_cpp/Simulation/GeometryNavigator.hh"
#include "src/legacy/Interface/Squeak.hh"
#include "src/common_cpp/Recon/Global/MaterialModelDynamic.hh"
#include "src/common_cpp/Recon/Global/MaterialModelAxialLookup.hh"
#include "src/common_cpp/Recon/Kalman/Global/ErrorTrackingControl.hh"
#include "src/common_cpp/Recon/Kalman/Global/ErrorTrackingControlLookup.hh"
#include "src/common_cpp/Recon/Kalman/Global/ErrorTracking.hh"

namespace MAUS {
namespace Kalman {
namespace Global {

std::stringstream ErrorTracking::tracking_fail;

const double ErrorTracking::_float_tolerance = 1e-15;

ErrorTracking* ErrorTracking::_tz_for_propagate = NULL;

ErrorTracking::ErrorTracking()
  : _field(Globals::GetMCFieldConstructor()), _dx(1.), _dy(1.), _dz(1.),
    _dt(1.), _charge(1.), _matrix(6, std::vector<double>(6, 0.)) {
}

void ErrorTracking::Propagate(double x[29], double target_z) {

  _mass_squared = x[4]*x[4] - x[5]*x[5] - x[6]*x[6] - x[7]*x[7];
  if (_mass_squared < -_float_tolerance || _mass_squared != _mass_squared) {
    throw(MAUS::Exception(MAUS::Exception::recoverable,
          "Mass undefined in stepping function",
          "ErrorTracking::Propagate"));
  }
  const gsl_odeiv_step_type * T = gsl_odeiv_step_rk4;
  gsl_odeiv_step    * step    = gsl_odeiv_step_alloc(T, 29);
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
  gsl_odeiv_evolve  * evolve  = gsl_odeiv_evolve_alloc(29);
  gsl_odeiv_system    system  =
                      {ErrorTracking::EquationsOfMotion, NULL, 29, NULL};

  double z = x[3];
  _gsl_h = fabs(_min_step_size);  // ignore _step_size sign, we "auto detect"
  if (z > target_z) {
      _gsl_h *= -1;  // stepping backwards...
      _charge = -1;
  } else {
      _charge = 1;  // stepping forwards...
  }
  int    nsteps = 0;
  _tz_for_propagate = this;
  tracking_fail.str("");  // clear the error stream
  while(fabs(z-target_z) > _float_tolerance) {
    nsteps++;
    // std::cerr << "step in z " << z << " dz " << _gsl_h << " target " << target_z << std::endl;
    int status =  gsl_odeiv_evolve_apply
                    (evolve, NULL, step, &system, &z, target_z, &_gsl_h, x);
    if (control != NULL) {
        control->type->hadjust(control->state, 0, 0, x, NULL, NULL, &_gsl_h);
    }
    // std::cerr << "step out z " << z << " dz " << _gsl_h << " target " << target_z << std::endl;
    if (nsteps > _max_n_steps) {
        tracking_fail << "Exceeded maximum number of steps\n"; 
        status = GSL_FAILURE;
    }
    if (status != GSL_SUCCESS) {
        tracking_fail << "Killing tracking with step size " <<_gsl_h
            << " at step " << nsteps << " of " << _max_n_steps << "\n";
        print(tracking_fail, x);
        throw(MAUS::Exception(MAUS::Exception::recoverable,
                            tracking_fail.str(),
                            "ErrorTracking::Propagate") );
    }
  }
  gsl_odeiv_evolve_free (evolve);
  gsl_odeiv_control_free(control);
  gsl_odeiv_step_free   (step);
}

void ErrorTracking::PropagateTransferMatrix(double x[44], double target_z) {
  _mass_squared = x[4]*x[4] - x[5]*x[5] - x[6]*x[6] - x[7]*x[7];
  if (_mass_squared < -_float_tolerance || _mass_squared != _mass_squared) {
    throw(MAUS::Exception(MAUS::Exception::recoverable,
          "Mass undefined in stepping function",
          "ErrorTracking::Propagate"));
  }
  const gsl_odeiv_step_type * T = gsl_odeiv_step_rk4;
  gsl_odeiv_step    * step    = gsl_odeiv_step_alloc(T, 44);
  gsl_odeiv_evolve  * evolve  = gsl_odeiv_evolve_alloc(44);
  gsl_odeiv_system    system  =
                       {ErrorTracking::EquationsOfMotion, NULL, 44, NULL};

  double z = x[3];
  _gsl_h = fabs(_max_step_size);  // ignore _step_size sign, we "auto detect"
  if (z > target_z)
      _gsl_h *= -1;  // stepping backwards...
  int nsteps = 0;
  _tz_for_propagate = this;
  while(fabs(z-target_z) > _float_tolerance) {
    nsteps++;
    
    int status =  gsl_odeiv_evolve_apply
                        (evolve, NULL, step, &system, &z, target_z, &_gsl_h, x);
    
    if(status != GSL_SUCCESS)
    {
        tracking_fail << "Killing tracking with step size " << _gsl_h
                      << " at step " << nsteps
                      << " of " << _max_n_steps << "\n";
        print(tracking_fail, x);
        throw(MAUS::Exception(MAUS::Exception::recoverable,
                            tracking_fail.str(),
                            "ErrorTracking::Propagate") );
        break;
    }
    if(nsteps > _max_n_steps)
    {
        tracking_fail << "Killing tracking with step size " << _gsl_h
                      << " at step " << nsteps << " of " << _max_n_steps << "\n"
                      << "\nExceeded maximum number of steps\n";
        print(tracking_fail, x);
        throw(MAUS::Exception(MAUS::Exception::recoverable,
                              tracking_fail.str(),
                              "ErrorTracking::Propagate") );
        break;
    }
  }

  gsl_odeiv_evolve_free (evolve);
  gsl_odeiv_step_free   (step);


}

int ErrorTracking::EquationsOfMotion(double z, 
                                           const double x[29],
                                           double dxdz[29],
                                           void* params) {
  if (fabs(x[7]) < _float_tolerance) {
    // z-momentum is 0
      tracking_fail << "pz " << x[7] << " was less than float tolerance " << _float_tolerance << "\n";
      return GSL_FAILURE;
  }
  for (size_t i = 0; i < 29; ++i) {
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

  // If no material processes are active, don't run MaterialEquationsOfMotion
  // at all
  if (_tz_for_propagate->_eloss_model == no_eloss &&
      _tz_for_propagate->_mcs_model == no_mcs &&
      _tz_for_propagate->_estrag_model == no_estrag) {
      return GSL_SUCCESS;
  }

  int material_success = MaterialEquationOfMotion(z, x, dxdz, params);
  if (material_success != GSL_SUCCESS) {
      tracking_fail << "Material transport failed\n";
      return material_success;
  }
  Squeak::mout(Squeak::debug) << "    ErrorTracking::EquationsOfMotion Values ";
  for (size_t i = 0; i < 8; ++i)
    Squeak::mout(Squeak::debug) << x[i] << " ";
  Squeak::mout(Squeak::debug) << "\n    ErrorTracking::EquationsOfMotion Derivatives ";
  for (size_t i = 0; i < 8; ++i)
      Squeak::mout(Squeak::debug) << dxdz[i] << " ";
  Squeak::mout(Squeak::debug) << std::endl;
  return GSL_SUCCESS;
}

int ErrorTracking::EMEquationOfMotion(double z,
                                            const double x[29],
                                            double dxdz[29],
                                            void* params) {
  double field[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  double xfield[4] = {x[1], x[2], x[3], x[0]};
  _tz_for_propagate->_field->GetFieldValue(xfield, field);

  double direction = 1.;
  if ((_tz_for_propagate->_track_model == em_forwards_dynamic || 
      _tz_for_propagate->_track_model == em_backwards_dynamic) &&
      _tz_for_propagate->_gsl_h < 0.) {
      direction = -1.;
  } else if (_tz_for_propagate->_gsl_h > 0.) {
      direction = -1.;
  }
  double charge = _tz_for_propagate->_charge;
  double dtdz = x[4]/x[7];
  double dir = fabs(x[7])/x[7]; // direction of motion
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
  size_t x_index = 8;
  _tz_for_propagate->UpdateTransferMatrix(x, direction);
  std::vector< std::vector<double> > var(6, std::vector<double>(6, 0.));
  for (size_t i = 0; i < 6; ++i) {
      for (size_t j = i; j < 6; ++j) {
          var[i][j] = var[j][i] = x[x_index];
          ++x_index;
      }
  }

  // m V m^T
  std::vector< std::vector<double> > deriv_var(6, std::vector<double>(6, 0.));
  for (size_t i = 0; i < 6; ++i) {
      for (size_t j = 0; j < 6; ++j) {
          for (size_t k = 0; k < 6; ++k) {
              deriv_var[i][j] += var[i][k]*_tz_for_propagate->_matrix[j][k]; // V dM^T
          }
      }
  }
  std::vector< std::vector<double> > deriv_var_2(6, std::vector<double>(6, 0.));
  for (size_t i = 0; i < 6; ++i) {
      for (size_t j = 0; j < 6; ++j) {
          for (size_t k = 0; k < 6; ++k) {
              deriv_var_2[i][j] += _tz_for_propagate->_matrix[i][k]*deriv_var[k][j]; // dM V dM^T
              deriv_var_2[i][j] += _tz_for_propagate->_matrix[i][k]*var[k][j]; // dM V dM^T + dM V
          }
          deriv_var_2[i][j] += deriv_var[i][j];  // dM V dM^T + dM V + V dM^T
      };
  }

  x_index = 8;
  for (size_t i = 0; i < 6; ++i) {
      for (size_t j = i; j < 6; ++j) {
          dxdz[x_index] = deriv_var_2[i][j]; // dM V dM^T + dM V + V dM^T
          x_index++;
      }
  }
  return GSL_SUCCESS;
}


int ErrorTracking::MatrixEquationOfMotion(double z,
                                                const double x[44],
                                                double dxdz[44],
                                                void* params) {
  double field[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  double xfield[4] = {x[1], x[2], x[3], x[0]};
  _tz_for_propagate->_field->GetFieldValue(xfield, field);

  double charge = _tz_for_propagate->_charge;
  double dtdz = x[4]/x[7];
  double dir = fabs(x[7])/x[7]; // direction of motion
  dxdz[0] = dtdz/c_l; // dt/dz
  dxdz[1] = x[5]/x[7]; // dx/dz = px/pz
  dxdz[2] = x[6]/x[7]; // dy/dz = py/pz
  dxdz[3] = 1.0; // dz/dz
  // dE/dz only contains electric field as B conserves energy, not relevant at
  // least in step 4 as all fields are static.
  dxdz[4] = (dxdz[1]*charge*field[3] + dxdz[2]*charge*field[4] +
             charge*field[5])*dir; // dE/dz
  // dpx/dz = q*c*(dy/dz*Bz - dz/dz*By) + q*Ex*dt/dz
  dxdz[5] = charge*c_l*(dxdz[2]*field[2] - dxdz[3]*field[1])
            + charge*field[3]*dtdz*dir; // dpx/dz
  dxdz[6] = charge*c_l*(dxdz[3]*field[0] - dxdz[1]*field[2])
            + charge*field[4]*dtdz*dir; // dpy/dz
  dxdz[7] = charge*c_l*(dxdz[1]*field[1] - dxdz[2]*field[0])
            + charge*field[5]*dtdz*dir; // dpz/dz
  double direction = 1.;
  if ((_tz_for_propagate->_track_model == em_forwards_dynamic || 
      _tz_for_propagate->_track_model == em_backwards_dynamic) &&
      _tz_for_propagate->_gsl_h < 0.) {
      direction = -1.;
  } else if (_tz_for_propagate->_gsl_h > 0.) {
      direction = -1.;
  }

  _tz_for_propagate->UpdateTransferMatrix(x, direction);
  std::vector< std::vector<double> > dMdz(6, std::vector<double>(6, 0.));
  std::vector< std::vector<double> > M(6, std::vector<double>(6, 0.));
  std::vector< std::vector<double> > dM = _tz_for_propagate->_matrix;
  // dM/dz = dM M
  size_t matrix_index = 8;
  for (size_t i = 0; i < 6; ++i)
      for (size_t j = 0; j < 6; ++j) {
          M[i][j] = x[matrix_index];
          ++matrix_index;
      }
  matrix_index = 8;
  for (size_t i = 0; i < 6; ++i) {
      for (size_t j = 0; j < 6; ++j) {
          for (size_t k = 0; k < 6; ++k) {
              dMdz[i][j] += dM[i][k]*M[k][j];
          }
          dxdz[matrix_index] = dMdz[i][j];
          ++matrix_index;
      }
  }
  return GSL_SUCCESS;
}


int ErrorTracking::MaterialEquationOfMotion(double z,
                                                  const double x[29],
                                                  double dxdz[29],
                                                  void* params) {
    // Must be called after EMEquationOfMotion
    MaterialModel* material = NULL;
    if (_tz_for_propagate->_geometry == geant4) {
        material = new MaterialModelDynamic(x[1], x[2], x[3]);
    } else if (_tz_for_propagate->_geometry == axial_lookup) {
        material = new MaterialModelAxialLookup(x[1], x[2], x[3]);
    } else {
        throw MAUS::Exception(Exception::recoverable,
                              "Did not recognise material model",
                              "ErrorTracking::MaterialEquationOfMotion");
    }
    if (material->GetMaterial() == NULL) { // material is not enabled
        delete material;
        return GSL_SUCCESS;
    }
    //std::cerr << "ErrorTracking::MAterialEqueationOfMotion MaterierlaModelAxialLookup Getting material" << std::endl;
    //std::cerr << "    x: " << x[1] << " y: " << x[2] << " z: " << x[3] << std::endl;
    //std::cerr << "    Found name: " << material << std::flush << " " <<  material->GetMaterial() << std::flush << " " << material->GetMaterial()->GetName() << std::endl;

    double energy = sqrt(x[4]*x[4]);
    double p = sqrt(x[5]*x[5]+x[6]*x[6]+x[7]*x[7]);
    double mass = sqrt(energy*energy-p*p);
    double charge = _tz_for_propagate->_charge;
    double dEdz;
    double d2EdzdE;
    double dtheta2dz;
    double destragdz;
    if (_tz_for_propagate->_eloss_model == bethe_bloch_forwards) {
        dEdz = material->dEdx(energy, mass, charge); // should be negative
        d2EdzdE = 0.; // material->d2EdxdE(energy, mass, charge);
    } else if (_tz_for_propagate->_eloss_model == bethe_bloch_backwards) {
        dEdz = -material->dEdx(energy, mass, charge); // should be negative; during tracking backwards dz is negative so dEdz gets handled properly
        d2EdzdE = 0.; // material->d2EdxdE(energy, mass, charge);
    } else {
        dEdz = 0.; // no eloss
        d2EdzdE = 0;
    }

    if (_tz_for_propagate->_estrag_model == estrag_forwards) {
        destragdz = material->estrag2(energy, mass, charge);
    } else if (_tz_for_propagate->_estrag_model == estrag_backwards) {
        destragdz = -material->estrag2(energy, mass, charge);
    } else {
        destragdz = 0.;
    } 

    if (_tz_for_propagate->_mcs_model == moliere_forwards) {
        dtheta2dz = material->dtheta2dx(energy, mass, charge);
    } else if (_tz_for_propagate->_mcs_model == moliere_backwards) {
        dtheta2dz = -material->dtheta2dx(energy, mass, charge);
    } else {
        dtheta2dz = 0.;
    }

    // E^2 = p^2+m^2; 2E dE/dz = 2p dp/dz
    double dpdz = energy/p*dEdz;
    dxdz[4] += dEdz;
    dxdz[5] += dpdz*x[5]/p;
    dxdz[6] += dpdz*x[6]/p;
    dxdz[7] += dpdz*x[7]/p;

    // d/dz <E^2> = d<estrag2>/dz + 2E d/dE dE/dz
    dxdz[23] += destragdz+2*x[4]*d2EdzdE;

    // product rule - <p_x^2> = <x'^2> p_z^2
    // d<px^2>/dz = d<x'^2>/dz p_z^2 + 2 <x'^2> p_z dp_z/dz 
    //            = d<x'^2>/dz p_z^2 + 2 <p_x^2>/p_z dp_z/dz
    dxdz[26] += 2*dpdz/x[7]*x[26] + dtheta2dz*x[7]*x[7];
    dxdz[28] += 2*dpdz/x[7]*x[28] + dtheta2dz*x[7]*x[7];

    delete material;
    
    //std::cerr << "MATERIAL z: " << x[3]
              //<< " dEdx " << material->dEdx(energy, mass, charge)
    //          << " x26/28: " << x[26] << " " << x[28]
    //          << " dxdz26/28: " << dxdz[26] << " " << dxdz[28]
    //          << " dpdz: " << dpdz << " pz: " << x[7]
    //          << " dt2dz: " << dtheta2dz << std::endl;
              //<< " mcs_model: " << _tz_for_propagate->_mcs_model << std::endl;
    
    return GSL_SUCCESS;
}

void ErrorTracking::UpdateTransferMatrix(const double* x,
                                               double direction) {
  // dbx/dx, dby/dx, dbz/dx, dbx/dy, dby/dy, dbz/dy
  double xfield[4] = {x[1], x[2], x[3], x[0]};
  double field_value[] = {0., 0., 0., 0., 0., 0.};
  _field->GetFieldValue(xfield, field_value);
  double dbdx[6] = {0., 0., 0., 0., 0., 0.};
  FieldDerivative(xfield, dbdx);

  double energy = x[4];
  double px = x[5];
  double py = x[6];
  double pz = x[7];

  double bz = field_value[2];

  double dbxdx = dbdx[0];
  double dbydx = dbdx[1];
  double dbzdx = dbdx[2];
  double dbxdy = dbdx[3];
  double dbydy = dbdx[4];
  double dbzdy = dbdx[5];

  double m_0_3 = 1./pz/c_l - energy*energy/pz/pz/pz/c_l; // 1/(beta gamma)^2 /c_l / pz when px, py = 0
  double m_0_4 = energy*px/pz/pz/pz/c_l;
  double m_0_5 = energy*py/pz/pz/pz/c_l;

  double m_1_3 = -px*energy/pz/pz/pz; // px d(1/pz)/dE = - px E / pz ^3
  double m_1_4 = 1./pz+px*px/pz/pz/pz;
  double m_1_5 = px*py/pz/pz/pz; // px d(1/pz)/dpy = px py / pz^3

  double m_2_3 = -py*energy/pz/pz/pz; // py d(1/pz)/dE = - py E / pz ^3
  double m_2_4 = py*px/pz/pz/pz; // py d(1/pz)/dpx = py px / pz^3
  double m_2_5 = 1./pz+py*py/pz/pz/pz;

  // m_3_x = 0; energy is conserved by magnetic fields

  double m_4_1 = direction*_charge*c_l*(py*dbzdx-pz*dbydx)/pz;
  double m_4_2 = direction*_charge*c_l*(py*dbzdy-pz*dbydy)/pz;
  double m_4_3 = -direction*_charge*c_l*py*bz*energy/pz/pz/pz;
  double m_4_4 = direction*_charge*c_l*py*px*bz/pz/pz/pz;
  double m_4_5 = direction*_charge*c_l*bz/pz+_charge*c_l*py*py*bz/pz/pz/pz;

  double m_5_1 = direction*_charge*c_l*(pz*dbxdx-px*dbzdx)/pz;
  double m_5_2 = direction*_charge*c_l*(pz*dbxdy-px*dbzdy)/pz;
  double m_5_3 = direction*_charge*c_l*px*bz*energy/pz/pz/pz;
  double m_5_4 = -direction*_charge*c_l*bz/pz-_charge*c_l*px*px*bz/pz/pz/pz;
  double m_5_5 = -direction*_charge*c_l*py*px*bz/pz/pz/pz;

  double matrix[36] = {
    0.,    0.,    0., m_0_3, m_0_4, m_0_5,
    0.,    0.,    0., m_1_3, m_1_4, m_1_5,
    0.,    0.,    0., m_2_3, m_2_4, m_2_5,
    0.,    0.,    0.,    0.,    0.,    0.,
    0., m_4_1, m_4_2, m_4_3, m_4_4, m_4_5,
    0., m_5_1, m_5_2, m_5_3, m_5_4, m_5_5
  };
  for (size_t i = 0; i < 6; ++i)
      for (size_t j = 0; j < 6; ++j)
          _matrix[i][j] = matrix[i*6+j];
}

void ErrorTracking::FieldDerivative
                              (const double* point, double* derivative) const {
    double pos[4] = {point[0], point[1], point[2], point[3]};
    double field_x_pos[6] = {0, 0, 0, 0, 0, 0};
    double field_x_neg[6] = {0, 0, 0, 0, 0, 0};
    double field_y_pos[6] = {0, 0, 0, 0, 0, 0};
    double field_y_neg[6] = {0, 0, 0, 0, 0, 0};

    pos[0] += _dx;
    _field->GetFieldValue(pos, field_x_pos);
    pos[0] -= 2*_dx;
    _field->GetFieldValue(pos, field_x_neg);
    pos[0] = point[0];

    pos[1] += _dy;
    _field->GetFieldValue(pos, field_y_pos);
    pos[1] -= 2*_dy;
    _field->GetFieldValue(pos, field_y_neg);

    derivative[0] = (field_x_pos[0] - field_x_neg[0])/2/_dx;
    derivative[1] = (field_x_pos[1] - field_x_neg[1])/2/_dx;
    derivative[2] = (field_x_pos[2] - field_x_neg[2])/2/_dx;
    derivative[3] = (field_y_pos[0] - field_y_neg[0])/2/_dy;
    derivative[4] = (field_y_pos[1] - field_y_neg[1])/2/_dy;
    derivative[5] = (field_y_pos[2] - field_y_neg[2])/2/_dy;
}

std::vector<double> ErrorTracking::GetDeviations() const {
  double dev_a[] = {_dx,_dy,_dz,_dt};
  return std::vector<double>(dev_a, dev_a+sizeof(dev_a)/sizeof(double));
}

void ErrorTracking::SetDeviations
                                (double dx, double dy, double dz, double dt) {
  _dx = dx;
  _dy = dy;
  _dz = dz;
  _dt = dt;
}

std::vector<std::vector<double> > ErrorTracking::GetMatrix() const {
  return _matrix;
}

void ErrorTracking::SetField(BTField* field) {
    if (field == NULL) {
        _field = Globals::GetMCFieldConstructor();
    } else {
        _field = field;
    }
}

void ErrorTracking::SetGeometryModel(std::string geometry_model) {
    if (geometry_model == "geant4") {
        SetGeometryModel(geant4);
    } else if (geometry_model == "axial_lookup") {
        SetGeometryModel(axial_lookup);
    } else {
        throw MAUS::Exception(Exception::recoverable,
                          "Failed to recognise geometry model '"+geometry_model
                          +"' Should be 'geant4' or 'axial_lookup'",
                          "ErrorTracking::SetGeometryModel");
    }
}

ostream& ErrorTracking::print(std::ostream& out, const double* x) {
    out << "t: " << x[0] << " pos: " << x[1] << " " << x[2] << " " << x[3] << "\n"
        << "E: " << x[4] << " mom: " << x[5] << " " << x[6] << " " << x[7] << std::endl; 
    size_t index = 8;
    for (size_t i = 0; i < 6; ++i) {
        for (size_t j = 0; j < i; ++j) {
            out << "              ";
        }
        for (size_t j = i; j < 6; ++j) {
            out << std::setprecision(6) << std::setw(14) << x[index];
            ++index;
        }
        out << std::endl;
    }
    return out;
}

} // namespace Global
} // namespace Kalman
} // namespace MAUS

