// MAUS WARNING: THIS IS LEGACY CODE.
#ifndef BTTRACKER_HH
#define BTTRACKER_HH

#include "CLHEP/Vector/Rotation.h"

class BTField;

//Class to integrate equations of motion through BTFields (using gsl numerical RK4 or other)
//Don't try to instantiate this class! Just call integrate or whatever...
//I want a namespace but with some private variables to keep encapsulation in integration parameters, so I use a class wrapper
//Choice of integration variables - z (along z-axis for linacs), u (some arbitrary coordinate system), t (time), tau (proper time)
//Would like to implement s (distance) at some point - but it's probably difficult
//Starting to work out multiparticle integration for space charge routines
class BTTracker
{
public:
  //distance along z, path length, distance along a vector u, time, proper time (using GetFieldValue), proper time (GetVectorPotential), 
  //proper time with 1st and 2nd order symplectic integrators 
  //(nb: symplectic integrators need fields to have GetVectorPotential and GetVectorPotentialDifferential defined)
  enum var{z,u,t, tau_field, tau_potential, symplectic_tau1, symplectic_tau2}; 
  enum sc {truncated_coulomb};

  //integrate equations of motion from x_in, through field BTField* using indie as the variable of integration
  //rot is a rotation, use when indie = u; indie_in is the start value of independent variable, target_indie is the end value
  //x_in is a vector of size 8, going like t,x,y,z; E,px,py,pz
  //step_size is initial step size
  static void integrate(double target_indie, double* x_in, const BTField* field, BTTracker::var indep, double step_size, double charge,
                        CLHEP::Hep3Vector origin=CLHEP::Hep3Vector(), CLHEP::HepRotation rot=CLHEP::HepRotation());
  //integrate equations of motion from x_in for n_events particles, through field BTField* using indie as the variable of integration
  //rot is a rotation, use when indie = u; indie_in is the start value of independent variable, target_indie is the end value
  //x_in is an array of size n*8, going like (t,x,y,z; E,px,py,pz)_1,(t,x,y,z; E,px,py,pz)_2...(t,x,y,z; E,px,py,pz)_N
  //macro_size is an array of size n storing the number of particles per macro particle for space charge calculation
  //charge is an array of size n storing the particle charges for tracking
  //n_events is the number of hits N in x_in
  //step_size is initial step size
  //used to have also a pointer for space charge fields, although this was removed
  static void integrateMany(double target_indie, int n_events, double* x_in, double* macro_size, double* charge, const BTField* field, BTTracker::var indep, double step_sizeL);

  //integrate using Hamiltonian H=(p-qA/c)^2/m from x_in, through field BTField* using proper time tau as the variable of integration
  //x_in is a vector of size 8, going like t,x,y,z; E^c,px^c,py^c,pz^c *** momenta are canonical momenta ***
  //step_size is absolute step size (not adaptive)
  //BTField should have GetVectorPotential and GetVectorPotentialDifferential defined
  static void symplecticIntegrate(double end_tau, double* x_in, const BTField* field, double start_tau, double step_size, double charge, int order);

  //Add or subtract vector potential from momenta to convert from kinetic to canonical coordinates and vice versa
  //x is an array of size 8, going like (t,x,y,z; E,px,py,pz)
  static void canonicalToKinetic(double* x_in, double charge, BTField* field);
  static void kineticToCanonical(double* x_in, double charge, BTField* field);

  //Accessors etc
  static double absoluteError()           {return _absoluteError;}
  static double relativeError()           {return _relativeError;}
  static int    maximumStepNumber()       {return _maxNSteps;}
  static double absoluteError(double err) {_absoluteError = err; return _absoluteError;}
  static double relativeError(double err) {_relativeError = err; return _relativeError;}
  static int    maximumStepNumber(int n)  {_maxNSteps     = n;   return _maxNSteps;}

private:
  BTTracker () {;}
  ~BTTracker() {;}



  static const BTField*     _field;
  static CLHEP::HepRotation _rot;
  static CLHEP::HepRotation _rotback;

  static const  double _c_l;
  static double _absoluteError;
  static double _relativeError;
  static int    _maxNSteps;

  static double _q;
  static double _m;

  //single particle equations of motion
  static int  tau_field_equations_motion    (double t,   const double x[8], double dxdt[8], void* params);
  static int  tau_potential_equations_motion(double t,   const double x[8], double dxdt[8], void* params);
  static int  t_equations_motion            (double t,   const double x[8], double dxdt[8], void* params);
  static int  z_equations_motion            (double z,   const double x[8], double dxdt[8], void* params); //optimisation - special case of u_eqns_motion where u = 0,0,1
  static int  u_equations_motion            (double u,   const double x[8], double dxdt[8], void* params);

  //data specific to multiparticle transport
  static int                 _nevents;    //number of events in the tracking
  static double*             _charges;    //charge for tracking
  static double*             _macro_size; //macroparticle size for space charge
  //multiparticle equations of motion
  static int  t_equations_motion_many       (double t,   const double x[],  double dxdt[], void* params);

  //First order non-symplectic integration
  //x is a vector of size 4 going like x,y,z,t
  //p is kinetic momentum vector of size 4 going like px,py,pz,E
  //Uses vector potential diff
  static void tauIntegrateStep1(double* x, double* p, const BTField* field, double& tau, double step_size, double charge, double mass);
  //First order integration
  //x is a vector of size 4 going like x,y,z,t
  //p is kinetic momentum vector of size 4 going like px,py,pz,E
  //Uses vector potential diff and vector potential
  static void symplecticIntegrateStep1(double* x, double* p, const BTField* field, double& tau, double step_size, double charge, double mass);
  //Second order integration
  static void symplecticIntegrateStep2(double* x, double* p, const BTField* field, double& tau, double step_size, double charge, double mass);

//  static int s_equations_motion  (double s,   const double x[8], double dxdt[8], void* params); NOT IMPLEMENTED
};

#endif
