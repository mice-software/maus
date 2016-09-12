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

//! Testing getListOfProperties<T> in MiceModules
#include <limits>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <fstream>

#include "gtest/gtest.h" 

#include "Config/MiceModule.hh"

namespace {

// TODO(Rogers):
// * would be better to pass micemodules a string stream (istream ref) - current
//   implementation is dangerous

/////////////// MiceModule Tests ///////////////
//Fixture - allows to set up some default sets of data
//for testing against
class MiceModuleTest : public ::testing::Test {
protected:
  MiceModuleTest() {SetUp();}
  virtual ~MiceModuleTest() { delete moduleTest;}
  static void SetUpTestCase();
  virtual void  SetUp();
  virtual void TearDown() {}

  MiceModule* moduleTest;
  static std::string test_module_path;
  static std::string test_module_name;
};

std::string MiceModuleTest::test_module_path = "";
std::string MiceModuleTest::test_module_name = "";

TEST_F(MiceModuleTest, ReadModuleTest) {//name test...
  EXPECT_EQ(moduleTest->fullName(), test_module_name);
  EXPECT_THROW(MiceModule(""), MAUS::Exceptions::Exception);
}

TEST_F(MiceModuleTest, GetBoolTest) {//bool test
  std::map<std::string, bool> t = moduleTest->getListOfProperties<bool>();
  EXPECT_EQ(t.size(),(unsigned)1);
  EXPECT_EQ(t["Invisible"], true);
  EXPECT_EQ(moduleTest->propertyBool("Invisible"), true);
  EXPECT_THROW(moduleTest->propertyBool("NOTEXIST_Invisible"), MAUS::Exceptions::Exception);
}

TEST_F(MiceModuleTest, GetIntTest) {//int test
  std::map<std::string, int> t = moduleTest->getListOfProperties<int>();
  EXPECT_EQ(t.size(),(unsigned)1);
  EXPECT_EQ(t["Copies"],1);
}

TEST_F(MiceModuleTest, GetStringTest) {//string test
  std::map<std::string, std::string> t = moduleTest->getListOfProperties<std::string>();
  EXPECT_EQ(t.size(),(unsigned)2);
  EXPECT_EQ(t["Material"], "AIR");
  EXPECT_EQ(t["Volume"], "Box");
  EXPECT_EQ(moduleTest->propertyString("Material"), "AIR");
  EXPECT_EQ(moduleTest->propertyString("Volume"), "Box");
  EXPECT_THROW(moduleTest->propertyString("NONEXIST_Material"), MAUS::Exceptions::Exception);
}

TEST_F(MiceModuleTest, GetDoubleTest) {//double test
  std::map<std::string, double> t = moduleTest->getListOfProperties<double>();
  EXPECT_EQ(t.size(),(unsigned)2);
  EXPECT_EQ(t["GreenColour"], 0.5); //These are all represented exactly
  EXPECT_EQ(t["RedColour"], 1.0);
}

TEST_F(MiceModuleTest, Get3VecTest) {//Hep3Vec test
  std::map<std::string, Hep3Vector> t = moduleTest->getListOfProperties<Hep3Vector>();
  EXPECT_EQ(t.size(),(unsigned)3);
  EXPECT_EQ(t["Dimensions"], Hep3Vector(390,2600,1000));
  EXPECT_EQ(t["Position"], Hep3Vector(100,200,300));
  EXPECT_EQ(t["Rotation"].x(),0.);
  EXPECT_EQ(t["Rotation"].y(),0.);
  EXPECT_LT(fabs(t["Rotation"].z()-1.5707963267948966),1.e-8);
  EXPECT_EQ(moduleTest->propertyHep3Vector("Dimensions"), Hep3Vector(390,2600,1000));
  EXPECT_EQ(moduleTest->propertyHep3Vector("Position"), Hep3Vector(100,200,300));
  EXPECT_THROW(moduleTest->propertyHep3Vector("NONEXIST_Position"), MAUS::Exceptions::Exception);
}

TEST_F(MiceModuleTest, propertyDoubleAccessors) { //AddPropertyDouble(string, string), AddPropertyDouble(string, double), propertyDouble(string), propertyDoubleThis(string)
  MiceModule mod;
  mod.addPropertyDouble("MyProp", "3. m");
  EXPECT_EQ   (mod.propertyDouble("MyProp"), 3000.);
  mod.setProperty("MyProp", 1000.);
  EXPECT_EQ   (mod.propertyDouble("MyProp"), 1000.);
  mod.addPropertyDouble("MyProp2", "3000.");
  EXPECT_EQ   (mod.propertyDouble("MyProp2"), 3000.);
  mod.addPropertyDouble("MyProp3", 3000.);
  EXPECT_EQ   (mod.propertyDouble("MyProp3"), 3000.);
  EXPECT_EQ   (mod.propertyDoubleThis("MyProp3"), 3000.);
  mod.addPropertyDouble("MyProp4", "3000. fish");
  EXPECT_THROW(mod.propertyDouble("MyProp4"), MAUS::Exceptions::Exception);
  EXPECT_THROW(mod.addPropertyDouble("MyProp4", "3000."), MAUS::Exceptions::Exception);
}

TEST_F(MiceModuleTest, propertyHep3VecAccessors) { //AddPropertyHep3Vector(string, string), AddPropertyH3V(string, H3V), propertyh3v(string), propertyH3VThis(string)
  MiceModule mod;
  mod.addPropertyHep3Vector("MyProp", "1. 2. 3. m");
  EXPECT_EQ   (mod.propertyHep3Vector("MyProp"), CLHEP::Hep3Vector(1000., 2000., 3000.) );
  mod.setProperty("MyProp", CLHEP::Hep3Vector(4000., 5000., 6000.));
  EXPECT_EQ   (mod.propertyHep3Vector("MyProp"), CLHEP::Hep3Vector(4000., 5000., 6000.));
  mod.addPropertyHep3Vector("MyProp2", "1000. 2000. 3000.");
  EXPECT_EQ   (mod.propertyHep3Vector("MyProp2"), CLHEP::Hep3Vector(1000., 2000., 3000.) );
  mod.addPropertyHep3Vector("MyProp3", CLHEP::Hep3Vector(1000., 2000., 3000.) );
  EXPECT_EQ   (mod.propertyHep3Vector("MyProp3"), CLHEP::Hep3Vector(1000., 2000., 3000.) );
  EXPECT_EQ   (mod.propertyHep3VectorThis("MyProp3"), CLHEP::Hep3Vector(1000., 2000., 3000.));
  mod.addPropertyHep3Vector("MyProp4", "1000. 2000.");
  EXPECT_THROW(mod.propertyHep3Vector("MyProp4"), MAUS::Exceptions::Exception);
  mod.addPropertyHep3Vector("MyProp5", "1000. 2000. 3000. fish");
  EXPECT_THROW(mod.propertyHep3Vector("MyProp5"), MAUS::Exceptions::Exception);
  EXPECT_THROW(mod.addPropertyHep3Vector("MyProp2", "1000. 2000. 3000."), MAUS::Exceptions::Exception);
}

TEST_F(MiceModuleTest, printTree) {  // printTree doesn't print root module data
  std::ostringstream OutGDML("");
  moduleTest->printTree( 0, OutGDML );
  std::string MMPrint("Configuration "+test_module_name+"\n"); 
  EXPECT_EQ(MMPrint.compare(OutGDML.str()), 0);
}

TEST_F(MiceModuleTest, DumpMiceModule) {
  std::ostringstream OutMM("");
  std::ostringstream MMPrint(""); 
  std::string Indent("");
  moduleTest->DumpMiceModule(Indent, OutMM);
  MMPrint << "Configuration " << test_module_name << std::endl << "{" << std::endl
    << " Dimensions 390 2600 1000" << std::endl
    << " PropertyDouble GreenColour 0.5" << std::endl
    << " PropertyDouble RedColour 1.0" << std::endl
    << " PropertyBool Invisible 1" << std::endl
    << " PropertyInt Copies 1" << std::endl
    << " PropertyString Material AIR" << std::endl << "}" << std::endl;
  EXPECT_EQ(MMPrint.str().compare(OutMM.str()), 0);
}

//Make a MiceModule
//nb: this fails if we dont have write access to the disk, filename already
//    exists as a directory, ... - basically flaky. Better to pass an istream&
void MiceModuleTest::SetUpTestCase() {
  std::stringstream mice_mod_test;
  mice_mod_test << "// ModuleTest.cards\n"
                << "//\n"
                << "// A test module for MiceModule\n"
                << "//\n"
                << "// \n"
                << "// \n"
                << "\n"
                << "Module TestModule\n"
                << "{\n"
                << "  Dimensions 39.0 260.0  100.0 cm\n"
                << "  Position 10.0 20.0 30.0 cm\n"
                << "  Rotation 0.0 0.0 90.0 degree\n"
                << "  PropertyString Material AIR\n"
                << "  PropertyBool Invisible 1\n"
                << "  PropertyInt Copies 1\n"
                << "  PropertyDouble GreenColour 0.5\n"
                << "  PropertyDouble RedColour 1.0\n"
                << "}\n";

  ASSERT_TRUE(getenv( "MAUS_ROOT_DIR" ) != NULL)
                                   << "MAUS_ROOT_DIR not set - test failed";
  test_module_path  = getenv( "MAUS_ROOT_DIR" );
  test_module_path += "/tmp/";
  test_module_name  = "mice_module_test.dat";

  std::ofstream fout((test_module_path+test_module_name).c_str());
  fout << mice_mod_test.str();
  fout.close();
}

void MiceModuleTest::SetUp() {
  moduleTest = new MiceModule(test_module_path+test_module_name);
}
} //namespace



