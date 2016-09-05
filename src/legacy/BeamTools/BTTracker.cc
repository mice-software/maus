// MAUS WARNING: THIS IS LEGACY CODE.
#include "BTField.hh"
#include "BTTracker.hh"
#include "Utils/Exception.hh"
#include "src/common_cpp/Utils/Globals.hh"

#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Vector/ThreeVector.h"

#include "gsl/gsl_odeiv.h"
#include "gsl/gsl_errno.h"

#include <sstream>
const BTField*     BTTracker::_field   = NULL;
CLHEP::HepRotation BTTracker::_rot     = CLHEP::HepRotation();
CLHEP::HepRotation BTTracker::_rotback = CLHEP::HepRotation();
double             BTTracker::_q       = 0.;
double             BTTracker::_m       = 0.;

const  double BTTracker::_c_l    = 299.792458; //mm*ns^{-1} (By definition) Worry about initialisation order, so don't use CLHEP::c_light
double BTTracker::_absoluteError = 1e-4;
double BTTracker::_relativeError = 1e-4;
int    BTTracker::_maxNSteps     = 10000;


int                 BTTracker::_nevents    = 0;    //number of events in the tracking
double*             BTTracker::_charges    = NULL;    //charge for tracking
double*             BTTracker::_macro_size = NULL; //macroparticle size for space charge


void BTTracker::integrate(double target_indie, double* y, const BTField* field, BTTracker::var indep, double step_size, double charge,
                          CLHEP::Hep3Vector origin, CLHEP::HepRotation rot)
{
  _q       = charge;
  _m       = sqrt(y[4]*y[4] - y[5]*y[5] - y[6]*y[6] - y[7]*y[7]);
  if( std::isnan(_m) || std::isinf(_m) ) {
    if (y[4]*y[4] - y[5]*y[5] - y[6]*y[6] - y[7]*y[7] > -1e-6) {
      _m = 0.;
    } else {
      throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Mass undefined in stepping function", "BTTracker::integrate"));
    }
  }

  if(indep == symplectic_tau1) { BTTracker::symplecticIntegrate(target_indie, y, field, 0., step_size, charge, 1); return; }
  if(indep == symplectic_tau2) { BTTracker::symplecticIntegrate(target_indie, y, field, 0., step_size, charge, 2); return; }

  _field = field;
  const gsl_odeiv_step_type * T = gsl_odeiv_step_rk4;
  gsl_odeiv_step    * step    = gsl_odeiv_step_alloc(T,8);
  gsl_odeiv_control * control = gsl_odeiv_control_y_new(_absoluteError, _relativeError);
  gsl_odeiv_evolve  * evolve  = gsl_odeiv_evolve_alloc(8);
  int (*FuncEqM)(double z, const double y[], double f[], void *params)=NULL;

  _rot     = rot;
  _rotback = rot.inverse();

  //Probably not the most efficient, but only does it once per integrate call
  _absoluteError = (*MAUS::Globals::GetInstance()->GetConfigurationCards())["field_tracker_absolute_error"].asDouble();
  _relativeError = (*MAUS::Globals::GetInstance()->GetConfigurationCards())["field_tracker_relative_error"].asDouble();

  double indie  = 0.;
  switch (indep)
  {
    case tau_field:
    { FuncEqM = &BTTracker::tau_field_equations_motion;     indie = 0.; break; }
    case tau_potential:
    { FuncEqM = &BTTracker::tau_potential_equations_motion; indie = 0.; break; }
    case u:
    {
      CLHEP::Hep3Vector pos(y[1], y[2], y[3]);
      pos     -= origin;
      FuncEqM  = &BTTracker::u_equations_motion; 
      indie    = (_rot*pos)[2];
      break;
    }
    case z:
    { FuncEqM = &BTTracker::z_equations_motion; indie = y[3]; break; }
    case t:
    { FuncEqM = &BTTracker::t_equations_motion; indie = y[0]; break; }
    default:
    { return; }
  }
  gsl_odeiv_system    system  = {FuncEqM, NULL, 8, NULL};

  double h      = step_size;
  int    nsteps = 0;
  while(fabs(indie-target_indie) > 1e-6)
  {
    nsteps++;
    int status =  gsl_odeiv_evolve_apply(evolve, control, step, &system, &indie, target_indie, &h, y);
    if(status != GSL_SUCCESS)
    {
      throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Failed during tracking", "BTTracker::integrate") );
      break;
    }
    if(nsteps > _maxNSteps)
    {
      std::stringstream ios;
      ios << "Killing tracking with step size " << h << " at step " << nsteps << " of " << _maxNSteps << "\n" 
          << "t: " << y[0] << " pos: " << y[1] << " " << y[2] << " " << y[3] << "\n"
          << "E: " << y[4] << " mom: " << y[5] << " " << y[6] << " " << y[7] << std::endl; 
      throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, ios.str()+" Exceeded maximum number of steps\n", "BTTracker::integrate") );
      break;
    }
  }
  gsl_odeiv_evolve_free (evolve);
  gsl_odeiv_control_free(control);
  gsl_odeiv_step_free   (step);
}


int BTTracker::tau_field_equations_motion(double tau, const double x[8], double dxdt[8], void* params)
{
  double xfield[4] = {x[1], x[2], x[3], x[0]};
  double field[6] = {0.,0.,0.,0.,0.,0.};
  _field->GetFieldValue(xfield, field);

  dxdt[0]   = x[4]/_m;
  for(int i=1; i<4; i++) dxdt[i]   = x[i+4]*_c_l/_m;

  dxdt[4] = _q*field[3]*dxdt[1]+_q*field[4]*dxdt[2]+_q*field[5]*dxdt[3];
  dxdt[5] = _q*_c_l*( dxdt[2]*field[2] - dxdt[3]*field[1] ) + _q*field[3]*_c_l;
  dxdt[6] = _q*_c_l*( dxdt[3]*field[0] - dxdt[1]*field[2] ) + _q*field[4]*_c_l;
  dxdt[7] = _q*_c_l*( dxdt[1]*field[1] - dxdt[2]*field[0] ) + _q*field[5]*_c_l;


  return GSL_SUCCESS;
}

int BTTracker::tau_potential_equations_motion(double tau, const double x[8], double dxdt[8], void* params)
{
  double __x   [8] = {x[1], x[2], x[3], x[0], x[5], x[6], x[7], x[4]};
  double __dxdt[8] = {0,0,0,0,0,0,0,0};
  double potDiff[4][4];
  for(int i=0; i<4; i++) 
  {
    for(int j=0; j<4; j++) potDiff[i][j] = 0.;
    _field->GetVectorPotentialDifferential(__x, potDiff[i], i);
  }

  __dxdt[3]   = __x[7]/_m;
  for(int i=0; i<3; i++) __dxdt[i] = __x[i+4]*_c_l/_m;
  for(int i=0; i<4; i++)
    for(int j=0; j<4; j++)
      __dxdt[i+4] += _q/_m*__x[j+4]*(potDiff[j][i] - potDiff[i][j])*_c_l*_c_l;

  dxdt[0] = __dxdt[3]; dxdt[1] = __dxdt[0]; dxdt[2] = __dxdt[1]; dxdt[3] = __dxdt[2];
  dxdt[4] = __dxdt[7]; dxdt[5] = __dxdt[4]; dxdt[6] = __dxdt[5]; dxdt[7] = __dxdt[6];

  return GSL_SUCCESS;
}

int BTTracker::t_equations_motion  (double t,   const double x[8], double dxdt[8], void* params)
{
  double field[6] = {0.,0.,0.,0.,0.,0.};
  double xfield[4] = {x[1], x[2], x[3], x[0]};
  _field->GetFieldValue(xfield, field);
  double q_c    = _q;
  //dx/dt = px/E
  dxdt[1] = _c_l*x[5]/x[4];
  dxdt[2] = _c_l*x[6]/x[4];
  dxdt[3] = _c_l*x[7]/x[4];
  //dt/dt = 1.
  dxdt[0] = 1.;
  // dp/dt = qE + vxB
  dxdt[5] = q_c*_c_l*( dxdt[2]*field[2] - dxdt[3]*field[1] ) + q_c*field[3]*_c_l;
  dxdt[6] = q_c*_c_l*( dxdt[3]*field[0] - dxdt[1]*field[2] ) + q_c*field[4]*_c_l;
  dxdt[7] = q_c*_c_l*( dxdt[1]*field[1] - dxdt[2]*field[0] ) + q_c*field[5]*_c_l;
  // E^2  = px^2+py^2+pz^2+ m^2
  // E dE = px dpx + py dpy + pz dpz
  // dEdt = dxdt dpx + dydt dpy + dzdt dpz (ignore B as B conserves energy)
  dxdt[4] = q_c*field[3]*dxdt[1]+q_c*field[4]*dxdt[2]+q_c*field[5]*dxdt[3];
  return GSL_SUCCESS;
}

int BTTracker::z_equations_motion  (double z, const double x[8], double dxdz[8], void* params)
{
  if(fabs(x[7]) < 1e-9) { return GSL_ERANGE;}
  double field[6] = {0.,0.,0.,0.,0.,0.};
  double xfield[4] = {x[1], x[2], x[3], x[0]};
  _field->GetFieldValue(xfield, field);

  double q_c    = _q;
  double dtdz   = x[4]/x[7]; //NOTE to self:- why not use x[4]/fabs(x[7]) to simplify things?
  double dir    = fabs(x[7])/x[7]; //direction of motion
  //dx/dz = px/pz
  dxdz[0] = dtdz/_c_l;
  dxdz[1] = x[5]/x[7];
  dxdz[2] = x[6]/x[7];
  dxdz[3] = 1.;
  // dp/dz = dp/dt dt/dz = dt/dz qE + dt/dz dx/dt X B
  // WORRY ABOUT FACTOR OF dt/dz (~E/pz)
  dxdz[5] = q_c*_c_l*( dxdz[2]*field[2] - dxdz[3]*field[1] ) + q_c*field[3]*dtdz*dir; //dpx/dz = qc/pz py*bz
  dxdz[6] = q_c*_c_l*( dxdz[3]*field[0] - dxdz[1]*field[2] ) + q_c*field[4]*dtdz*dir;
  dxdz[7] = q_c*_c_l*( dxdz[1]*field[1] - dxdz[2]*field[0] ) + q_c*field[5]*dtdz*dir;
  // E^2  = px^2+py^2+pz^2+ m^2
  // E dE = px dpx + py dpy + pz dpz
  // dEdz = dxdt dpxdz + dydt dpydz + dzdt dpzdz (ignore B as B conserves energy)
  // dEdz = dxdz Ex + dydz Ey + Ez (ignore B as B conserves energy)
  dxdz[4] = (dxdz[1]*q_c*field[3]+dxdz[2]*q_c*field[4]+q_c*field[5])*dir;
  return GSL_SUCCESS;
}

int BTTracker::u_equations_motion  (double z, const double x[8], double dxdz[8], void* params)
{
  double field1[6] = {0.,0.,0.,0.,0.,0.};
  double xfield[4] = {x[1], x[2], x[3], x[0]};
  _field->GetFieldValue(xfield, field1);
  //bfield, efield are fields in rotated frame
  CLHEP::Hep3Vector bfield(field1[0],field1[1],field1[2]);
  CLHEP::Hep3Vector efield(field1[3],field1[4],field1[5]);
  CLHEP::Hep3Vector p_in  (x[5],x[6],x[7]);
  bfield           = _rot*bfield;
  efield           = _rot*efield;
  p_in             = _rot*p_in;
  double field[6]  = {bfield[0],bfield[1],bfield[2],efield[0],efield[1],efield[2]};
  if(fabs(p_in[2]) < 1e-9) return GSL_ERANGE;
  double dir    = 1.;//p_in[2])/p_in[2]; //direction of motion

  double q_c    = _q;
  double dtdz   = x[4]/p_in[2];
  //dx/dz = px/pz
  dxdz[0] = dtdz/_c_l;
  CLHEP::Hep3Vector xP; //dx/dz
  xP[0] = p_in[0]/p_in[2];
  xP[1] = p_in[1]/p_in[2];
  xP[2] = 1.;
  // dp/dz = dp/dt dt/dz = qE + dt/dz dx/dz X B
  // WORRY ABOUT FACTOR OF dt/dz (~E/pz)
  CLHEP::Hep3Vector pP; //dp/dz
  pP[0] = q_c*_c_l*( xP[1]*field[2] - xP[2]*field[1] ) + q_c*field[3]*dtdz*dir; //dpx/dz = qc/pz py*bz
  pP[1] = q_c*_c_l*( xP[2]*field[0] - xP[0]*field[2] ) + q_c*field[4]*dtdz*dir;
  pP[2] = q_c*_c_l*( xP[0]*field[1] - xP[1]*field[0] ) + q_c*field[5]*dtdz*dir;
  // E^2  = px^2+py^2+pz^2+ m^2
  // E dE = px dpx + py dpy + pz dpz
  // dEdz = dxdz Ex + dydz Ey + Ez (ignore B as B conserves energy)
  dxdz[4] = (q_c*xP[0]*field[3]+q_c*xP[1]*field[4]+q_c*field[5])*dir;

  //now rotate back
  xP      = _rotback*xP;
  pP      = _rotback*pP;

  dxdz[1] = xP[0];
  dxdz[2] = xP[1];
  dxdz[3] = xP[2];

  dxdz[5] = pP[0];
  dxdz[6] = pP[1];
  dxdz[7] = pP[2];

  return GSL_SUCCESS;
}

void BTTracker::symplecticIntegrate(double end_tau, double* x_in, const BTField* field, double start_tau, double step_size, double charge, int order)
{
  //convert to canonical momentum, px^c = px^k + qA
  double x[4]         = {x_in[1], x_in[2], x_in[3], x_in[0]};
  double potential[4] = {0,0,0,0};
  field->GetVectorPotential(x, potential);

  double p[4];
  //x_in is a vector of size 8, going like t,x,y,z; E,px,py,pz
  //x is a vector of size 4 going like x,y,z,t
  //p is a vector of size 4 going like px,py,pz,E
  p[0] = x_in[5]-charge*potential[0]*_c_l;
  p[1] = x_in[6]-charge*potential[1]*_c_l;
  p[2] = x_in[7]-charge*potential[2]*_c_l;
  p[3] = x_in[4]-charge*potential[3]*_c_l;

  double tau = 0;
  if(order == 1)
  {
    tau    = start_tau;
    for(int n=0; n<_maxNSteps && tau+step_size<end_tau; n++) 
      symplecticIntegrateStep1(x, p, field, tau, step_size, charge, _m); 
    symplecticIntegrateStep1(x, p, field, tau, end_tau-tau, charge, _m);
  }

  if(order == 2)
  {
    tau    = start_tau;
    for(int n=0; n<_maxNSteps && tau+step_size<end_tau; n++) 
      symplecticIntegrateStep2(x, p, field, tau, step_size, charge, _m); 
    symplecticIntegrateStep2(x, p, field, tau, end_tau-tau, charge, _m);
  }

  if(end_tau-tau > step_size/100.)
    throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Error during tracking", "BTTracker::symplecticIntegrate"));

  //convert to kinetic momentum
  field->GetVectorPotential(x, potential);
  x_in[1] = x[0];
  x_in[2] = x[1];
  x_in[3] = x[2];
  x_in[0] = x[3];
  x_in[5] = p[0]+charge*potential[0]*_c_l;
  x_in[6] = p[1]+charge*potential[1]*_c_l;
  x_in[7] = p[2]+charge*potential[2]*_c_l;
  x_in[4] = p[3]+charge*potential[3]*_c_l;
}

void BTTracker::canonicalToKinetic(double* x_in, double charge, BTField* field)
{
  double potential[4] = {0,0,0,0};
  double x[4]         = {x_in[1], x_in[2], x_in[3], x_in[0]};
  field->GetVectorPotential(x, potential);
  x_in[5] -= charge*potential[0]*_c_l;
  x_in[6] -= charge*potential[1]*_c_l;
  x_in[7] -= charge*potential[2]*_c_l;
  x_in[4] -= charge*potential[3]*_c_l;
}

void BTTracker::kineticToCanonical(double* x_in, double charge, BTField* field)
{
  double potential[4] = {0,0,0,0};
  double x[4]         = {x_in[1], x_in[2], x_in[3], x_in[0]};
  field->GetVectorPotential(x, potential);
  x_in[5] += charge*potential[0]*_c_l;
  x_in[6] += charge*potential[1]*_c_l;
  x_in[7] += charge*potential[2]*_c_l;
  x_in[4] += charge*potential[3]*_c_l;
}


//x is a vector of size 4 going like x,y,z,t
//p is canonical momentum vector of size 4 going like px,py,pz,E
//Symplectic first order step
//Hamiltonian H = sum(p - qA/c)^2 /2/m
void BTTracker::symplecticIntegrateStep1(double* x, double* p, const BTField* field, double& tau, double step_size, double charge, double mass)
{
  double potential     [4] = {0,0,0,0};
  double delta         [4];
  double xin           [4] = {x[0],x[1],x[2],x[3]};
  field->GetVectorPotential(x, potential);
  for(int i=0; i<4; i++) delta[i]  = (p[i] + charge*potential[i]*_c_l)*step_size/mass;
  for(int i=0; i<3; i++) delta[i] *= _c_l;
  for(int i=0; i<4; i++)
  {
    x[i] += delta[i];
    double potentialDiff1[4] = {0,0,0,0};
    field->GetVectorPotentialDifferential(xin, potentialDiff1, i);
    for(int j=0; j<4; j++)
      p[i] -= delta[j]*potentialDiff1[j]*charge*_c_l; //dp_i/dtau = dH/dx_i = sum_j (p_j-qA_j/c).q (dA_j/dx_i)/c/m
  }
  tau += step_size;
}

//second order integrator
void BTTracker::symplecticIntegrateStep2(double* x, double* p, const BTField* field, double& tau, double step_size, double charge, double mass)
{
  double potential1    [4] = {0,0,0,0};
  double potential2    [4] = {0,0,0,0};
  double potentialDiff1[4] = {0,0,0,0};
  double potentialDiff2[4] = {0,0,0,0};
  for(int i=0; i<4; i++)
  {
    field->GetVectorPotential            (x, potential1);
    field->GetVectorPotentialDifferential(x, potentialDiff1, i);
    double delta_i = (p[i] + charge*potential1[i]*_c_l)*step_size/mass;
    if(i != 3) delta_i *= _c_l;
    x[i]     += delta_i;
    field->GetVectorPotential            (x, potential2);
    field->GetVectorPotentialDifferential(x, potentialDiff2, i);
    for(int j=0; j<4; j++) p[j] += (potentialDiff2[j]-potentialDiff1[j])*charge*_c_l*delta_i;
  }

  tau += step_size;
}



void BTTracker::integrateMany(double target_indie, int n_events, double* y, double* macro_size, double* charge, const BTField* field, BTTracker::var indep, 
                double step_size)
{
  _field = field;
  const gsl_odeiv_step_type * T = gsl_odeiv_step_rk4;
  gsl_odeiv_step    * step    = gsl_odeiv_step_alloc(T,8*n_events);
  gsl_odeiv_control * control = gsl_odeiv_control_y_new(_absoluteError, _relativeError);
  gsl_odeiv_evolve  * evolve  = gsl_odeiv_evolve_alloc(8*n_events);
  int (*FuncEqM)(double z, const double y[], double f[], void *params)=NULL;

  //Probably not the most efficient, but only does it once per integrate call
  _absoluteError = (*MAUS::Globals::GetInstance()->GetConfigurationCards())["field_tracker_absolute_error"].asDouble();
  _relativeError = (*MAUS::Globals::GetInstance()->GetConfigurationCards())["field_tracker_relative_error"].asDouble();
  _maxNSteps     = (*MAUS::Globals::GetInstance()->GetConfigurationCards())["maximum_number_of_steps"].asInt();

  _nevents    = n_events;
  _charges    = charge;
  _macro_size = macro_size;


  double indie  = 0.;
  switch (indep)
  {
    case t:
    { FuncEqM = &BTTracker::t_equations_motion_many; indie = y[0]; break; }
    default:
    { throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Only integration through time implemented in integrate many", "BTTracker::integrateMany") ); }
  }
  gsl_odeiv_system    system  = {FuncEqM, NULL, 8*n_events, NULL};

  double h      = step_size;
  int    nsteps = 0;
  while(fabs(indie-target_indie) > 1e-6)
  {
    nsteps++;
    int status =  gsl_odeiv_evolve_apply(evolve, control, step, &system, &indie, target_indie, &h, y);
    if(status != GSL_SUCCESS)
    {
      throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Failed during tracking", "BTTracker::integrateMany") );
      break;
    }
    if(nsteps > _maxNSteps)
    {
      std::stringstream ios;
      ios << "Killing tracking with step size " << h << " at step " << nsteps << "Event failed:\n" 
          << "t: " << y[0] << " pos: " << y[1] << " " << y[2] << " " << y[3] << "\n"
          << "E: " << y[4] << " mom: " << y[5] << " " << y[6] << " " << y[7] << std::endl; 
      throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, ios.str()+"Exceeded maximum number of steps\n", "BTTracker::integrateMany") );
      break;
    }
  }
  gsl_odeiv_evolve_free (evolve);
  gsl_odeiv_control_free(control);
  gsl_odeiv_step_free   (step);
}

int BTTracker::t_equations_motion_many       (double t,   const double x[], double dxdt[], void* params)
{
  for(int i=0; i<_nevents; i++)
  {
    const double* x0 = &x   [i*8];
    double* dx0dt    = &dxdt[i*8];
    _q = _charges[i];
    _m = sqrt(x0[4]*x0[4] - x0[5]*x0[5] - x0[6]*x0[6] - x0[7]*x0[7]);
    int success = t_equations_motion(t, x0, dx0dt, params);
    if(success != GSL_SUCCESS) return success;
  }
  return GSL_SUCCESS;
}

