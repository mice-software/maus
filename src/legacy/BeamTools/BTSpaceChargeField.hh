// MAUS WARNING: THIS IS LEGACY CODE.
//Contains algorithms to solve for the self-induced space charge for some set of particles
//
//Space charge solvers inherit from BTSpaceChargeField, either sum over all particles (assume point particles) or
//integrate over particles (assume blurred particles) to get the space charge field from each one
//Call UpdateU(...) or UpdateX(...) each time you want to recalculate the field with a new set of particle positions


#ifndef BTSPACECHARGEFIELD_HH
#define BTSPACECHARGEFIELD_HH

#include "BTField.hh"

class ThreeDGrid;
class Interpolator3dGridTo1d;
class Interpolator3dGridTo3d;


//Should be a singleton - inherited singleton? Sounds interesting...
class BTSpaceChargeField : public BTField
{
public:
  BTSpaceChargeField();
  virtual ~BTSpaceChargeField();

  //Return the space-charge induced field from the particles
  virtual void     GetFieldValue( const double Point[4], double *EMfield ) const = 0;
  //Update the space charge field - 
  //u_vector is a vector of particles like  (t,x,y,z; E,px,py,pz)*n_events (so length is 8*n_events), 
  //q_vector is a vector of charges with length n_events
  //macro_vector is a vector of weight per macro particle with length n_events
  virtual void     CalculateField(int n_events, const double * u_vector, const double * q_vector, const double* macro_vector) = 0;

  virtual void     Print(std::ostream &out) const {}
  virtual BTField* Clone() const = 0;


protected:
  //scale factor - 1/(4*pi*e0)
  static double e_const;
  //Calculate the coulomb field at Point from a particle with 8-vector u, mass m and charge q
  //If r<sqrt(rmin2), r=sqrt(rmin2); 
  //Put the field in EMfield 6-vector (bx, by, bz, ex, ey, ez)
  static void   TruncatedCoulombField    (const double* Point, const double* u, const double& q, const double& m, const double& rmin2, double* EMfield);
  //Put the potential in potential 4-vector (phi, ax, ay, az)
  static void   TruncatedCoulombPotential(const double* Point, const double* u, const double& q, const double& m, const double& rmin2, double* potential);
  
private:


};


//BTTruncatedCoulombPotential uses a field like
//E = Sum q_i/ (4 pi e0 r_i^2) for r_i>r0
//E = Sum q_i r_i^2/ (4 pi e0 r0^4)  for r_i<r0
//where r0 is a user-defined cut-off
//All events must have same t (i.e. I assume t is independent variable of integration)
//APPROXIMATION - acceleration between time steps dv/dt is small
class BTTruncatedCoulomb_SC : public BTSpaceChargeField
{
public:
  //r_min is the cut-off for coulomb force calculation, macroParticleSize is the number of particles per macro particle
  BTTruncatedCoulomb_SC (double r_min) : rMin2(r_min*r_min) {}
  ~BTTruncatedCoulomb_SC() {}


  void     Print(std::ostream &out) const {out << "Truncated Coulomb space charge field truncated at r<" << sqrt(rMin2) << std::endl;}
  BTField* Clone() const {return new BTTruncatedCoulomb_SC(*this);}

  //Iterate over all events and add up the coulomb force
  void GetFieldValue( const double Point[4], double *EMfield ) const;
  void GetVectorPotential( const double Point[4], double *potential ) const;
  //ALL u must be at the same t
  void CalculateField(int n_events, const double * u_vector, const double * q_vector, const double* macro_vector);

private:
  //vector of (t,x,y,z,E,px,py,pz)*N
  std::vector<double> u_vec;
  //vector of q*N (q is charge*macro particle size)
  std::vector<double> q_vec;
  //vector of m*N (m is mass)
  std::vector<double> m_vec;
  //square of min r value for cut-off
  double rMin2;
  static const double r2Tol;
};


//BTPIC_SC (Particle-In-Cell space charge)
//(i) particles are binned
//(ii) the 4-vector potential or field at each bin is calculated wrt distance from each bin point
//(iii) the 4-vector potential or field at each point is calculated by interpolation off the grid
//APPROXIMATION - acceleration between time steps dv/dt is small
class BTPIC_SC : public BTSpaceChargeField
{
public:
  //set willCalculateFields to true to calc fields
  //set willCalculatePotential to true to calc potential
  //size* is number of grid nodes in each dimension
  BTPIC_SC (bool willCalculateFields, bool willCalculatePotential, int sizeX, int sizeY, int sizeZ, double rmin); 
  ~BTPIC_SC();

  void     Print(std::ostream &out) const {out << "Particle-in-cell space charge field" << std::endl;}
  BTField* Clone() const;

  //Iterate over all events and add up the coulomb force
  void GetFieldValue     ( const double Point[4], double *EMfield )   const;
  void GetVectorPotential( const double Point[4], double *potential ) const;
  //ALL u must be at the same t
  void CalculateField(int n_events, const double * u_vector, const double * q_vector, const double* macro_vector);
  
  //Get Grid and Interpolators - but BTPIC_SC still owns the memory!
  ThreeDGrid*             GetGrid()               const  {return _grid;}
  Interpolator3dGridTo3d* GetBInterpolator()      const  {return _bInterpolator;}
  Interpolator3dGridTo3d* GetEInterpolator()      const  {return _eInterpolator;}
  Interpolator3dGridTo1d* GetAInterpolator(int i) const  {return _aInterpolator[i];}

  bool                    WillCalculateFields()   const  {return _calculateFields;}
  bool                    WillCalculateFields(bool a)    {return _calculateFields = a;}

  bool                    WillCalculatePotential() const {return _calculatePot;}
  bool                    WillCalculatePotential(bool a) {return _calculatePot = a;}

private:
  bool _calculateFields;
  bool _calculatePot;
  //Rectangular space grid
  ThreeDGrid* _grid;
  //Interpolator
  Interpolator3dGridTo3d*              _bInterpolator; //b-field
  Interpolator3dGridTo3d*              _eInterpolator; //e-field
  std::vector<Interpolator3dGridTo1d*> _aInterpolator; //a-field (a bit slower to do the mesh lookup 4 times)

  std::vector<int>                     _gridSize;
  double _rMin2; //truncate coulomb field near to grid points
  
  //turn *x into an array like x[xsize][ysize][zsize]
  void Build (double**** x);
  void Delete(double**** x);

};

#endif
