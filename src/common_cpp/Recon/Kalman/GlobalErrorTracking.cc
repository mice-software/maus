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
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Recon/Kalman/GlobalErrorTracking.hh"

namespace MAUS {

GlobalErrorTracking* GlobalErrorTracking::_tz_for_propagate = NULL;

GlobalErrorTracking::GlobalErrorTracking() : _field(Globals::GetMCFieldConstructor()),
               _dx(1.), _dy(1.), _dz(1.), _dt(1.), _charge(1.),
               _matrix(6, std::vector<double>(6, 0.)) {
}

void GlobalErrorTracking::Propagate(double x[29], double target_z) {
  std::cerr << "GlobalErrorTracking::Propagate material model " << _eloss_model << std::endl;

  double mass_squared = x[4]*x[4] - x[5]*x[5] - x[6]*x[6] - x[7]*x[7];
  if (mass_squared < -1e-6 || mass_squared != mass_squared) {
    throw(MAUS::Exception(MAUS::Exception::recoverable,
          "Mass undefined in stepping function",
          "GlobalErrorTracking::Propagate"));
  }
  const gsl_odeiv_step_type * T = gsl_odeiv_step_rk4;
  gsl_odeiv_step    * step    = gsl_odeiv_step_alloc(T, 29);
  gsl_odeiv_control * control = gsl_odeiv_control_y_new(_absolute_error, _relative_error);
  gsl_odeiv_evolve  * evolve  = gsl_odeiv_evolve_alloc(29);
  gsl_odeiv_system    system  = {GlobalErrorTracking::EquationsOfMotion, NULL, 29, NULL};

  double z = x[3];
  double h = fabs(_step_size);  // ignore _step_size sign, we "auto detect"
  if (z > target_z)
      h *= -1;  // stepping backwards...
  int    nsteps = 0;
  _tz_for_propagate = this;
  //std::cerr << "GlobalErrorTracking::Propagate BEFORE" << std::endl;
  //print(std::cerr, x);
  while(fabs(z-target_z) > 1e-6) {
    // std::cerr << "Stepping " << nsteps << " dz: " << h << " z_tot: " << z << std::endl;
    nsteps++;
    
    int status =  gsl_odeiv_evolve_apply(evolve, control, step, &system, &z, target_z, &h, x);
    // std::cerr << "Stepped " << nsteps << " dz: " << h << " z_tot: " << z << std::endl;
    
    if(status != GSL_SUCCESS)
    {
        throw(MAUS::Exception(MAUS::Exception::recoverable,
                            "Failed during tracking",
                            "GlobalErrorTracking::Propagate") );
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
                              "GlobalErrorTracking::Propagate") );
        break;
    }
  }
  //std::cerr << "GlobalErrorTracking::Propagate AFTER" << std::endl;
  //print(std::cerr, x);
  gsl_odeiv_evolve_free (evolve);
  gsl_odeiv_control_free(control);
  gsl_odeiv_step_free   (step);
}

void GlobalErrorTracking::PropagateTransferMatrix(double x[44], double target_z) {
  double mass_squared = x[4]*x[4] - x[5]*x[5] - x[6]*x[6] - x[7]*x[7];
  if (mass_squared < -1e-6 || mass_squared != mass_squared) {
    throw(MAUS::Exception(MAUS::Exception::recoverable,
          "Mass undefined in stepping function",
          "GlobalErrorTracking::Propagate"));
  }
  const gsl_odeiv_step_type * T = gsl_odeiv_step_rk4;
  gsl_odeiv_step    * step    = gsl_odeiv_step_alloc(T, 44);
  gsl_odeiv_control * control = gsl_odeiv_control_y_new(_absolute_error, _relative_error);
  gsl_odeiv_evolve  * evolve  = gsl_odeiv_evolve_alloc(44);
  gsl_odeiv_system    system  = {GlobalErrorTracking::EquationsOfMotion, NULL, 44, NULL};

  double z = x[3];
  double h = fabs(_step_size);  // ignore _step_size sign, we "auto detect"
  if (z > target_z)
      h *= -1;  // stepping backwards...
  int    nsteps = 0;
  _tz_for_propagate = this;
  while(fabs(z-target_z) > 1e-6) {
    nsteps++;
    
    int status =  gsl_odeiv_evolve_apply(evolve, control, step, &system, &z, target_z, &h, x);
    
    if(status != GSL_SUCCESS)
    {
        throw(MAUS::Exception(MAUS::Exception::recoverable,
                            "Failed during tracking",
                            "GlobalErrorTracking::Propagate") );
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
                              "GlobalErrorTracking::Propagate") );
        break;
    }
  }

  gsl_odeiv_evolve_free (evolve);
  gsl_odeiv_control_free(control);
  gsl_odeiv_step_free   (step);


}

int GlobalErrorTracking::EquationsOfMotion(double z, const double x[29], double dxdz[29],
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

  // If no material processes are active, don't run MaterialEquationsOfMotion
  // at all
  std::cerr << "GlobalErrorTracking::EquationsOFMotion checking material model" << std::endl;
  if (_tz_for_propagate->_eloss_model == no_eloss &&
      _tz_for_propagate->_mcs_model == no_mcs &&
      _tz_for_propagate->_estrag_model == no_estrag) {
      std::cerr << "GlobalErrorTracking::EquationsOFMotion material models disabled" << std::endl;
      return GSL_SUCCESS;
  }
  std::cerr << "GlobalErrorTracking::EquationsOFMotion material models enabled; proceeding with EqM" << std::endl;

  int material_success = MaterialEquationOfMotion(z, x, dxdz, params);
  if (material_success != GSL_SUCCESS) {
      return material_success;
  }
  return GSL_SUCCESS;
}

int GlobalErrorTracking::EMEquationOfMotion(double z, const double x[29], double dxdz[29],
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


int GlobalErrorTracking::MatrixEquationOfMotion(double z, const double x[44], double dxdz[44],
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

  _tz_for_propagate->UpdateTransferMatrix(x);
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


int GlobalErrorTracking::MaterialEquationOfMotion(double z, const double x[29], double dxdz[29],
                                   void* params) {
    // Must be called after EMEquationOfMotion
    std::cerr << "GlobalErrorTracking::MaterialEquationsOfMotion" << std::endl;
    MaterialModel& material = _tz_for_propagate->_mat_mod;
    G4ThreeVector pos(x[1], x[2], x[3]);
    double mom_tot = sqrt(x[5]*x[5]+x[6]*x[6]+x[7]*x[7]);
    G4ThreeVector mom(x[5]/mom_tot, x[6]/mom_tot, x[7]/mom_tot);

    GeometryNavigator* navigator = Globals::GetMCGeometryNavigator();
    navigator->SetPoint(ThreeVector(x[1], x[2], x[3]));
    /*
    if (navigator == NULL) {
        std::cerr << "Navigator was NULL" << std::endl;
        return GSL_FAILURE;
    }
    G4VPhysicalVolume* phys_vol = navigator->LocateGlobalPointAndSetup(pos, &mom, false);
    if (phys_vol == NULL) {
        std::cerr << "Phys_vol was NULL" << std::endl;
        return GSL_FAILURE;
    }
    G4LogicalVolume* log_vol = phys_vol->GetLogicalVolume();
    if (log_vol == NULL) {
        std::cerr << "Log_vol was NULL" << std::endl;
        return GSL_FAILURE;
    }
    */
    // material.SetMaterial(log_vol->GetMaterial());
    std::cerr << "GlobalErrorTracking::MaterialEquationsOfMotion phys_vol " << navigator->GetMaterialName()
              << " x: " << x[1] << " y: " << x[2] << " z: " << x[3]
              << " E: " << x[4] << " pz: " << x[7] << std::endl;
    return GSL_SUCCESS;
    double energy = sqrt(x[4]*x[4]);
    double p = sqrt(x[5]*x[5]+x[6]*x[6]+x[7]*x[7]);
    double mass = sqrt(energy*energy-p*p);
    double charge = _tz_for_propagate->_charge;
    double dEdz;
    double d2EdzdE;
    double dtheta2dz;
    double destragdz;
    if (_tz_for_propagate->_eloss_model == bethe_bloch_forwards) {
        dEdz = material.dEdx(energy, mass, charge); // should be negative
        d2EdzdE =  material.d2EdxdE(energy, mass, charge);
    } else if (_tz_for_propagate->_eloss_model == bethe_bloch_backwards) {
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

void GlobalErrorTracking::UpdateTransferMatrix(const double* x) {
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

  double m_4_1 = _charge*c_l*(py*dbzdx-pz*dbydx)/pz;
  double m_4_2 = _charge*c_l*(py*dbzdy-pz*dbydy)/pz;
  double m_4_3 = -_charge*c_l*py*bz*energy/pz/pz/pz;
  double m_4_4 = _charge*c_l*py*px*bz/pz/pz/pz;
  double m_4_5 = _charge*c_l*bz/pz+_charge*c_l*py*py*bz/pz/pz/pz;

  double m_5_1 = _charge*c_l*(pz*dbxdx-px*dbzdx)/pz;
  double m_5_2 = _charge*c_l*(pz*dbxdy-px*dbzdy)/pz;
  double m_5_3 = _charge*c_l*px*bz*energy/pz/pz/pz;
  double m_5_4 = -_charge*c_l*bz/pz-_charge*c_l*px*px*bz/pz/pz/pz;
  double m_5_5 = -_charge*c_l*py*px*bz/pz/pz/pz;

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

void GlobalErrorTracking::FieldDerivative(const double* point, double* derivative) const {
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

std::vector<double> GlobalErrorTracking::GetDeviations() const {
  double dev_a[] = {_dx,_dy,_dz,_dt};
  return std::vector<double>(dev_a, dev_a+sizeof(dev_a)/sizeof(double));
}

void GlobalErrorTracking::SetDeviations(double dx, double dy, double dz, double dt) {
  _dx = dx;
  _dy = dy;
  _dz = dz;
  _dt = dt;
}

std::vector<std::vector<double> > GlobalErrorTracking::GetMatrix() const {
  return _matrix;
}

void GlobalErrorTracking::SetField(BTField* field) {
    if (field == NULL) {
        _field = Globals::GetMCFieldConstructor();
    } else {
        _field = field;
    }
}

ostream& GlobalErrorTracking::print(std::ostream& out, double* x) {
    out << "t: " << x[0] << " pos: " << x[1] << " " << x[2] << " " << x[3] << "\n"
        << "E: " << x[4] << " mom: " << x[5] << " " << x[6] << " " << x[7] << std::endl; 
    size_t index = 8;
    for (size_t i = 0; i < 6; ++i) {
        for (size_t j = 0; j < i; ++j) {
            out << "       ";
        }
        for (size_t j = i; j < 6; ++j) {
            out << std::setprecision(6) << std::setw(10) << x[i];
            ++index;
        }
        out << std::endl;
    }
    return out;
}

}

