// MAUS WARNING: THIS IS LEGACY CODE.
#include "BTField.hh"
#include "Interface/Differentiator.hh"
#include "Interface/Interpolator.hh"
#include "Maths/Matrix.hh"
#include "Maths/Vector.hh"


//div.E
double BTField::Maxwell1(const double point[4], const double delta[4]) const
{
  void (BTField::*fieldvalue)(const double*, double*) const = &BTField::GetFieldValue;
  MemberFunctionConst<const BTField>* func = new MemberFunctionConst<const BTField>(*this, fieldvalue, 4, 6);
  Differentiator*                diff = new Differentiator(func, std::vector<double>(&delta[0], &delta[0]+4), std::vector<double>(2,1));
  MAUS::Matrix<double> differentials(6,5,0.);
  MAUS::Vector<double> aPoint(point, 4);
  for(int i=0; i<4; i++) aPoint(i+1) = point[i];
  diff->F(aPoint, differentials);

  double result = 0;
  for(int i=0; i<3; i++) result += differentials(i+4,i+2);
  return result;
}

//div.B
double BTField::Maxwell2(const double point[4], const double delta[4]) const
{
  MemberFunctionConst<const BTField>* func = new MemberFunctionConst<const BTField>(*this, &BTField::GetFieldValue, 4, 6);
  std::vector<double> deltas(delta, delta+4);
  std::vector<double> magnitudes(2, 1);
  Differentiator* diff = new Differentiator(func, deltas, magnitudes);
  MAUS::Matrix<double> differentials(6,5,0.);
  MAUS::Vector<double> aPoint(point, 4);
  diff->F(aPoint, differentials);

  double result = 0;
  for(int i=0; i<3; i++) result += differentials(i+1,i+2);
  return result;
}

//curl E + dB/dt (3-vector)
CLHEP::Hep3Vector BTField::Maxwell3(const double point[4], const double delta[4]) const
{
  MemberFunctionConst<const BTField>* func = new MemberFunctionConst<const BTField>(*this, &BTField::GetFieldValue, 4, 6);
  Differentiator*                diff = new Differentiator(func, std::vector<double>(&delta[0], &delta[0]+4), std::vector<double>(2,1));
  MAUS::Matrix<double> differentials(6,5,0.);
  MAUS::Vector<double> aPoint(point, 4);
  diff->F(aPoint, differentials);

  CLHEP::Hep3Vector result;
  result[0] = differentials(6,3) - differentials(5,4) + differentials(1,5); 
  result[1] = differentials(6,2) - differentials(4,4) + differentials(2,5); 
  result[2] = differentials(5,2) - differentials(4,3) + differentials(3,5); 
  return result;
}

//curl B + mu eps dE/dt
CLHEP::Hep3Vector BTField::Maxwell4(const double point[4], const double delta[4]) const
{
  MemberFunctionConst<const BTField>* func = new MemberFunctionConst<const BTField>(*this, &BTField::GetFieldValue, 4, 6);
  Differentiator*                diff = new Differentiator(func, std::vector<double>(&delta[0], &delta[0]+4), std::vector<double>(2,1));
  MAUS::Matrix<double> differentials(6,5,0.);
  MAUS::Vector<double> aPoint(point, 4);
  diff->F(aPoint, differentials);

  double mueps = CLHEP::mu0*CLHEP::epsilon0;
  CLHEP::Hep3Vector result;
  result[0] = differentials(3,3) - differentials(2,4) + mueps*differentials(4,5); 
  result[1] = differentials(3,2) - differentials(1,4) + mueps*differentials(5,5); 
  result[2] = differentials(2,2) - differentials(1,3) + mueps*differentials(6,5); 
  return result;
}

void BTField::FieldFromVectorPotential(const double point[4], const double delta[4], double * field) const
{
  MemberFunctionConst<const BTField>* func = new MemberFunctionConst<const BTField>(*this, &BTField::GetVectorPotential, 4, 4);
  Differentiator*                diff = new Differentiator(func, std::vector<double>(&delta[0], &delta[0]+4), std::vector<double>(2,1));
  MAUS::Matrix<double> differentials(6,5,0.);
  MAUS::Vector<double> aPoint(point, 4);
  diff->F(aPoint, differentials);
  //B = Curl A // d_j y_i
  field[0] = differentials(3,3) - differentials(2,4);
  field[1] = differentials(3,2) - differentials(1,4);
  field[2] = differentials(2,2) - differentials(1,3);
  //E = Grad phi - dA/dt // d_j y_i
  field[3] = differentials(4,2) + differentials(1,5);
  field[4] = differentials(4,3) + differentials(2,5);
  field[5] = differentials(4,4) + differentials(3,5);
}

void BTField::FieldFromVectorPotentialDifferential(const double point[4], double * field) const
{
  double differentials[4][4];
  for(int i=0; i<4; i++)
  {
    for(int j=0; j<4; j++) differentials[i][j] = 0;
    GetVectorPotentialDifferential(point, differentials[i], i); //d_i A_j
  }

  //As above, but matrix indexing is in a different order
  //B = Curl A // d_i y_j
  field[0] = differentials[1][2] - differentials[2][1]; //dAy/dz - dAz/dy
  field[1] = differentials[0][2] - differentials[2][0]; //dAx/dz - dAx/dz
  field[2] = differentials[0][1] - differentials[1][0]; //dAx/dy - dAy/dx
  //E = Grad phi - dA/dt // d_j y_i
  field[3] = differentials[3][0] + differentials[0][3]; 
  field[4] = differentials[3][1] + differentials[1][3];
  field[5] = differentials[3][2] + differentials[2][3];
}

std::ostream& operator<<(std::ostream& out, const BTField& field)
{
  (&field)->Print(out);
  return out;
}


