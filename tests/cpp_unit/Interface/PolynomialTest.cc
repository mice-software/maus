#include <iostream>
#include <iomanip>

#include "gtest/gtest.h"

#include "Interface/Squeak.hh"
#include "Interface/MVector.hh"
#include "Interface/MMatrix.hh"
#include "Interface/MMatrixToCLHEP.hh"
#include "Interface/PolynomialVector.hh"
#include "Interface/Differentiator.hh"
#include "Config/ModuleConverter.hh"

/// NOTE: tests PolynomialVector, Differentiator and PolynomialInterpolator classes ///
/// Three classes that are very closely related anyway... ///

using namespace G4MICE; 

bool PolynomialVectorTest();
bool PolynomialLeastSquaresTest();
bool PolyVector_PointBox_Test();
bool PolyVector_GetAvgChi2OfDifference_Test();
bool PolyVector_Means_Test();
bool PolyVector_Covariances_Test();
bool PolyVectorFTest(const PolynomialVector& vecP, double point[], double answer[]);
bool PolyVectorCoeffTest(const PolynomialVector& vecP);
bool PolynomialInterpolatorTest();
bool TestF(PolynomialInterpolator* pInt, double tolerance);

bool PolynomialVectorTest()
{
    Squeak::mout(Squeak::debug) << "PolynomialVectorTest" << std::endl;
    bool testpass = true;
    CLHEP::HepMatrix coeffHep(3,6);
    for(unsigned int i=0; i<6; i++) coeffHep[0][i] =  i+1;
    for(unsigned int i=0; i<6; i++) coeffHep[1][i] =  i*i;
    for(unsigned int i=0; i<6; i++) coeffHep[2][i] = -int(i)-1;
    Squeak::mout(Squeak::debug) << coeffHep << std::endl;
    PolynomialVector* vecP  = new PolynomialVector(2, CLHEP_to_MMatrix( coeffHep ));
    PolynomialVector* clone = vecP->Clone();
    delete vecP;
    Squeak::mout(Squeak::debug) << "Ctor " << true << std::endl;
    Squeak::mout(Squeak::debug) << "Dtor " << true << std::endl;
    Squeak::mout(Squeak::debug) << "Clone " << true << std::endl;
    testpass &= clone->PointDimension() == 2;
    Squeak::mout(Squeak::debug) << "PointDimension " << testpass << std::endl;
    testpass &= clone->ValueDimension() == 3;
    Squeak::mout(Squeak::debug) << "ValueDimension " << testpass << std::endl;
    std::vector<PolynomialVector::PolynomialCoefficient> coeffs = clone->GetCoefficientsAsVector();
    PolynomialVector testVec(coeffs);
    testpass &= testVec.PointDimension() == 2;
    Squeak::mout(Squeak::debug) << "PointDimension Coeffs " << testpass << std::endl;
    testpass &= testVec.ValueDimension() == 3;
    Squeak::mout(Squeak::debug) << "ValueDimension Coeffs " << testpass << std::endl;

    double point1[2]  = {0,0};
    double answer1[3] = {1,0,-1};
    testpass &= PolyVectorFTest(*clone, point1, answer1);
    Squeak::mout(Squeak::debug) << "FTest1 " << testpass << std::endl;
    testpass &= PolyVectorFTest(testVec, point1, answer1);
    Squeak::mout(Squeak::debug) << "PolynomialCoefficients " << testpass << std::endl;

    double point2[2]  = {3,-2};
    double answer2[3] = {31,80,-31};
    testpass &= PolyVectorFTest(*clone, point2, answer2);
    Squeak::mout(Squeak::debug) << "FTest2 " << testpass << std::endl;
    testpass &= PolyVectorFTest(testVec, point2, answer2);
    Squeak::mout(Squeak::debug) << "PolynomialCoefficients " << testpass << std::endl;
    for(int i=0; i<5; i++) Squeak::mout(Squeak::debug) << "nPCoeffs(3," << i << ") " << PolynomialVector::NumberOfPolynomialCoefficients(3,i) << std::endl;
    for(int i=0; i<20; i++) 
    {
        std::vector<int> indexP = PolynomialVector::IndexByPower(i, 3);
        std::vector<int> indexV = PolynomialVector::IndexByVector(i, 3);
        Squeak::mout(Squeak::debug) << std::setw(5) << i << "   *|* ";
        for(unsigned int j=0; j<indexP.size(); j++) Squeak::mout(Squeak::debug) << std::setw(5) << indexP[j] << " ";
        Squeak::mout(Squeak::debug) << "  *|*  ";
        for(unsigned int j=0; j<indexV.size(); j++) Squeak::mout(Squeak::debug) << std::setw(5) << indexV[j] << " ";
        Squeak::mout(Squeak::debug) << std::endl;
    }
    delete clone;

    testpass &= PolyVector_PointBox_Test();
    Squeak::mout(Squeak::debug) << "PointBoxTest " << testpass << std::endl;
    testpass &= PolyVector_Means_Test();
    Squeak::mout(Squeak::debug) << "MeansTest " << testpass << std::endl;
    testpass &= PolyVector_Covariances_Test();
    Squeak::mout(Squeak::debug) << "CovariancesTest " << testpass << std::endl;
    testpass &= PolyVector_GetAvgChi2OfDifference_Test();
    Squeak::mout(Squeak::debug) << "PolyVector_GetAvgChi2OfDifference_Test " << testpass << std::endl;

    if(!testpass) Squeak::mout(Squeak::debug) << "FAILED PolynomialVectorTest" << std::endl;
    return testpass;
}

bool PolyVector_IterableEquality_Test()
{
  bool testpass = true;
  
  std::vector<int> a;
  std::vector<int> b;
  testpass &=  PolynomialVector::IterableEquality(a, b, a.begin(), a.end(), b.begin(), b.end());
  a.push_back(-2);
  testpass &= !PolynomialVector::IterableEquality(a, b, a.begin(), a.end(), b.begin(), b.end());
  b.push_back(-2);
  testpass &=  PolynomialVector::IterableEquality(a, b, a.begin(), a.end(), b.begin(), b.end());
  b.push_back(-2);
  testpass &= !PolynomialVector::IterableEquality(a, b, a.begin(), a.end(), b.begin(), b.end());
  return testpass;
}

bool PolyVector_PointBox_Test()
{
  bool testpass = true;
  double deltaA[] = {2.,3.,4.,5.};
  std::vector<double> delta(deltaA, deltaA+4);
  std::vector<std::vector<double> > ps = PolynomialVector::PointBox(delta, 6);
  for(size_t i=0; i<ps.size(); i++)
  {
    bool   on_edge = false;
    for(size_t j=0; j<ps[i].size(); j++) {
      on_edge = on_edge || fabs( delta[j]-fabs(ps[i][j]) )<1e-9; //at least one of ps[i] must be == +- delta for it to be on edge
    }
    if(!on_edge) {
      for(size_t j=0; j<ps[i].size(); j++) Squeak::mout(Squeak::debug) << ps[i][j] << " ";
      Squeak::mout(Squeak::debug) << on_edge << " " << testpass << std::endl;
    }
    testpass &= on_edge;
  }

  return testpass;
}

bool PolyVector_GetAvgChi2OfDifference_Test()
{
  bool testpass = true;
  double mat_data[] = {1,4,1,  2,3,2,  7,11,7, 13,3,13, 5,7,8, 0,9,2, 1,3,4};
  MMatrix<double>  mat(3,6,mat_data);
  PolynomialVector pvec(2, mat);
  std::vector< std::vector<double> > in;
  std::vector< std::vector<double> > out;
  std::vector< std::vector<double> > out_neg;
  try{ pvec.GetAvgChi2OfDifference(in,out); testpass = false;}
  catch (Squeal squee) {}
  for(int i=0; i<10; i++)  
  {
    in.push_back(std::vector<double>(2,i));
    out    .push_back(std::vector<double>(3,0));
    out_neg.push_back(std::vector<double>(3,0));
    in.back()[1] = i*2;
    pvec.F(&in.back()[0],&out.back()[0]);
    for(int i=0; i<3; i++) {
      out_neg.back()[i] = -out.back()[i]; 
    }
  }
  double avg_chi2 = pvec.GetAvgChi2OfDifference(in,out    );
  testpass &= fabs(avg_chi2) < 1e-6;
  avg_chi2 = pvec.GetAvgChi2OfDifference(in,out_neg);
  testpass &= fabs( (avg_chi2/1e14 - 4.90542) ) < 1e-3; //Hope that is right
  return testpass;
}

bool PolyVector_Means_Test()
{
  std::vector<std::vector<double> > values;
  values.push_back(std::vector<double>());
  for(size_t i=1; i<=3; i++) values.back().push_back( i );
  values.push_back(std::vector<double>());
  for(size_t i=1; i<=3; i++) values.back().push_back( -double(i) );
  values.push_back(std::vector<double>());
  for(size_t i=1; i<=3; i++) values.back().push_back( i*i );
  std::vector<double> weights1(3, 1.);
  std::vector<double> weights2(3, 1.);
  weights2[2] = 0.;

  MVector<double> v1 = PolynomialVector::Means(values, std::vector<double>());
  MVector<double> v2 = PolynomialVector::Means(values, weights1);
  MVector<double> v3 = PolynomialVector::Means(values, weights2);

  bool testpass = true;
  for(size_t i=1; i<=3; i++)
  {
    testpass &= fabs( v1(i)-double(i*i)/3. ) < 1e-6;
    testpass &= fabs( v2(i)-double(i*i)/3. ) < 1e-6;
    testpass &= fabs( v3(i)             ) < 1e-6;
  }
  return testpass;
}

bool PolyVector_Covariances_Test()
{
  std::vector<std::vector<double> > values;
  values.push_back(std::vector<double>());
  for(size_t i=0; i<3; i++) values.back().push_back(1);
  values.push_back(std::vector<double>());
  for(size_t i=0; i<3; i++) values.back().push_back(1);
  values.push_back(std::vector<double>());
  for(size_t i=0; i<3; i++) values.back().push_back(-1);
  values.push_back(std::vector<double>());
  for(size_t i=0; i<3; i++) values.back().push_back(-1);
  MVector<double> orig1(3, 0.);
  MVector<double> orig2(3, -1.);
  std::vector<double> weights1(4, 1.);
  std::vector<double> weights2(4, 1.);
  weights2[0] = 0.;
  weights2[1] = 0.;

  MMatrix<double> m1 = PolynomialVector::Covariances(values, std::vector<double>(), MVector<double>());
  MMatrix<double> m2 = PolynomialVector::Covariances(values, weights1, orig1);
  MMatrix<double> m3 = PolynomialVector::Covariances(values, weights2, orig2);

  bool testpass = true;
  testpass &= fabs((m1-MMatrix<double>(3,3,1.)).determinant()) < 1e-6;
  testpass &= fabs((m2-MMatrix<double>(3,3,1.)).determinant()) < 1e-6;
  testpass &= fabs((m3.determinant())) < 1e-6;
  return testpass;
}

bool PolyVectorFTest(const PolynomialVector& vecP, double point[], double answer[])
{
    bool testpass = true;
    MVector<double> valueMV(3,0);
    MVector<double> pointMV(2,0);
    for(int i=1; i<=2; i++) pointMV(i) = point[i-1];
    Squeak::mout(Squeak::debug) << "PolyVectorFTest" << std::endl;
    vecP.F(pointMV, valueMV);
    Squeak::mout(Squeak::debug) << "PolyVectorFTest" << std::endl;
    for(int i=0; i<3; i++) testpass &= fabs(valueMV(i+1) - answer[i]) < 1e-9;
    for(int i=0; i<3; i++) Squeak::mout(Squeak::debug) << valueMV(i+1) << " ";
    Squeak::mout(Squeak::debug) << "Hep F(*,*) " << testpass << std::endl;

    std::vector<double> valueVec(3,-2);
    std::vector<double> pointVec(2,0);
    for(int i=0; i<2; i++) pointVec[i] = point[i];
    vecP.F(&pointVec[0], &valueVec[0]);
    for(int i=0; i<3; i++) testpass &= fabs(valueVec[i] - answer[i]) < 1e-9;
    for(int i=0; i<3; i++) Squeak::mout(Squeak::debug) << valueVec[i] << " ";
    Squeak::mout(Squeak::debug) << "Vec F(*,*) " << testpass << std::endl;

    return testpass;
}

bool PolynomialLeastSquaresTest()
{
  Squeak::mout(Squeak::debug) << "PolynomialLeastSquaresTest" << std::endl;
  int               nX   = 4;
  int               nY   = 4;
  int               nZ   = 4;
  CLHEP::HepMatrix mat(3,4,0);
  mat[0][0] = 1.;
  mat[1][0] = 4.;
  mat[2][0] = 1.;
  mat[0][1] = 2.;
  mat[1][1] = 3.;
  mat[2][1] = 2.;
  mat[0][2] = 7.;
  mat[1][2] = 11.;
  mat[2][2] = 7.;
  mat[0][3] = 13.;
  mat[1][3] = 3.;
  mat[2][3] = 13.;

  PolynomialVector* vecF = new PolynomialVector(3, CLHEP_to_MMatrix(mat) );
  std::vector< std::vector<double> > points(nX*nY*nZ, std::vector<double>(3));
  std::vector< std::vector<double> > values(nX*nY*nZ, std::vector<double>(3));
  std::vector<double>                weights(nX*nY*nZ, 1);
  for(int i=0; i<nX; i++)
    for(int j=0; j<nY; j++)
      for(int k=0; k<nZ; k++)
      {
        int a = k+j*nZ + i*nY*nZ;
        points[a][0] = i/double(nX)*105.;
        points[a][1] = j/double(nY)*201.;
        points[a][2] = k/double(nZ)*105.;
        vecF->F(&points[a][0], &values[a][0]);
      }
  delete vecF;
  PolynomialVector* pVec    = PolynomialVector::PolynomialLeastSquaresFit(points, values, 2);
  CLHEP::HepMatrix recCoeff = MMatrix_to_CLHEP( pVec->GetCoefficientsAsMatrix() );
  bool testpass=true;
  Squeak::mout(Squeak::debug) << "Input" << mat << "Output" << recCoeff << std::endl;
  for(int i=0; i<recCoeff.num_row(); i++)
    for(int j=0; j<recCoeff.num_col(); j++)
      if( fabs(recCoeff[i][j] - mat[i][j]) > 1e-6 ) testpass=false;
  delete pVec;

  //now add an outlier with 0 weight - try weighted fit
  points .push_back(std::vector<double>(3,92.));
  values .push_back(std::vector<double>(3,17.));
  weights.push_back(0.);
  pVec     = PolynomialVector::PolynomialLeastSquaresFit(points, values, 2, weights);
  recCoeff = MMatrix_to_CLHEP( pVec->GetCoefficientsAsMatrix() );
  Squeak::mout(Squeak::debug) << "Weighted Input" << mat << "Weighted Output" << recCoeff << std::endl;
  for(int i=0; i<recCoeff.num_row(); i++)
    for(int j=0; j<recCoeff.num_col(); j++)
      if( fabs(recCoeff[i][j] - mat[i][j]) > 1e-6 ) testpass=false;
  delete pVec;

  //now take some of the input values, try for a constrained fit
  PolynomialVector Constraint(2, CLHEP_to_MMatrix( mat.sub(1,2,1,3) ));
  std::vector<PolynomialVector::PolynomialCoefficient> coeffs = Constraint.GetCoefficientsAsVector();
  for(int i=0; i<2; i++)
    coeffs.erase(coeffs.begin());
  PolynomialVector* constraintPVec = new PolynomialVector(coeffs);
  pVec     = PolynomialVector::ConstrainedPolynomialLeastSquaresFit(points, values, 2, constraintPVec->GetCoefficientsAsVector(), weights);
  recCoeff = MMatrix_to_CLHEP( pVec->GetCoefficientsAsMatrix() );
  Squeak::mout(Squeak::debug) << "Constrained Input\n" << *constraintPVec << "Constrained Output\n" << *pVec << std::endl;
  for(int i=0; i<recCoeff.num_row(); i++)
    for(int j=0; j<recCoeff.num_col(); j++)
      if( fabs(recCoeff[i][j] - mat[i][j]) > 1e-6 ) testpass=false;
  delete pVec;

  //should return a copy of 
  double mat2[] = {1.,1.,1., 2.,3.,4., 7.,8.,9., 13.,14.,15., 200.,300.,400., 500.,600.,700., 800.,900.,1000., 1100.,1200.,1300., 1400.,1500.,1600., 1700.,1800.,1900.};
  Squeak::mout(Squeak::debug) << "Chi2SweepingLeastSquaresFit" << std::endl;
  PolynomialVector* testF = new PolynomialVector(3, MMatrix<double>(3, 10, mat2) );
  std::vector<double> delta(3);
  delta[0] = 1e-50;
  delta[1] = 1e-50;
  delta[2] = 1e-50;
  //check we can make a polynomial vector at all
  pVec  = PolynomialVector::Chi2SweepingLeastSquaresFit
                          (*testF, 5, std::vector< PolynomialVector::PolynomialCoefficient >() , 
                           1e-40, delta, 10., 100);
  if (pVec == NULL) {
    testpass = false;
    Squeak::mout(Squeak::debug) << "Failed to make PolynomialVector when PolynomialVector expected " << pVec << std::endl;
  }
  else {
    MMatrix<double> o1 = pVec ->GetCoefficientsAsMatrix();
    MMatrix<double> o2 = testF->GetCoefficientsAsMatrix();
    testpass &= o1.num_row() == o2.num_row() && o1.num_col() == o2.num_col();
    for(size_t i=1; i<=o2.num_row(); i++)
      for(size_t j=1; j<=o2.num_col(); j++)
        testpass &= fabs( o1(i,j) - o2(i,j) ) < 1e-2;
    Squeak::mout(Squeak::debug) << "Input should be same as output\nInput\n" << *testF 
                                << "Output\n" << *pVec << " testpass " << testpass << std::endl;

  }
  if (pVec != NULL) delete pVec;


  //check we can get a decent fit at lower order to a higher order polynomial
  delta[0] = 1e-50; delta[1] = 1e-50; delta[2] = 1e-50;
  pVec  = PolynomialVector::Chi2SweepingLeastSquaresFit
                          (*testF, 2, std::vector< PolynomialVector::PolynomialCoefficient >() , 
                           1e-40, delta, 10., 100);
  if (pVec == NULL) {
    testpass = false;
    Squeak::mout(Squeak::debug) << "Failed to make PolynomialVector when PolynomialVector expected " << pVec << std::endl;
  }
  else {
    MMatrix<double> o1 = pVec ->GetCoefficientsAsMatrix();
    MMatrix<double> o2 = testF->GetCoefficientsAsMatrix();
    testpass &= o1.num_row() == o2.num_row() && o1.num_col() == 4;
    for(size_t i=1; i<=o1.num_row(); i++)
      for(size_t j=1; j<=o1.num_col(); j++)
        testpass &= fabs( o1(i,j) - o2(i,j) ) < 1e-6;
    Squeak::mout(Squeak::debug) << "Input should be same as output\nInput\n" << *testF 
                                << "Output\n" << *pVec << " testpass " << testpass << std::endl;
  }



  //test that I return NULL if I can't converge; for these parameters floating point precision should
  //mean that I don't converge.
  delta[0] = 1e6;
  delta[1] = 1e6;
  delta[2] = 1e6;
  pVec  = PolynomialVector::Chi2SweepingLeastSquaresFit
                          (*testF, 5, std::vector< PolynomialVector::PolynomialCoefficient >() , 
                           1e-60, delta, 10., 100);
  if (pVec != NULL) 
    testpass = false;
  Squeak::mout(Squeak::debug) << "Should be NULL " << pVec << " testpass " << testpass << std::endl;

  //the point of mat3 is that it introduces an effective unit into each dimension;
  //so x_0 has units like 1., x_1 has units like 1e6 and x_2 has units like 1e3
  //validity region (delta at output) should reflect the units
  double deltaMaxA[] = {1e12, 1e12, 1e12, 1e12};
  std::vector<double> deltaMax(deltaMaxA, deltaMaxA+3);
  double mat3[] = {1.,1.,1., 
                   2.,3.,4., 
                   7.e6,8.e6,9.e6, 
                   13.e3,14.e3,15.e3, 
                   200.,300.,400., 
                   500.e6,600.e6,700.e6, 
                   800.e3,900.e3,1000.e3, 
                   1100.e12,1200.e12,1300.e12, 
                   1400.e9,1500.e9,1600.e9, 
                   1700.e6,1800.e6,1900.e6
  };
  PolynomialVector* testF2 = new PolynomialVector(3, MMatrix<double>(3, 10, mat3) );
  Squeak::mout(Squeak::debug) << "Chi2SweepingLeastSquaresFitVariableWalls" << std::endl;
 // delta.push_back(0.);
  delta[0] = 1e-50; delta[1] = 1e-50; delta[2] = 1e-50;
  pVec  = PolynomialVector::Chi2SweepingLeastSquaresFitVariableWalls
                          (*testF2, 3, std::vector< PolynomialVector::PolynomialCoefficient >() , 
                           1e-20, delta, 10., 100, deltaMax);
  Squeak::mout(Squeak::debug) << "delta variable walls: ";
  for(size_t i=0; i<delta.size(); i++) Squeak::mout(Squeak::debug) << delta[i] << " ";
  Squeak::mout(Squeak::debug) << std::endl;
  delta[0] = 1e-50; delta[1] = 1e-50; delta[2] = 1e-50;
  PolynomialVector* pVec2  = PolynomialVector::Chi2SweepingLeastSquaresFit
                          (*testF2, 3, std::vector< PolynomialVector::PolynomialCoefficient >() , 
                           1e-20, delta, 10., 100);
  Squeak::mout(Squeak::debug) << "delta fixed walls:    ";
  for(size_t i=0; i<delta.size(); i++) Squeak::mout(Squeak::debug) << delta[i] << " ";
  Squeak::mout(Squeak::debug) << std::endl;
  if (pVec == NULL) {
    Squeak::mout(Squeak::debug) << "Error - Chi2SweepingLeastSquaresFitVariableWalls returns " << pVec << std::endl;
    testpass = false;
  }
  else {
    Squeak::mout(Squeak::debug) << "Input should be same as output\nInput\n" << *testF2 
                                << "Output\n" << *pVec 
                                << "For comparison, using normal algorithm\n" << *pVec2 << std::endl;
    MMatrix<double> o1 = pVec  ->GetCoefficientsAsMatrix();
    MMatrix<double> o2 = testF2->GetCoefficientsAsMatrix();
    testpass &= o1.num_row() == o2.num_row() && o1.num_col() == o2.num_col();
    for(size_t i=1; i<=o1.num_row() && i<=o2.num_row(); i++)
      for(size_t j=1; j<=o1.num_col() && j<=o2.num_col(); j++) {
        testpass &= fabs( o1(i,j) - o2(i,j) )/fabs( o1(i,j) + o2(i,j) ) < 1.e-3 || fabs( o1(i,j) + o2(i,j) ) < 1e-9;
     }
  }
  if (pVec != NULL) delete pVec;
  Squeak::mout(Squeak::debug) << "testpass " << testpass << std::endl;

  //test that I return NULL if I can't converge; for these parameters floating point precision should
  //mean that I don't converge.
  delta[0] = 1e6; delta[1] = 1e6; delta[2] = 1e6;
  pVec  = PolynomialVector::Chi2SweepingLeastSquaresFitVariableWalls
                          (*testF2, 5, std::vector< PolynomialVector::PolynomialCoefficient >() , 
                           1e-60, delta, 10., 100, deltaMax);
  if (pVec != NULL)
    testpass = false;
  Squeak::mout(Squeak::debug) << "Should be NULL " << pVec << std::endl;

  delete testF;
  Squeak::mout(Squeak::debug) << "PolynomialLeastSquaresTest " << *testF2 << testpass << std::endl;
  return testpass;
}

TEST(PolynomialTest, old_unit_tests)
{
    EXPECT_TRUE(PolynomialLeastSquaresTest());
}


