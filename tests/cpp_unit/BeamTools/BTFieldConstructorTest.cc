// Copyright 2011 Chris Rogers
//
// This file is a part of G4MICE
//
// G4MICE is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// G4MICE is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with G4MICE in the doc folder.  If not, see
// <http://www.gnu.org/licenses/>.

#include "gtest/gtest.h"

#include "Utils/Exception.hh"
#include "Interface/dataCards.hh"

#include "Config/MiceModule.hh"

#include "src/common_cpp/FieldTools/DerivativesSolenoid.hh"
#include "src/common_cpp/FieldTools/SectorMagneticFieldMap.hh"
#include "src/common_cpp/FieldTools/SectorField.hh"

#include "BeamTools/BTCombinedFunction.hh"
#include "BeamTools/BTMultipole.hh"
#include "BeamTools/BTFieldConstructor.hh"

namespace {
class BTFieldConstructorTest : public ::testing::Test {
 protected:
  BTFieldConstructorTest() {
    _field = new BTFieldConstructor();
    for (size_t i=0; i<20; ++i) _mod.push_back(new MiceModule());
  }
  virtual ~BTFieldConstructorTest() {
    for (size_t i=0; i<20; ++i) delete _mod[i];
    delete _field;
  }
  virtual void SetUp() {}
  virtual void TearDown() {}
 
  BTFieldConstructor* _field;
  std::vector<MiceModule*> _mod;
};

TEST_F(BTFieldConstructorTest, EndFieldTest) {
  BTMultipole::EndFieldModel * ef1 = _field->GetEndFieldModel(_mod[0], 1);
  EXPECT_EQ(ef1, static_cast<BTMultipole::EndFieldModel*>(NULL));
  _mod[0]->addPropertyString("EndFieldType", "HardEdged");
  ef1 = _field->GetEndFieldModel(_mod[0], 1);
  EXPECT_EQ(ef1, static_cast<BTMultipole::EndFieldModel*>(NULL));
  _mod[1]->addPropertyString("EndFieldType", "Tanh");
  _mod[1]->addPropertyDouble("EndLength", 1.);
  _mod[1]->addPropertyDouble("CentreLength", 5.);
  _mod[1]->addPropertyInt("MaxEndPole", 3);
  ef1 = _field->GetEndFieldModel(_mod[1], 1);
  BTMultipole::TanhEndField tanh(5., 1., 2);
  for (double x = 0; x < 10; ++x)
    EXPECT_DOUBLE_EQ(tanh.Function(x,0), ef1->Function(x,0));
  delete ef1;

  double enge_a[2] = {1., 0.5};
  _mod[2]->addPropertyString("EndFieldType", "Enge");
  _mod[2]->addPropertyDouble("EndLength", 1.);
  _mod[2]->addPropertyDouble("CentreLength", 5.);
  _mod[2]->addPropertyInt("MaxEndPole", 3);
  _mod[2]->addPropertyDouble("Enge1", enge_a[0]);
  _mod[2]->addPropertyDouble("Enge2", enge_a[1]);
  ef1 = _field->GetEndFieldModel(_mod[2], 1);
  BTMultipole::EngeEndField enge
                             (std::vector<double>(enge_a, enge_a+2), 5., 1., 2);
  for (double x = 0; x < 10; ++x)
    EXPECT_DOUBLE_EQ(enge.Function(x,0), ef1->Function(x,0));
  delete ef1;

  //This is a UI so check for invalid input
  _mod[0]->setProperty<std::string>("EndFieldType","SomeNonsense");
  EXPECT_THROW(delete _field->GetEndFieldModel(_mod[0], 1), MAUS::Exceptions::Exception);
  _mod[0]->setProperty<std::string>("EndFieldType","Tanh");

  // tanh //
  _mod[1]->setProperty<double>("EndLength", 0);
  EXPECT_THROW(delete _field->GetEndFieldModel(_mod[1], 1), MAUS::Exceptions::Exception);
  _mod[1]->setProperty<double>("EndLength", 1);

  _mod[1]->setProperty<double>("CentreLength", 0);
  EXPECT_THROW(delete _field->GetEndFieldModel(_mod[1], 1), MAUS::Exceptions::Exception);
  _mod[1]->setProperty<double>("CentreLength", 5);

  _mod[1]->setProperty<int>("MaxEndPole", 1);
  EXPECT_THROW(delete _field->GetEndFieldModel(_mod[1], 2), MAUS::Exceptions::Exception);
  _mod[1]->setProperty<int>("MaxEndPole", 3);

  // enge //
  _mod[2]->setProperty<double>("EndLength", 0);
  EXPECT_THROW(delete _field->GetEndFieldModel(_mod[2], 1), MAUS::Exceptions::Exception);
  _mod[2]->setProperty<double>("EndLength", 1);

  _mod[2]->setProperty<double>("CentreLength", 0);
  EXPECT_THROW(delete _field->GetEndFieldModel(_mod[2], 1), MAUS::Exceptions::Exception);
  _mod[2]->setProperty<double>("CentreLength", 5);

  _mod[2]->setProperty<int>("MaxEndPole", 1);
  EXPECT_THROW(delete _field->GetEndFieldModel(_mod[2], 2), MAUS::Exceptions::Exception);
  _mod[2]->setProperty<int>("MaxEndPole", 3);
}

TEST_F(BTFieldConstructorTest, GetMultipoleTest) {
  _mod[0]->addPropertyString("EndFieldType", "Tanh");
  _mod[0]->addPropertyDouble("EndLength", 1.);
  _mod[0]->addPropertyDouble("CentreLength", 5.);
  _mod[0]->addPropertyInt("MaxEndPole", 3);
  BTMultipole::EndFieldModel * ef1 = _field->GetEndFieldModel(_mod[0], 1);
  _mod[0]->addPropertyString("FieldType", "Multipole");
  _mod[0]->addPropertyInt("Pole", 1);
  _mod[0]->addPropertyDouble("Height", 2);
  _mod[0]->addPropertyDouble("Width", 3);
  _mod[0]->addPropertyDouble("Length", 4);
  _mod[0]->addPropertyDouble("FieldStrength", 5);
  BTMultipole* mult1 = new BTMultipole();
  mult1->Init(1, 5, 4, 2, 3, "", 0., ef1, 2);
  BTMultipole* mult2 = static_cast<BTMultipole*>(_field->GetMultipole(_mod[0]));
  for (double x = 0; x < 4; ++x)
    for (double z = 0; z < 5; ++z) {
      double pos[4] = {x, 0, z, 0};
      double field1[6] = {0, 0, 0, 0, 0, 0};
      double field2[6] = {0, 0, 0, 0, 0, 0};
      mult1->GetFieldValue(pos, field1);
      mult2->GetFieldValue(pos, field2);
      for (size_t i = 0; i < 3; ++i) {
        EXPECT_DOUBLE_EQ(field1[i], field2[i]);
      }
    }
  delete mult2;
  delete mult1;
  delete ef1;

  //This is a UI so check for invalid input
  _mod[0]->setProperty<int>("Pole", 0);
  EXPECT_THROW(delete _field->GetMultipole(_mod[0]), MAUS::Exceptions::Exception);
  _mod[0]->setProperty<int>("Pole", 1);

  _mod[0]->setProperty<double>("Height", 0);
  EXPECT_THROW(delete _field->GetMultipole(_mod[0]), MAUS::Exceptions::Exception);
  _mod[0]->setProperty<double>("Height", 2);

  _mod[0]->setProperty<double>("Width", 0);
  EXPECT_THROW(delete _field->GetMultipole(_mod[0]), MAUS::Exceptions::Exception);
  _mod[0]->setProperty<double>("Width", 3);

  _mod[0]->setProperty<double>("Length", 0);
  EXPECT_THROW(delete _field->GetMultipole(_mod[0]), MAUS::Exceptions::Exception);
  _mod[0]->setProperty<double>("Length", 4);
}

TEST_F(BTFieldConstructorTest, GetCombinedFunctionTest) {
  _mod[0]->addPropertyString("EndFieldType", "Tanh");
  _mod[0]->addPropertyDouble("EndLength", 1.);
  _mod[0]->addPropertyDouble("CentreLength", 5.);
  _mod[0]->addPropertyInt("MaxEndPole", 3);
  BTMultipole::EndFieldModel * ef1 = _field->GetEndFieldModel(_mod[0], 1);
  _mod[0]->addPropertyString("FieldType", "CombinedFunction");
  _mod[0]->addPropertyInt("Pole", 1);
  _mod[0]->addPropertyDouble("FieldIndex", 1.3);
  _mod[0]->addPropertyDouble("BendingField", 2.3);
  _mod[0]->addPropertyDouble("Height", 2);
  _mod[0]->addPropertyDouble("Width", 3);
  _mod[0]->addPropertyDouble("Length", 4);
  BTCombinedFunction* cf1 = 
                   new BTCombinedFunction(1, 2.3, 1.3, 4, 2, 3, "", 0., ef1, 2);
  BTCombinedFunction* cf2 =
         static_cast<BTCombinedFunction*>(_field->GetCombinedFunction(_mod[0]));
  for (double x = 0; x < 4; ++x)
    for (double z = 0; z < 5; ++z) {
      double pos[4] = {x, 0, z, 0};
      double field1[6] = {0, 0, 0, 0, 0, 0};
      double field2[6] = {0, 0, 0, 0, 0, 0};
      cf1->GetFieldValue(pos, field1);
      cf2->GetFieldValue(pos, field2);
      for (size_t i = 0; i < 3; ++i) {
        EXPECT_DOUBLE_EQ(field1[i], field2[i]);
      }
    }
  delete cf2;
  delete cf1;
  delete ef1;

  //This is a UI so check for invalid input
  _mod[0]->setProperty<int>("Pole", 0);
  EXPECT_THROW(delete _field->GetCombinedFunction(_mod[0]), MAUS::Exceptions::Exception);
  _mod[0]->setProperty<int>("Pole", 1);

  _mod[0]->setProperty<double>("FieldIndex", 0);
  EXPECT_THROW(delete _field->GetCombinedFunction(_mod[0]), MAUS::Exceptions::Exception);
  _mod[0]->setProperty<double>("FieldIndex", 2);

  _mod[0]->setProperty<double>("Height", 0);
  EXPECT_THROW(delete _field->GetCombinedFunction(_mod[0]), MAUS::Exceptions::Exception);
  _mod[0]->setProperty<double>("Height", 3);

  _mod[0]->setProperty<double>("Width", 0);
  EXPECT_THROW(delete _field->GetCombinedFunction(_mod[0]), MAUS::Exceptions::Exception);
  _mod[0]->setProperty<double>("Width", 3);

  _mod[0]->setProperty<double>("Length", 0);
  EXPECT_THROW(delete _field->GetCombinedFunction(_mod[0]), MAUS::Exceptions::Exception);
  _mod[0]->setProperty<double>("Length", 4);
}

TEST_F(BTFieldConstructorTest, GetSectorMagneticFieldMapTest) {
  _mod[0]->addPropertyString("FieldType", "SectorMagneticFieldMap");
  _mod[0]->addPropertyString("FileName",
            "${MAUS_ROOT_DIR}/tests/cpp_unit/FieldTools/test_sector_map.table");
  _mod[0]->addPropertyString("FileType", "tosca_sector_1");
  MAUS::SectorMagneticFieldMap* map = static_cast<MAUS::SectorMagneticFieldMap*>
                                                    (_field->GetField(_mod[0]));
  Interpolator3dGridTo3d* null_interpolator = NULL;
  EXPECT_NE(map->GetInterpolator(), null_interpolator);
  EXPECT_EQ(map->GetSymmetry(), "None");
  double point[4] = {210.+1e-9, 1.-1.e-9, 22.5/180.*M_PI-1.e-9, 0.};
  double field_in[6] = {0., 0., 0., 0., 0., 0.};
  double field_ex[3] = {-0.10002656988, -5.3512047987, -0.42126847878E-01};
  MAUS::SectorField::ConvertToCartesian(point);
  map->GetFieldValue(point, field_in);
  for (int k = 0; k < 3; ++k) {
    EXPECT_NEAR(field_in[k], field_ex[k], 1e-6)
      << "axis " << k << ": " << point[0] << " " << point[1] << " " << point[2];
  }
  delete map;

  _mod[0]->addPropertyString("Symmetry", "Dipole");
  _mod[0]->addPropertyDouble("Unit1", 10.);
  _mod[0]->addPropertyDouble("Unit2", 10.);
  _mod[0]->addPropertyDouble("Unit3", 10.);
  _mod[0]->addPropertyDouble("Unit4", 1.e-4);
  _mod[0]->addPropertyDouble("Unit5", 1.e-4);
  _mod[0]->addPropertyDouble("Unit6", 1.e-4);
  EXPECT_THROW(_field->GetField(_mod[0]), MAUS::Exceptions::Exception);
  MAUS::SectorMagneticFieldMap::ClearFieldCache();
  map = static_cast<MAUS::SectorMagneticFieldMap*>(_field->GetField(_mod[0]));
  for (int i = 0; i < 3; ++i) {
    point[i] *= 10.;
  }
  point[1] *= -1.;
  map->GetFieldValue(point, field_in);
  double field_ex_2[3] = {0.10002656988, -5.3512047987, 0.42126847878E-01};
  for (int k = 0; k < 3; ++k) {
    EXPECT_NEAR(field_in[k], field_ex_2[k]*1e-4, 1e-9)
      << "axis " << k << ": " << point[0] << " " << point[1] << " " << point[2];
  }
  MAUS::SectorMagneticFieldMap::ClearFieldCache();
}

TEST_F(BTFieldConstructorTest, GetDerivativesSolenoidTest) {
  _mod[0]->addPropertyString("FieldType", "DerivativesSolenoid");
  _mod[0]->addPropertyString("EndFieldType", "Tanh");
  _mod[0]->addPropertyDouble("EndLength", 1.);
  _mod[0]->addPropertyDouble("CentreLength", 5.);
  _mod[0]->addPropertyInt("MaxEndPole", 3);
  _mod[0]->addPropertyDouble("PeakField", 2.);
  _mod[0]->addPropertyDouble("ZMax", 5.);
  _mod[0]->addPropertyDouble("RMax", 4.);

  MAUS::DerivativesSolenoid* map = static_cast<MAUS::DerivativesSolenoid*>
                                                    (_field->GetField(_mod[0]));
  EXPECT_EQ(map->GetPeakField(), 2.);
  EXPECT_EQ(map->GetRMax(), 4.);
  EXPECT_EQ(map->GetZMax(), 5.);
  EXPECT_EQ(map->GetHighestOrder(), 3);
  EXPECT_NEAR(map->GetEndFieldModel()->Function(5.0, 0), 0.5, 1e-6);
}

TEST_F(BTFieldConstructorTest, GetSolenoidTest) {
  _mod[0]->addPropertyString("FieldType", "Solenoid");
  _mod[0]->addPropertyString("FileName",
                             "${MAUS_TMP_DIR}/BTFieldConstructorTest1.fld");
  _mod[0]->addPropertyString("FieldMapMode", "Analytic");
  _mod[0]->addPropertyInt("NumberOfZCoords", 10);
  _mod[0]->addPropertyInt("NumberOfRCoords", 10);
  _mod[0]->addPropertyDouble("Length", 2.);
  _mod[0]->addPropertyDouble("Thickness", 3.);
  _mod[0]->addPropertyDouble("InnerRadius", 4.);

  EXPECT_THROW(_field->GetField(_mod[0]), MAUS::Exceptions::Exception);
  _mod[0]->setProperty("FileName",
                             "${MAUS_TMP_DIR}/BTFieldConstructorTest2.fld");
  _mod[0]->addPropertyDouble("Current", 5.);
  _mod[0]->addPropertyInt("NumberOfTurns", 6);
  double point[4] = {0., 0., 0., 0.};
  double field1[6] = {0., 0., 0., 0., 0., 0.};
  BTSolenoid* map1 = static_cast<BTSolenoid*>(_field->GetField(_mod[0]));
  map1->GetAnalyticFieldValue(point, field1);
  _mod[0]->setProperty("FileName",
                             "${MAUS_TMP_DIR}/BTFieldConstructorTest3.fld");
  _mod[0]->addPropertyDouble("CurrentDensity", 5.*6./3./2.); // A/mm^2
  double field2[6] = {0., 0., 0., 0., 0., 0.};
  BTSolenoid* map2 = static_cast<BTSolenoid*>(_field->GetField(_mod[0]));
  map2->GetAnalyticFieldValue(point, field2);
  for (size_t i = 0; i < 6; ++i) {
      EXPECT_NEAR(field1[i], field2[i], 1e-12);
  }
}
}

