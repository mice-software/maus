#include <math.h>

#include <sstream>

#include "gsl/gsl_odeiv.h"
#include "gsl/gsl_errno.h"

#include "Geant4/G4Navigator.hh"
#include "Geant4/G4TransportationManager.hh"
#include "Geant4/G4VPhysicalVolume.hh"
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4Material.hh"

#include "src/legacy/Interface/Squeak.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Recon/Global/Tracking.hh"

namespace MAUS {

TrackingZ* TrackingZ::_tz_for_propagate = NULL;

TrackingZ::TrackingZ() : _field(Globals::GetMCFieldConstructor()),
               _dx(1.), _dy(1.), _dz(1.), _dt(1.), _charge(1.),
               _matrix(6, std::vector<double>(6, 0.)) {
}

void TrackingZ::Propagate(double* x, double target_z) {
  double mass_squared = x[4]*x[4] - x[5]*x[5] - x[6]*x[6] - x[7]*x[7];
  if (mass_squared < -1e-6) {
    throw(MAUS::Exception(MAUS::Exception::recoverable,
          "Mass undefined in stepping function",
          "TrackingZ::Propagate"));
  }
  const gsl_odeiv_step_type * T = gsl_odeiv_step_rk4;
  gsl_odeiv_step    * step    = gsl_odeiv_step_alloc(T, 29);
  gsl_odeiv_control * control = gsl_odeiv_control_y_new(_absolute_error, _relative_error);
  gsl_odeiv_evolve  * evolve  = gsl_odeiv_evolve_alloc(29);
  gsl_odeiv_system    system  = {TrackingZ::EquationsOfMotion, NULL, 29, NULL};

  double z = x[3];
  double h = _step_size;
  int    nsteps = 0;
  _tz_for_propagate = this;
  /*
  std::cerr << "BEFORE" << std::endl;
  for (size_t i = 0; i < 8; ++i)
      std::cerr << x[i] << " ";
  std::cerr << std::endl;
  for (size_t i = 8; i < 29; ++i)
      std::cerr << x[i] << " ";
  std::cerr << std::endl;
  */
  while(fabs(z-target_z) > 1e-6) {
    // std::cerr << "Stepping " << nsteps << " dz: " << h << " z_tot: " << z << std::endl;
    nsteps++;
    
    int status =  gsl_odeiv_evolve_apply(evolve, control, step, &system, &z, target_z, &h, x);
    // std::cerr << "Stepped " << nsteps << " dz: " << h << " z_tot: " << z << std::endl;
    
    if(status != GSL_SUCCESS)
    {
        throw(MAUS::Exception(MAUS::Exception::recoverable,
                            "Failed during tracking",
                            "TrackingZ::Propagate") );
        break;
    }
    if(nsteps > _max_n_steps)
    {
        std::stringstream ios;
        ios << "Killing tracking with step size " << h << " at step " << nsteps << " of " << _max_n_steps << "\n" 
            << "t: " << x[0] << " pos: " << x[1] << " " << x[2] << " " << x[3] << "\n"
            << "E: " << x[4] << " mom: " << x[5] << " " << x[6] << " " << x[7] << std::endl; 
        throw(MAUS::Exception(MAUS::Exception::recoverable,
                              ios.str()+" Exceeded maximum number of steps\n",
                              "TrackingZ::Propagate") );
        break;
    }
  }
  /*
  std::cerr << "AFTER" << std::endl;
  for (size_t i = 0; i < 8; ++i)
      std::cerr << x[i] << " ";
  std::cerr << std::endl;
  for (size_t i = 8; i < 29; ++i)
      std::cerr << x[i] << " ";
  std::cerr << std::endl;
  */
  gsl_odeiv_evolve_free (evolve);
  gsl_odeiv_control_free(control);
  gsl_odeiv_step_free   (step);


}

int TrackingZ::EquationsOfMotion(double z, const double x[29], double dxdz[29],
                                   void* params) {
  if (fabs(x[7]) < 1e-9) {
    // z-momentum is 0
      return GSL_FAILURE;
  }
  if (_tz_for_propagate == NULL) {
      return GSL_FAILURE;
  }
  int em_success = EMEquationOfMotion(z, x, dxdz, params);
  if (em_success != GSL_SUCCESS) {
      return em_success;
  }
  // Commented as it was giving segv...
  int material_success = GSL_SUCCESS; //MaterialEquationOfMotion(z, x, dxdz, params);
  if (material_success != GSL_SUCCESS) {
      return material_success;
  }
  return GSL_SUCCESS;
}

int TrackingZ::EMEquationOfMotion(double z, const double x[29], double dxdz[29],
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

  size_t x_index = 8;
  _tz_for_propagate->UpdateTransferMatrix(x);
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

int TrackingZ::MaterialEquationOfMotion(double z, const double x[29], double dxdz[29],
                                   void* params) {
    // Must be called after EMEquationOfMotion
    MaterialModel& material = _tz_for_propagate->_mat_mod;
    G4ThreeVector pos(x[1], x[2], x[3]);
    G4ThreeVector mom(x[5], x[6], x[7]);
    G4Navigator* navigator = G4TransportationManager::
                          GetTransportationManager()->GetNavigatorForTracking();
    G4VPhysicalVolume* phys_vol = navigator->LocateGlobalPointAndSetup(pos, &mom);
    G4LogicalVolume* log_vol = phys_vol->GetLogicalVolume();
    material.SetMaterial(log_vol->GetMaterial());
    // std::cerr << "TrackingZ::MaterialEquationsOfMotion phys_vol " << phys_vol->GetName() << " " << log_vol->GetMaterial()->GetName()
    //          << " z: " << x[3] << " E: " << x[4] << " pz: " << x[7] << std::endl;
    double energy = sqrt(x[4]*x[4]);
    double p = sqrt(x[5]*x[5]+x[6]*x[6]+x[7]*x[7]);
    double mass = sqrt(energy*energy-p*p);
    double charge = _tz_for_propagate->_charge;
    double dEdz;
    double d2EdzdE;
    double dtheta2dz;
    double destragdz;
    if (_tz_for_propagate->_eloss_model == bethebloch_forwards) {
        dEdz = material.dEdx(energy, mass, charge); // should be negative
        d2EdzdE =  material.d2EdxdE(energy, mass, charge);
    } else if (_tz_for_propagate->_eloss_model == bethebloch_backwards) {
        dEdz = -material.dEdx(energy, mass, charge); // should be positive
        d2EdzdE = -material.d2EdxdE(energy, mass, charge);
    } else {
        dEdz = 0.; // no eloss
        d2EdzdE = 0;
    }

    if (_tz_for_propagate->_estrag_model == estrag_forwards) {
        destragdz = material.estrag2(energy, mass, charge);
    } else if (_tz_for_propagate->_estrag_model == estrag_backwards) {
        destragdz = material.estrag2(energy, mass, charge);
    } else {
        destragdz = 0.;
    } 

    if (_tz_for_propagate->_mcs_model == moliere_forwards) {
        dtheta2dz = material.dtheta2dx(energy, mass, charge);
    } else if (_tz_for_propagate->_mcs_model == moliere_backwards) {
        dtheta2dz = -material.dtheta2dx(energy, mass, charge);
    } else {
        dtheta2dz = 0.;
    }

    // E^2 = p^2+m^2; 2E dE/dz = 2p dp/dz
    double dpdz = energy/p*dEdz;
    dxdz[4] = dEdz;
    dxdz[5] = dpdz*x[5]/p;
    dxdz[6] = dpdz*x[6]/p;
    dxdz[7] = dpdz*x[7]/p;

    // d/dz <E^2> = d<estrag2>/dz + 2E d/dE dE/dz
    dxdz[23] = destragdz+2*x[4]*d2EdzdE;

    // product rule - <p_x^2> = <x'^2> p_z^2
    // d<px^2>/dz = d<x'^2>/dz p_z^2 + 2 <x'^2> p_z dp_z/dz 
    //            = d<x'^2>/dz p_z^2 + 2 <p_x^2>/p_z dp_z/dz
    dxdz[26] = 2*dpdz/x[7]*x[26] + dtheta2dz*x[7]*x[7];
    dxdz[28] = 2*dpdz/x[7]*x[28] + dtheta2dz*x[7]*x[7];

    return GSL_SUCCESS;
}

void TrackingZ::UpdateTransferMatrix(const double* x) {
  // dbx/dx, dby/dx, dbz/dx, dbx/dy, dby/dy, dbz/dy
  double xfield[4] = {x[1], x[2], x[3], x[0]};
  double field_value[] = {0., 0., 0., 0., 0., 0.};
  _field->GetFieldValue(xfield, field_value);
  double dbdx[6] = {0., 0., 0., 0., 0., 0.};
  FieldDerivative(xfield, dbdx);

  double px = x[5];
  double py = x[6];
  double pz = x[7];
  double mass = sqrt(x[4]*x[4]-x[5]*x[5]-x[6]*x[6]-x[7]*x[7]);
  double betagamma = sqrt(x[5]*x[5]+x[6]*x[6]+x[7]*x[7])/mass;

  double bz = field_value[2];

  double dbxdx = dbdx[0];
  double dbydx = dbdx[1];
  double dbzdx = dbdx[2];
  double dbxdy = dbdx[3];
  double dbydy = dbdx[4];
  double dbzdy = dbdx[5];

  double matrix[36] = {
    0., 0., 0., -1./betagamma/betagamma/c_l/pz, 0.,    0.,
    0., 0., 0., 0.,        1./pz, 0.,
    0., 0., 0., 0.,        0.,    1/pz,
    0., 0., 0., 0.,  0., 0.,
    0., _charge*c_l*(py*dbzdx-pz*dbydx)/pz, _charge*c_l*(py*dbzdy-pz*dbydy)/pz, 0., 0., _charge*c_l*bz/pz,
    0., _charge*c_l*(pz*dbxdx-px*dbzdx)/pz, _charge*c_l*(pz*dbxdy-px*dbzdy)/pz, 0., -_charge*c_l*bz/pz, 0.
  };
  for (size_t i = 0; i < 6; ++i)
      for (size_t j = 0; j < 6; ++j)
          _matrix[i][j] = matrix[i*6+j];
}

void TrackingZ::FieldDerivative(const double* point, double* derivative) const {
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

std::vector<double> TrackingZ::GetDeviations() const {
  double dev_a[] = {_dx,_dy,_dz,_dt};
  return std::vector<double>(dev_a, dev_a+sizeof(dev_a)/sizeof(double));
}

void TrackingZ::SetDeviations(double dx, double dy, double dz, double dt) {
  _dx = dx;
  _dy = dy;
  _dz = dz;
  _dt = dt;
}

std::vector<std::vector<double> > TrackingZ::GetMatrix() const {
  return _matrix;
}

void TrackingZ::SetField(BTField* field) {
    if (field == NULL) {
        _field = Globals::GetMCFieldConstructor();
    } else {
        _field = field;
    }
}

}

