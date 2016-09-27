//This file is a part of G4MICE
//
//G4MICE is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//G4MICE is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with G4MICE in the doc folder.  If not, see 
//<http://www.gnu.org/licenses/>.

#include "gtest/gtest.h"

#include "Geant4/G4Box.hh"
#include "Geant4/G4Tubs.hh"
#include "Geant4/G4Trd.hh"
#include "Geant4/G4Sphere.hh"
#include "Geant4/G4Polycone.hh"
#include "Geant4/G4Torus.hh"
#include "Geant4/G4EllipticalCone.hh"
#include "Geant4/G4UnionSolid.hh"
#include "Geant4/G4SubtractionSolid.hh"
#include "Geant4/G4IntersectionSolid.hh"
#include "Geant4/G4VSolid.hh"
#include "Geant4/G4AffineTransform.hh"

#include "EngModel/MiceModToG4Solid.hh"
#include "Utils/Exception.hh"


//MiceModToG4SolidTest 
//Check that we can build each volume and try to check that we can't get any G4 exceptions
//i.e. that we catch all exceptions in G4MICE before G4 can get to them, and hopefully
//produce more polite error messages (that, for example, list the module name that saw the
//error.)
//
//TODO: boolean logical volume is not tested - needs external files; I think I would prefer to
//      rework the way boolean logical volume works... need to think about it
//     

namespace {

class MiceModToG4SolidTest : public ::testing::Test {
protected:
  MiceModToG4SolidTest         () {}
  virtual ~MiceModToG4SolidTest() {}

  virtual void SetUp   () {}
  virtual void TearDown() {}
};

TEST_F(MiceModToG4SolidTest, checkDimTest) {
  MiceModule mod;
  mod.setProperty<CLHEP::Hep3Vector>("Dimensions", CLHEP::Hep3Vector(1.,1.,0.) );
  ASSERT_THROW   ( MiceModToG4Solid::checkDim(&mod, 3), MAUS::Exceptions::Exception );
  ASSERT_NO_THROW( MiceModToG4Solid::checkDim(&mod, 2) );
}

TEST_F(MiceModToG4SolidTest, buildWedgeTest) {
  MiceModule mod;
  mod.setProperty<CLHEP::Hep3Vector>("Dimensions",CLHEP::Hep3Vector(1.,2.,3.) );
  G4VSolid* sol=NULL;
  ASSERT_NO_THROW( sol = MiceModToG4Solid::buildWedge(&mod) );
  ASSERT_FALSE(sol==NULL);
  EXPECT_DOUBLE_EQ(((G4Trd*)sol)->GetXHalfLength1(), 0.5); //half length of wedge width at bottom
  EXPECT_DOUBLE_EQ(((G4Trd*)sol)->GetXHalfLength2(), 0.5); //half length of wedge width at top
  EXPECT_DOUBLE_EQ(((G4Trd*)sol)->GetYHalfLength1(), 2.0);
  EXPECT_NEAR     (((G4Trd*)sol)->GetYHalfLength2(), 0.0, 1e-9);
  EXPECT_DOUBLE_EQ(((G4Trd*)sol)->GetZHalfLength (), 1.5);
  if (sol!=NULL) delete sol; sol=NULL;
  mod.setProperty<CLHEP::Hep3Vector>("Dimensions",CLHEP::Hep3Vector(0., 1., 1.) );
  ASSERT_THROW   ( sol = MiceModToG4Solid::buildWedge(&mod), MAUS::Exceptions::Exception );
  mod.setProperty<CLHEP::Hep3Vector>("Dimensions",CLHEP::Hep3Vector(1., 0., 1.) );
  ASSERT_THROW   ( sol = MiceModToG4Solid::buildWedge(&mod), MAUS::Exceptions::Exception );
  mod.setProperty<CLHEP::Hep3Vector>("Dimensions",CLHEP::Hep3Vector(1., 1., 0.) );
  ASSERT_THROW   ( sol = MiceModToG4Solid::buildWedge(&mod), MAUS::Exceptions::Exception );
}

MiceModule trapezoid_module( double aX1, double aX2, double aY1, double aY2, double aZ ) {
  MiceModule mod;
  try{
	  mod.setProperty<CLHEP::Hep3Vector>("Dimensions",CLHEP::Hep3Vector(10.,20.,30.) );
  	  mod.addPropertyDouble( "TrapezoidWidthX1", aX1 );
  	  mod.addPropertyDouble( "TrapezoidWidthX2", aX2 );
  	  mod.addPropertyDouble( "TrapezoidHeightY1", aY1 );
  	  mod.addPropertyDouble( "TrapezoidHeightY2", aY2 );
  	  mod.addPropertyDouble( "TrapezoidLengthZ", aZ );
  	  return mod;
  } catch( MAUS::Exceptions::Exception exc ){
	  std::string error = exc.GetMessage();
	  std::cerr << "Exception in trapezoid_module: " << error << std::endl;
	  throw( MAUS::Exceptions::Exception( MAUS::Exceptions::recoverable, "Error in MiceModToG4Test: " + error + "\'", "trapezoid_module"));
  }
}

TEST_F(MiceModToG4SolidTest, buildTrapezoidTest) {
//
  double x1 = 1.0, x2 = 2.0, y1 = 3.0, y2 = 4.0, z = 5.0;
  MiceModule mod = trapezoid_module( x1, x2, y1, y2, z );
  G4VSolid* sol=NULL;

  ASSERT_NO_THROW( sol = MiceModToG4Solid::buildTrapezoid( &mod ) );
  ASSERT_FALSE(sol==NULL);
  EXPECT_DOUBLE_EQ(((G4Trd*)sol)->GetXHalfLength1(), x1 );
  EXPECT_DOUBLE_EQ(((G4Trd*)sol)->GetXHalfLength2(), x2 );
  EXPECT_DOUBLE_EQ(((G4Trd*)sol)->GetYHalfLength1(), y1 );
  EXPECT_DOUBLE_EQ(((G4Trd*)sol)->GetYHalfLength2(), y2 );
  EXPECT_DOUBLE_EQ(((G4Trd*)sol)->GetZHalfLength (), z );
  if (sol!=NULL) delete sol; sol=NULL;
}


TEST_F(MiceModToG4SolidTest, buildBoxTest) {
  MiceModule mod;
  mod.setProperty<CLHEP::Hep3Vector>("Dimensions",CLHEP::Hep3Vector(1.,2.,3.) );
  G4VSolid* sol=NULL;
  ASSERT_NO_THROW( sol = MiceModToG4Solid::buildBox(&mod) );
  ASSERT_FALSE(sol==NULL);
  EXPECT_DOUBLE_EQ(((G4Box*)sol)->GetXHalfLength(), 0.5);
  EXPECT_DOUBLE_EQ(((G4Box*)sol)->GetYHalfLength(), 1.0);
  EXPECT_DOUBLE_EQ(((G4Box*)sol)->GetZHalfLength(), 1.5);
  if (sol!=NULL) delete sol; sol=NULL;
  mod.setProperty<CLHEP::Hep3Vector>("Dimensions",CLHEP::Hep3Vector(0., 1., 1.) );
  ASSERT_THROW   ( sol = MiceModToG4Solid::buildBox(&mod), MAUS::Exceptions::Exception );
  mod.setProperty<CLHEP::Hep3Vector>("Dimensions",CLHEP::Hep3Vector(1., 0., 1.) );
  ASSERT_THROW   ( sol = MiceModToG4Solid::buildBox(&mod), MAUS::Exceptions::Exception );
  mod.setProperty<CLHEP::Hep3Vector>("Dimensions",CLHEP::Hep3Vector(1., 1., 0.) );
  ASSERT_THROW   ( sol = MiceModToG4Solid::buildBox(&mod), MAUS::Exceptions::Exception );
}

TEST_F(MiceModToG4SolidTest, buildCylinderTest) {
  MiceModule mod;
  mod.setProperty<CLHEP::Hep3Vector>("Dimensions",CLHEP::Hep3Vector(1.,2.,-1.) );
  G4VSolid* sol=NULL;
  ASSERT_NO_THROW( sol = MiceModToG4Solid::buildCylinder(&mod) );
  ASSERT_FALSE(sol==NULL);
  EXPECT_NEAR     (((G4Tubs*)sol)->GetInnerRadius(), 0.0, 1e-9);
  EXPECT_DOUBLE_EQ(((G4Tubs*)sol)->GetOuterRadius(), 1.0);
  EXPECT_DOUBLE_EQ(((G4Tubs*)sol)->GetZHalfLength(), 1.0);
  if (sol!=NULL) delete sol; sol=NULL;
  mod.setProperty<CLHEP::Hep3Vector>("Dimensions",CLHEP::Hep3Vector(0., 1., 1.) );
  ASSERT_THROW   ( sol = MiceModToG4Solid::buildCylinder(&mod), MAUS::Exceptions::Exception );
  mod.setProperty<CLHEP::Hep3Vector>("Dimensions",CLHEP::Hep3Vector(1., 0., 1.) );
  ASSERT_THROW   ( sol = MiceModToG4Solid::buildCylinder(&mod), MAUS::Exceptions::Exception );
}

TEST_F(MiceModToG4SolidTest, buildTubeTest) {
  MiceModule mod;
  mod.setProperty<CLHEP::Hep3Vector>("Dimensions",CLHEP::Hep3Vector(1.,2.,3.) );
  G4VSolid* sol=NULL;
  ASSERT_NO_THROW( sol = MiceModToG4Solid::buildTube(&mod) );
  ASSERT_FALSE(sol==NULL);
  EXPECT_DOUBLE_EQ(((G4Tubs*)sol)->GetInnerRadius(), 1.0);
  EXPECT_DOUBLE_EQ(((G4Tubs*)sol)->GetOuterRadius(), 2.0);
  EXPECT_DOUBLE_EQ(((G4Tubs*)sol)->GetZHalfLength(), 1.5);
  if (sol!=NULL) delete sol; sol=NULL;
  //Check we catch errors before G4 throws
  mod.setProperty<CLHEP::Hep3Vector>("Dimensions",CLHEP::Hep3Vector(2., 1., 1.) );
  ASSERT_THROW   ( sol = MiceModToG4Solid::buildTube(&mod), MAUS::Exceptions::Exception );
  mod.setProperty<CLHEP::Hep3Vector>("Dimensions",CLHEP::Hep3Vector(0., 1., 1.) );
  ASSERT_THROW   ( sol = MiceModToG4Solid::buildTube(&mod), MAUS::Exceptions::Exception );
  mod.setProperty<CLHEP::Hep3Vector>("Dimensions",CLHEP::Hep3Vector(1., 0., 1.) );
  ASSERT_THROW   ( sol = MiceModToG4Solid::buildTube(&mod), MAUS::Exceptions::Exception );
  mod.setProperty<CLHEP::Hep3Vector>("Dimensions",CLHEP::Hep3Vector(1., 1., 0.) );
  ASSERT_THROW   ( sol = MiceModToG4Solid::buildTube(&mod), MAUS::Exceptions::Exception );
}

TEST_F(MiceModToG4SolidTest, buildSphereTest) {
  MiceModule mod;
  mod.setProperty<CLHEP::Hep3Vector>("Dimensions",CLHEP::Hep3Vector(1.,2.,-1.) );
  G4VSolid* sol=NULL;
  ASSERT_NO_THROW( sol = MiceModToG4Solid::buildSphere(&mod) );
  ASSERT_FALSE(sol==NULL);
  EXPECT_DOUBLE_EQ(((G4Sphere*)sol)->GetInnerRadius(), 1.0);
  EXPECT_DOUBLE_EQ(((G4Sphere*)sol)->GetOuterRadius(), 2.0);
  EXPECT_DOUBLE_EQ(((G4Sphere*)sol)->GetStartPhiAngle(),   0.0);
  EXPECT_DOUBLE_EQ(((G4Sphere*)sol)->GetDeltaPhiAngle(),   360.0*deg);
  EXPECT_DOUBLE_EQ(((G4Sphere*)sol)->GetStartThetaAngle(), 0.0);
  EXPECT_DOUBLE_EQ(((G4Sphere*)sol)->GetDeltaThetaAngle(), 180.0*deg);
  delete sol; sol=NULL;

  mod.addPropertyHep3Vector("Phi",   CLHEP::Hep3Vector(5.*deg, 10.*deg,-1.)); //radians < 2pi
  mod.addPropertyHep3Vector("Theta", CLHEP::Hep3Vector(15.*deg,20.*deg,-1.)); //radians < pi
  ASSERT_NO_THROW( sol = MiceModToG4Solid::buildSphere(&mod) );
  ASSERT_FALSE(sol==NULL);
  EXPECT_DOUBLE_EQ(((G4Sphere*)sol)->GetInnerRadius(), 1.0);
  EXPECT_DOUBLE_EQ(((G4Sphere*)sol)->GetOuterRadius(), 2.0);
  EXPECT_DOUBLE_EQ(((G4Sphere*)sol)->GetStartPhiAngle(),   5.*deg);
  EXPECT_DOUBLE_EQ(((G4Sphere*)sol)->GetDeltaPhiAngle(),   10.*deg);
  EXPECT_DOUBLE_EQ(((G4Sphere*)sol)->GetStartThetaAngle(), 15.*deg);
  EXPECT_DOUBLE_EQ(((G4Sphere*)sol)->GetDeltaThetaAngle(), 20.0*deg);
  delete sol; sol=NULL;

  //Check we catch errors before G4 throws
  mod.setProperty<CLHEP::Hep3Vector>("Dimensions",CLHEP::Hep3Vector(2.,1.,-1.) );
  ASSERT_THROW( sol = MiceModToG4Solid::buildSphere(&mod), MAUS::Exceptions::Exception );
  mod.setProperty<CLHEP::Hep3Vector>("Dimensions",CLHEP::Hep3Vector(1.,-1.,-1.) );
  ASSERT_THROW( sol = MiceModToG4Solid::buildSphere(&mod), MAUS::Exceptions::Exception );
  mod.setProperty<CLHEP::Hep3Vector>("Dimensions",CLHEP::Hep3Vector(-1.,1.,-1.) );
  ASSERT_THROW( sol = MiceModToG4Solid::buildSphere(&mod), MAUS::Exceptions::Exception );
  mod.setProperty<CLHEP::Hep3Vector>("Dimensions",CLHEP::Hep3Vector(1.,2.,-1.) );
  //G4 throws if delta angle is negative, otherwise seems ok - but I want to force users to make valid input
  mod.addPropertyHep3Vector("Phi",   CLHEP::Hep3Vector(100.*deg,  -10.*deg,-1.));
  ASSERT_THROW( sol = MiceModToG4Solid::buildSphere(&mod), MAUS::Exceptions::Exception );
  mod.addPropertyHep3Vector("Phi",   CLHEP::Hep3Vector(500.*deg,  10.*deg,-1.));
  ASSERT_THROW( sol = MiceModToG4Solid::buildSphere(&mod), MAUS::Exceptions::Exception );
  mod.addPropertyHep3Vector("Phi",   CLHEP::Hep3Vector(5.*deg,   500.*deg,-1.));
  ASSERT_THROW( sol = MiceModToG4Solid::buildSphere(&mod), MAUS::Exceptions::Exception );
  mod.addPropertyHep3Vector("Phi",   CLHEP::Hep3Vector(5.*deg,    10.*deg,-1.));
  mod.addPropertyHep3Vector("Theta", CLHEP::Hep3Vector(100.*deg,  -10.*deg,-1.));
  ASSERT_THROW( sol = MiceModToG4Solid::buildSphere(&mod), MAUS::Exceptions::Exception );
  mod.addPropertyHep3Vector("Theta", CLHEP::Hep3Vector(500.*deg,  10.*deg,-1.));
  ASSERT_THROW( sol = MiceModToG4Solid::buildSphere(&mod), MAUS::Exceptions::Exception );
  mod.addPropertyHep3Vector("Theta", CLHEP::Hep3Vector(5.*deg,   500.*deg,-1.));
  ASSERT_THROW( sol = MiceModToG4Solid::buildSphere(&mod), MAUS::Exceptions::Exception );

}

TEST_F(MiceModToG4SolidTest, buildPolyconeTest) {
  MiceModule mod;
  ASSERT_THROW(MiceModToG4Solid::buildPolycone(&mod), MAUS::Exceptions::Exception); //test properly in polycone.hh
}

TEST_F(MiceModToG4SolidTest, buildMultipoleTest) {
  MiceModule mod;
  ASSERT_THROW(MiceModToG4Solid::buildMultipole(&mod), MAUS::Exceptions::Exception); //test properly in multipole.hh
}



MiceModule torus_module(double in_r, double out_r, double q_s, double q_e, double r_curv) {
  MiceModule mod;
  mod.addPropertyDouble("TorusInnerRadius",  in_r);
  mod.addPropertyDouble("TorusOuterRadius",  out_r);
  mod.addPropertyDouble("TorusAngleStart",   q_s);
  mod.addPropertyDouble("TorusOpeningAngle", q_e);
  mod.addPropertyDouble("TorusRadiusOfCurvature", r_curv);
  return mod;
}

TEST_F(MiceModToG4SolidTest, buildTorusTest) {
  MiceModule mod = torus_module(0.5, 1.0, 5.*deg, 10.*deg, 2.0);
  G4Torus* sol=NULL;
  ASSERT_NO_THROW(sol = (G4Torus*)MiceModToG4Solid::buildTorus(&mod));
  ASSERT_FALSE(sol==NULL);
  EXPECT_DOUBLE_EQ(sol->GetRmin(), 0.5 );
  EXPECT_DOUBLE_EQ(sol->GetRmax(), 1.0 );
  EXPECT_DOUBLE_EQ(sol->GetRtor(), 2.0 );
  EXPECT_DOUBLE_EQ(sol->GetSPhi(), 5.*deg );
  EXPECT_DOUBLE_EQ(sol->GetDPhi(), 10.*deg );
  MiceModule mod1 = torus_module(1.5, 1.0, 5.*deg, 10.*deg, 2.0);
  ASSERT_THROW(sol = (G4Torus*)MiceModToG4Solid::buildTorus(&mod1), MAUS::Exceptions::Exception);
  MiceModule mod2 = torus_module(-0.1, 1.0, 5.*deg, 10.*deg, 2.0);
  ASSERT_THROW(sol = (G4Torus*)MiceModToG4Solid::buildTorus(&mod2), MAUS::Exceptions::Exception);
  MiceModule mod3 = torus_module(0.5, 3.0, 5.*deg, 10.*deg, 2.0);
  ASSERT_THROW(sol = (G4Torus*)MiceModToG4Solid::buildTorus(&mod3), MAUS::Exceptions::Exception);
  MiceModule mod4 = torus_module(0.5, 0.25, 5.*deg, 10.*deg, 2.0);
  ASSERT_THROW(sol = (G4Torus*)MiceModToG4Solid::buildTorus(&mod4), MAUS::Exceptions::Exception);
  MiceModule mod5 = torus_module(0.5, 1.0, -1.*deg, 10.*deg, 2.0);
  ASSERT_THROW(sol = (G4Torus*)MiceModToG4Solid::buildTorus(&mod5), MAUS::Exceptions::Exception);
  MiceModule mod6 = torus_module(0.5, 1.0, 361.*deg, 10.*deg, 2.0);
  ASSERT_THROW(sol = (G4Torus*)MiceModToG4Solid::buildTorus(&mod6), MAUS::Exceptions::Exception);
  MiceModule mod7 = torus_module(0.5, 1.0, 5.*deg, -1.*deg, 2.0);
  ASSERT_THROW(sol = (G4Torus*)MiceModToG4Solid::buildTorus(&mod7), MAUS::Exceptions::Exception);
  MiceModule mod8 = torus_module(0.5, 1.0, 5.*deg, 361.*deg, 2.0);
  ASSERT_THROW(sol = (G4Torus*)MiceModToG4Solid::buildTorus(&mod8), MAUS::Exceptions::Exception);
}

TEST_F(MiceModToG4SolidTest, buildEllipticalConeTest) {
  MiceModule mod;
  mod.addPropertyDouble("ZCut", 1.0) ;
  mod.setProperty<CLHEP::Hep3Vector>("Dimensions",CLHEP::Hep3Vector(2.0, 3.0, 4.0));
  G4EllipticalCone* sol=NULL;
  ASSERT_NO_THROW(sol = (G4EllipticalCone*)MiceModToG4Solid::buildEllipticalCone(&mod));
  ASSERT_FALSE(sol==NULL);
  EXPECT_DOUBLE_EQ(sol->GetZTopCut(), 1.0);
  EXPECT_DOUBLE_EQ(sol->GetSemiAxisMax(), 3.0/4.0);
  MiceModule mod2;
  mod2.addPropertyDouble("ZCut", -1.0) ;
  mod2.setProperty<CLHEP::Hep3Vector>("Dimensions",CLHEP::Hep3Vector(2.0, 3.0, 4.0));
  ASSERT_THROW(sol = (G4EllipticalCone*)MiceModToG4Solid::buildEllipticalCone(&mod2), MAUS::Exceptions::Exception);
  MiceModule mod3;
  mod3.setProperty<CLHEP::Hep3Vector>("Dimensions",CLHEP::Hep3Vector(-1.0, 3.0, 4.0));
  ASSERT_THROW(sol = (G4EllipticalCone*)MiceModToG4Solid::buildEllipticalCone(&mod2), MAUS::Exceptions::Exception);
  mod3.setProperty<CLHEP::Hep3Vector>("Dimensions",CLHEP::Hep3Vector(1.0, -3.0, 4.0));
  ASSERT_THROW(sol = (G4EllipticalCone*)MiceModToG4Solid::buildEllipticalCone(&mod2), MAUS::Exceptions::Exception);
  mod3.setProperty<CLHEP::Hep3Vector>("Dimensions",CLHEP::Hep3Vector(1.0, 3.0, -4.0));
  ASSERT_THROW(sol = (G4EllipticalCone*)MiceModToG4Solid::buildEllipticalCone(&mod2), MAUS::Exceptions::Exception);
}


};


