
#ifndef BTFASTSOLENOID_HH
#define BTFASTSOLENOID_HH

#include "BTField.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
// BTFastSolenoid - not really terribly fast!
// Uses an analytical form for the on-axis field and it's derivatives; then a standard expansion
// to get the field off axis. For now, only tanh fields have been implemented,
// could also add an enge model if there is a need
// BUG option to use a lookup table for the on-axis field, rather than calculating it each time
// but this is broken right now
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

class BTFastSolenoid: public BTField
{
public:
  //end field is used to flag the type of field
  enum endField {tanh, glaser, lookup};
  //Fast solenoid algorithm
  //tanh uses tanh end field; glaser uses glaser endfield (1/1+x^2); glaser_lookup and tanh_lookup use the appropriate endfield but with a lookup table to store the differentials
  BTFastSolenoid();
  //tanh constructor
  static BTFastSolenoid BTFastTanh  (double peakField, double efoldDistance, double length, double z_max, double r_max, int highestOrder); 
  //glaser constructor
  static BTFastSolenoid BTFastGlaser(double peakField, double lambda, double z_max, double r_max, int highestOrder); 
  //set into lookup mode, where BTFastSolenoid keeps a list of differentials on-axis instead of calculating each time
  void           DoLookup(double stepSize);

  //Destructor
  ~BTFastSolenoid();

  BTFastSolenoid * Clone() const {return new BTFastSolenoid(*this);}

  //Return field at cartesian position, time Point[4] = [x,y,z,t].
  void   GetFieldValue ( const double  Point[4], double *Bfield ) const;
  double BzDifferential( const double& z, const int& order, endField model) const;
  void   Print(std::ostream &out) const;

  //Return the 4-vector potential at a point; potential should be a pointer initialised to double[4]
  void                    GetVectorPotential(const double point[4], double * potential) const;
  //Return the first differential of potential
  //potential[j] = dA_j/dx_i of the 4-vector potential at a point. i = axis
  void                    GetVectorPotentialDifferential(const double point[4], double * diff, int axis) const;

private:
  //generic parameters
  endField _model;
  int      _highestOrder;
  double   _zMax;
  double   _rMax;
  double   _peakField;

  //tanh parameters
  double _length;
  double _efoldDistance;

  //glaser parameters
  double _lambda;

  //lookup table parameters
  endField             _oldModel; //model before it was lookup
  std::vector<double*> _fields;
  double               _zStep;
}; // class BTFastSolenoid

namespace Tanh2
{

//d^(n)tanh[(x-x0)/lambda]/dx^(n) 
double Tanh(const double& x, const int& n, const double& lambda, const double& x0);

//I'm not clever enough to figure out analytical formula for y^(n)
//But I can figure out a recursion relation for y^(n+1) ito y^(n)
//So I dynamically build the set of tanh and sech functions for each
//differential as needed
void                AddTanhDiffIndex(int n);
std::vector< std::vector<int> > TanhDiffIndices(int n);


}

namespace Glaser //Lorentz function like 1/(1+x^2)
{
//y         = 1/(1+z^2/lambda^2)
//d^ny/dx^n = 1*sum_i (a0_in * x^a1_in * (1+x^2)^-a2_in)
//n is differential order (n=0 means no differential)
//lambda is scaling for x "end field length"
double Glaser(const double& x, const int& n, const double& lambda);

//I'm not clever enough to figure out analytical formula for y^(n)
//But I can figure out a recursion relation for y^(n+1) ito y^(n)
//So I dynamically build the 1D polynomial for each differential as needed
void   AddGlaserDiffIndex(unsigned int n);
//indexes functions like a2 * x^a1 * (1+x^2)^-a0
//where y^(n) = sum_i (a2_i * x^a1_i * (1+x^2)^-a0_i)
//and a2_i = gdi[n][i][0], a1_i = gdi[n][i][1], a0_i = gdi[n][i][2]
std::vector< std::vector<int*> >        GlaserDiffIndices(int n);
//return a[0] > b[0]; then if a[0] == b[0] by a[1] > b[1] and so on
template<unsigned int len, class T> bool less_than( T a[], T b[] );

}

#endif  // BTFASTSOLENOID_HH
