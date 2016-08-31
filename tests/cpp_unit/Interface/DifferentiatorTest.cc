#include <iostream>
#include <iomanip>

#include "gtest/gtest.h" 

#include "CLHEP/Matrix/Matrix.h"

#include "Config/ModuleConverter.hh"
#include "BeamTools/BTFieldConstructor.hh"
#include "BeamTools/BTTracker.hh"
#include "Utils/Squeak.hh"
#include "Interface/Differentiator.hh"
#include "Maths/Matrix.hh"
#include "Maths/PolynomialMap.hh"
#include "Maths/Vector.hh"

/// NOTE: tests MAUS::PolynomialMap, Differentiator and PolynomialInterpolator classes ///
/// Three classes that are very closely related anyway... ///
bool DifferentiatorTest();
bool PolynomialInterpolatorTest();
bool TestF(PolynomialInterpolator* pInt, double tolerance);


bool DifferentiatorTest()
{
    bool testpass = true;
    CLHEP::HepMatrix coeffHep(3,6);
    for(unsigned int i=0; i<6; i++) coeffHep[0][i] =  i+1;
    for(unsigned int i=0; i<6; i++) coeffHep[1][i] =  i*i;
    for(unsigned int i=0; i<6; i++) coeffHep[2][i] = -int(i)-1;
    Squeak::mout(Squeak::debug) << coeffHep << std::endl;
    MAUS::PolynomialMap* vecP  = new MAUS::PolynomialMap(2, MAUS::Matrix<double>(coeffHep));

    std::vector<double> delta(2, 0.1);
    std::vector<double> mag  (3, 1);
    Differentiator* diff  = new Differentiator(vecP, delta, mag);
    Differentiator* clone = diff->Clone();
    delete diff;
    Squeak::mout(Squeak::debug) << "Diff, Clone ~Diff: " << true << std::endl;
    testpass &= clone->FunctionMap() == vecP;
    Squeak::mout(Squeak::debug) << "FunctionMap: " << testpass << std::endl;
    testpass &= vecP->PointDimension() == clone->PointDimension();
    Squeak::mout(Squeak::debug) << "PointDimension: " << testpass << std::endl;
    testpass &= clone->NumberOfDiffRows()
                == MAUS::PolynomialMap::NumberOfPolynomialCoefficients(
                    delta.size(), mag.size()-1);
    Squeak::mout(Squeak::debug) << "NumberOfDiffRows: " << testpass << std::endl;
    testpass &= clone->ValueDimension() == clone->NumberOfDiffRows()*clone->FunctionMap()->ValueDimension();
    Squeak::mout(Squeak::debug) << "ValueDim: " << testpass << std::endl;
    double point [2] = {3,-2};
    double value [3] = {0,0,0};
    double value2[3] = {0,0,0};
    ///Y
    clone->Y(point, value);
    testpass &= fabs(value[0] - 31)<1e-9 && (value[1]- 80)<1e-9 && (value[2]- (-31))<1e-9;
    Squeak::mout(Squeak::debug) << "Y: " << value[0] << " " << value[1] << " " << value[2] << " * " << testpass << std::endl;
    ///PolyFromDiffs
    MAUS::PolynomialMap* vecP2 = clone->PolynomialFromDifferentials(point);
    vecP2->F(point, value2);
    testpass &= fabs(value[0] - value2[0]) < 1e-5 && fabs(value[1] - value2[1]) < 1e-5 && fabs(value[2] - value2[2]) < 1e-5;
    Squeak::mout(Squeak::debug) << "PolyFromDiffs:  " << value2[0] << " " << value2[1] << " " << value2[2] << " * " << testpass << std::endl;
    ///PolynomialMap
    MAUS::Vector<double> pointHep(2,0.);
    pointHep(1) = 3; pointHep(2) = -2;
    MAUS::Matrix<double> valueHep = clone->PolynomialMap(pointHep);
    for(int i=0; i<coeffHep.num_row(); i++) for(int j=0; j<coeffHep.num_col(); j++) testpass &= fabs(coeffHep[i][j] - valueHep(i+1,j+1)) < 1e-5;
    Squeak::mout(Squeak::debug) << valueHep << "PolyMap1: " << testpass << std::endl;
    pointHep(1) = 0; pointHep(2) = 0;
    valueHep = clone->PolynomialMap(pointHep);
    for(int i=0; i<coeffHep.num_row(); i++) for(int j=0; j<coeffHep.num_col(); j++) testpass &= fabs(coeffHep[i][j] - valueHep(i+1,j+1)) < 1e-5;
    Squeak::mout(Squeak::debug) << valueHep << "PolyMap2: " << testpass << std::endl;
    ///CentredPolynomialMap
    valueHep = clone->CentredPolynomialMap(pointHep);
    for(int i=0; i<coeffHep.num_row(); i++) for(int j=0; j<coeffHep.num_col(); j++) testpass &= fabs(coeffHep[i][j] - valueHep(i+1,j+1)) < 1e-5;
    Squeak::mout(Squeak::debug) << valueHep << "CentredPolyMap1: " << testpass << std::endl;
    pointHep(1) = 3; pointHep(2) = -2;
    valueHep = clone->CentredPolynomialMap(pointHep);
    Squeak::mout(Squeak::debug) << valueHep << "CentredPolyMap2: " << testpass << std::endl;
    ///F
    std::vector<double>  valueVec(clone->ValueDimension(), 0);
    clone->F(pointHep,  valueHep);
    clone->F(point,    &valueVec[0]);
    int index = -1;
    for(size_t i=0; i<valueHep.number_of_rows(); i++) for(size_t j=0; j<valueHep.number_of_columns(); j++) testpass &= valueVec[++index] == valueHep(i+1,j+1);
    Squeak::mout(Squeak::debug) << valueHep << std::endl;
    Squeak::mout(Squeak::debug) << "F: " << testpass << std::endl;
    delete clone;
    delete vecP;
    delete vecP2;
    if(!testpass) Squeak::mout(Squeak::debug) << "FAILED DifferentiatorTest" << std::endl;
    return testpass;
}


BTFieldConstructor* field = NULL;
void TrackingFunction(const double* in, double* out)
{
  // maps (x,px)_in to (x,px)_out
  if (field == NULL)
    field = new BTFieldConstructor(new MiceModule( "MICEStage6/Stage6.dat" ));
  double x[8] = {0,in[0],0,-2750,293,in[5],0,0}; // t,x,y,z; E,px,py,pz
  BTTracker::integrate(2750, x, field, BTTracker::z, 10, 1);
  out[0] = x[1];
  out[1] = x[5];
}


//Needs checking!
bool PolynomialInterpolatorTest()
{
    CLHEP::HepMatrix coeffHep(3,6);
    for(unsigned int i=0; i<6; i++) coeffHep[0][i] =  i+1;
    for(unsigned int i=0; i<6; i++) coeffHep[1][i] =  i*i;
    for(unsigned int i=0; i<6; i++) coeffHep[2][i] = -int(i)-1;
    Squeak::mout(Squeak::debug) << coeffHep << std::endl;
    MAUS::PolynomialMap* vecP = new MAUS::PolynomialMap(2, MAUS::Matrix<double>(coeffHep) );
    Function*         vecF = new Function(TrackingFunction, 2, 2);

    int     size   [4] = {9, 9, 12, 16};
    double  min    [4] = {-200, -200, -0.2, -0.1};
    double  spacing[4] = {5, 5, 0., 0.3};
    NDGrid* grid       = new NDGrid(2, size, spacing, min);

    unsigned int diffOrder  = 1;
    unsigned int pointOrder = 0;
    std::vector<double> delta   (4,0.1);
    std::vector<double> mag     (4,1);

    PolynomialInterpolator* pInt  = new PolynomialInterpolator(grid, vecF, diffOrder, pointOrder, delta, mag);
    PolynomialInterpolator* clone = pInt->Clone();
    delete pInt;
    pInt = clone;
    Squeak::mout(Squeak::debug) << "Ctor etc: " << true << " " << std::flush;
    bool sizepass  = pInt->PointDimension()  == vecF->PointDimension() && pInt->ValueDimension()  == vecF->ValueDimension() &&
                     pInt->PolynomialOrder() == diffOrder+pointOrder && pInt->DifferentialOrder() == diffOrder && pInt->PointOrder() == pointOrder;
    sizepass      &= pInt->NumberOfIndices()
                     == MAUS::PolynomialMap::NumberOfPolynomialCoefficients(
                        vecP->PointDimension(), diffOrder+pointOrder);
    sizepass      &= pInt->NumberOfPoints()  == ceil(pInt->NumberOfIndices()/pInt->NumberOfDiffIndices());
    sizepass      &= pInt->NumberOfDiffIndices()
                     == MAUS::PolynomialMap::NumberOfPolynomialCoefficients(
                        vecP->PointDimension(), diffOrder);
    sizepass      &= pInt->GetMesh() == grid;
    Squeak::mout(Squeak::debug) << "Bureaucracy: " << sizepass << " " << std::flush;

    TestF(pInt, 1e-5);

    delete pInt;
    if(!sizepass) Squeak::mout(Squeak::debug) << "FAILED PolynomialInterpolatorTest" << std::endl;
    return sizepass;
}

bool TestF(PolynomialInterpolator* pInt, double tolerance)
{
    bool funcpass = true;
    Mesh*      grid = pInt->GetMesh();
    VectorMap* vecP = pInt->Function();

    std::vector<double> spacing(pInt->PointDimension());
    std::vector<int>    index1 (pInt->PointDimension(), 1);
    std::vector<int>    index2 (pInt->PointDimension(), 1);
    for(unsigned int i=0; i<spacing.size(); i++) {index2[i]++; spacing[i] = Mesh::Iterator(index2, grid).Position()[i] - Mesh::Iterator(index1, grid).Position()[i];}
    double valuePInt[3] = {0,0,0};
    double valueVecP[3] = {0,0,0};

    std::vector<double> point(2,0);
    std::vector<double> point2(2,1);
    std::vector<double> value(3,0);
    Mesh::Iterator it = grid->Nearest(&point[0]); 

    double tol=1e-2;
    for(Mesh::Iterator it = grid->Begin(); it<grid->End(); it++)
    {
        Squeak::mout(Squeak::debug) << std::endl << it << std::endl;
        point = it.Position();
        point[0] -= spacing[0]/8.; point[1] -= spacing[1]/8.;
        vecP->F(&point[0], valueVecP);
        pInt->F(&point[0], valuePInt);
        Squeak::mout(Squeak::debug) << point[0] << " " << point[1] << " ## ";
        for(unsigned int i=0; i<vecP->ValueDimension(); i++) 
        {
          bool pass = fabs(valuePInt[i] -valueVecP[i])/2./fabs(valuePInt[i] +valueVecP[i]) < tol; 
          if(!pass) Squeak::mout(Squeak::debug) << "FAIL " << valuePInt[i] << " " << valueVecP[i] << " " <<  fabs(valuePInt[i] -valueVecP[i])/2./fabs(valuePInt[i] +valueVecP[i]) <<  "\n";
          funcpass &= pass;
        }
        Squeak::mout(Squeak::debug) << std::endl;

        point[0] += spacing[0]/4.;
        vecP->F(&point[0], valueVecP);
        pInt->F(&point[0], valuePInt);
        Squeak::mout(Squeak::debug) << point[0] << " " << point[1] << " ## ";
        for(unsigned int i=0; i<vecP->ValueDimension(); i++) 
        {
          bool pass = fabs(valuePInt[i] -valueVecP[i])/2./fabs(valuePInt[i] +valueVecP[i]) < tol; 
          if(!pass) Squeak::mout(Squeak::debug) << valuePInt[i] << " " << valueVecP[i] << " " <<  fabs(valuePInt[i] -valueVecP[i])/2./fabs(valuePInt[i] +valueVecP[i]) <<  " && ";
          funcpass &= pass;
        }
        Squeak::mout(Squeak::debug) << std::endl;

        point[1] += spacing[1]/4.;
        vecP->F(&point[0], valueVecP);
        pInt->F(&point[0], valuePInt);
        Squeak::mout(Squeak::debug) << point[0] << " " << point[1] << " ## ";
        for(unsigned int i=0; i<vecP->ValueDimension(); i++) 
        {
          bool pass = fabs(valuePInt[i] -valueVecP[i])/2./fabs(valuePInt[i] +valueVecP[i]) < tol; 
          if(!pass) Squeak::mout(Squeak::debug) << valuePInt[i] << " " << valueVecP[i] << " " <<  fabs(valuePInt[i] -valueVecP[i])/2./fabs(valuePInt[i] +valueVecP[i]) <<  " && ";
          funcpass &= pass;
        }
        Squeak::mout(Squeak::debug) << std::endl;

        point[0] -= spacing[0]/4.;
        vecP->F(&point[0], valueVecP);
        pInt->F(&point[0], valuePInt);
        Squeak::mout(Squeak::debug) << point[0] << " " << point[1] << " ## ";
        for(unsigned int i=0; i<vecP->ValueDimension(); i++) 
        {
          bool pass = fabs(valuePInt[i] -valueVecP[i])/2./fabs(valuePInt[i] +valueVecP[i]) < tol; 
          if(!pass) Squeak::mout(Squeak::debug) << valuePInt[i] << " " << valueVecP[i] << " " <<  fabs(valuePInt[i] -valueVecP[i])/2./fabs(valuePInt[i] +valueVecP[i]) <<  " && ";
          funcpass &= pass;
        }

        Squeak::mout(Squeak::debug) << "\n@@ " << funcpass << std::endl;

    }

    return funcpass;
}

TEST(DifferentiatorTest, old_unit_tests) {
    EXPECT_TRUE( DifferentiatorTest());
}


