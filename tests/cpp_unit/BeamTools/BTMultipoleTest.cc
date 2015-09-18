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

#include <sstream>

#include "gsl/gsl_sf_gamma.h"
#include "gsl/gsl_sf_pow_int.h"

#include "gtest/gtest.h"

#include "Interface/Differentiator.hh"
#include "Interface/MathUtils.hh"
#include "Utils/Exception.hh"
#include "Maths/Matrix.hh"
#include "Maths/Vector.hh"

#include "BeamTools/BTMultipole.hh"

namespace {
// TODO(rogers): Add test for sextupole
//               Add tests for rotated multipoles

/////////////// BTMultipole Tests ///////////////
class BTMultipoleTest : public ::testing::Test {
 protected:
  BTMultipoleTest() : _tanh(0., 0., 0),
                      _enge(std::vector<double>(), 0., 0., 0) {
    double _enge_a[4] = {0., 1.1, 2., 3.};
    _enge_v = std::vector<double>(_enge_a, _enge_a+4);
    _qm     = 1;
    _field  = 5.;
    _length = 3.;
    _height = 0.4;
    _width  = 0.3;
    _enge_v = std::vector<double>(_enge_a, _enge_a+4);
    _enge_x = 1.5;
    _enge_l = 0.2;
    _tanh_x = 1.7;
    _tanh_l = 0.21;
    _tanh = BTMultipole::TanhEndField(_tanh_x, _tanh_l, _qm);
    _enge = BTMultipole::EngeEndField(_enge_v, _enge_x, _enge_l, _qm);
    _hard_edged_straight_dip  = new BTMultipole();
    _hard_edged_straight_dip->Init(1, _field, _length, _height,
                                   _width, "", 0., NULL,   _qm);
    _enge_end_straight_dip    = new BTMultipole();
    _enge_end_straight_dip->Init(1, _field, _length, _height,
                                            _width, "", 0., &_enge, _qm);
    _tanh_end_straight_dip    = new BTMultipole();
    _tanh_end_straight_dip->Init(1, _field, _length, _height,
                                                _width, "", 0., &_tanh, _qm);
    _tanh_end_straight_quad   = new BTMultipole();
    _tanh_end_straight_quad->Init(2, _field, _length, _height,
                                              _width, "", 0., &_tanh, _qm);
    _mult[0] = _hard_edged_straight_dip;
    _mult[1] = _enge_end_straight_dip;
    _mult[2] = _tanh_end_straight_dip;
    Tanh::SetTanhDiffIndices(_qm+5);
  }

  virtual ~BTMultipoleTest() {
    delete _hard_edged_straight_dip;
    delete _enge_end_straight_dip;
    delete _tanh_end_straight_dip;
    delete _tanh_end_straight_quad;
  }
  virtual void SetUp() {}
  virtual void TearDown() {}

  int    _qm;
  std::vector<double> _enge_v;
  double _enge_x, _enge_l, _tanh_x, _tanh_l;
  double _length, _height, _width, _field;

  BTMultipole::TanhEndField _tanh;
  BTMultipole::EngeEndField _enge;
  BTMultipole* _hard_edged_straight_dip;
  BTMultipole* _enge_end_straight_dip;
  BTMultipole* _tanh_end_straight_dip;
  BTMultipole* _tanh_end_straight_quad;
  BTMultipole* _mult[3];
};

TEST_F(BTMultipoleTest, BTMultipoleEndFieldTest) {
  // Main tests for end fields are done elsewhere, here we make sanity checks to
  // test interface is correct
  std::stringstream out1, out2;
  Enge::SetEngeDiffIndices(3);
  Enge enge(_enge_v, _enge_x, _enge_l, _qm);
  EXPECT_EQ(enge.GetDoubleEnge(0.5, 2), _enge.Function(0.5, 2));
  EXPECT_NEAR(_enge.Function(0, 0), 1., 1e-2);
  EXPECT_NEAR(_enge.Function(_length*10., 0), 0., 1e-2);
  EXPECT_NEAR(_enge.Function(-_length*10., 0), 0., 1e-2);
  _enge.Print(out1);
  EXPECT_EQ(out1.str(), std::string("Enge"));
  Tanh tanh(_tanh_x, _tanh_l, 5);
  EXPECT_EQ(tanh.GetDoubleTanh(0.5, 2), _tanh.Function(0.5, 2));
  EXPECT_NEAR(_tanh.Function(0., 0), 1., 1e-2);
  EXPECT_NEAR(_tanh.Function(_tanh_x+_tanh_l*5., 0), 0., 1e-2);
  EXPECT_NEAR(_tanh.Function(-_tanh_x-_tanh_l*5., 0), 0., 1e-2);
  _tanh.Print(out2);
  EXPECT_EQ(out2.str(), std::string("Tanh"));
}

TEST_F(BTMultipoleTest, BTMultipoleConstructorTest) {
  EXPECT_TRUE(_hard_edged_straight_dip->GetEndField() == NULL);
  EXPECT_DOUBLE_EQ(_hard_edged_straight_dip->GetMagnitude(), _field);
  EXPECT_DOUBLE_EQ(_hard_edged_straight_dip->GetLength(), _length);
  EXPECT_DOUBLE_EQ(_hard_edged_straight_dip->GetHeight(), _height);
  EXPECT_DOUBLE_EQ(_hard_edged_straight_dip->GetWidth(), _width);
  EXPECT_EQ(_hard_edged_straight_dip->GetPole(), 1);
  EXPECT_EQ(_hard_edged_straight_dip->GetEndPole(), _qm);

  EXPECT_TRUE(_enge_end_straight_dip->GetEndField() != NULL);
  EXPECT_NEAR(_enge_end_straight_dip->GetMagnitude(), _field, 1e-3);
  EXPECT_DOUBLE_EQ(_enge_end_straight_dip->GetLength(), _length);
  EXPECT_DOUBLE_EQ(_enge_end_straight_dip->GetHeight(), _height);
  EXPECT_DOUBLE_EQ(_enge_end_straight_dip->GetWidth(), _width);
  EXPECT_EQ(_enge_end_straight_dip->GetPole(), 1);
  EXPECT_EQ(_enge_end_straight_dip->GetEndPole(), _qm);

  EXPECT_TRUE(_tanh_end_straight_dip->GetEndField() != NULL);
  EXPECT_NEAR(_tanh_end_straight_dip->GetMagnitude(), _field, 1e-3);
  EXPECT_DOUBLE_EQ(_tanh_end_straight_dip->GetLength(), _length);
  EXPECT_DOUBLE_EQ(_tanh_end_straight_dip->GetHeight(), _height);
  EXPECT_DOUBLE_EQ(_tanh_end_straight_dip->GetWidth(), _width);
  EXPECT_EQ(_tanh_end_straight_dip->GetPole(), 1);
  EXPECT_EQ(_tanh_end_straight_dip->GetEndPole(), _qm);
}

TEST_F(BTMultipoleTest, BTMultipoleTransformToRotated) {
// Only test straight
  for (int i = 0; i < 3; i++)
    for (double x = -_width*2.; x < _width*2.; x += _width/10)
      for (double y = -_height*2.; y < _height*2.; y += _height/10)
        for (double z = -_length*2.; z < _length*2.; z += _length/10) {
          double point[3] = {x, y, z};
          CLHEP::Hep3Vector test(x, y, z);
          CLHEP::Hep3Vector actual = _mult[i]->TransformToRotated(point);
          for (int j = 0; j < 3; j++)
            ASSERT_NEAR(test[j], actual[j], 1e-10)
                        << test[j] << " " << actual[j];
        }
}

TEST_F(BTMultipoleTest, GetFieldValueTest_HardEdged) {
  double d = 1e-9;
  double x[7] = {-_width,  -_width/2.-d,  -_width/2.+d, 0.,  _width/2.-d,
                  _width/2.+d, _width};
  double y[7] = {-_height, -_height/2.-d, -_height/2.+d, 0., _height/2.-d,
                  _height/2.+d, _height};
  double z[7] = {-_length, -d, +d, _length/2., _length-d, _length+d,
                  _length*2.};
  for (int i = 0; i < 7; i++)
    for (int j = 0; j < 7; j++)
      for (int k = 0; k < 7; k++) {
        double pos[4]   = {x[i], y[j], z[k], 0};
        double field[6] = {0, 0, 0, 0, 0, 0};
        _hard_edged_straight_dip->GetFieldValue(pos, field);
        // Check By = _field in the bounding box, By = 0 outside bounding box,
        // Bx, Bz = 0 everywhere
        ASSERT_NEAR(field[0], 0., 1e-9);
        ASSERT_NEAR(field[2], 0., 1e-9);
        if (fabs(x[i]) <= _width/2. && fabs(y[j]) <= _height/2. && z[k] > 0.
                                    && z[k] < _length)
          ASSERT_NEAR(field[1], _field, 1e-9);
        else
          ASSERT_NEAR(field[1], 0., 1e-9);
      }
}

TEST_F(BTMultipoleTest, BTMultipoleGetConstTest) {
  // tested in GetFieldValue tests;
}

// I have quite thorough test for tanh dipole
// * check that I calculate dipole fields appropriately
// * check that higher order terms are indeed more maxwellian than lower order
//   terms.
// * check that bounding box is okay
TEST_F(BTMultipoleTest, GetFieldValueTest_TanhDipole) {
  double dx = 1e-9;
  double delta[4] = {dx, dx, dx, dx};
  BTMultipole tanh_qm_plus_dip;
  tanh_qm_plus_dip.Init(1, _field, _length, _height, _width, "", 0.,
                               &_tanh, _qm+1);
  // check dipole on axis; check maxwellian on axis;
  // check _qm+1 is more maxwellian on axis
  for (double z = -_length; z < _length*2.0001; z += _length/20.1) {
    double pos[4]   = {0., 0., z, 0.};
    double field[6] = {0., 0., 0., 0., 0., 0.};
    _tanh_end_straight_dip->GetFieldValue(pos, field);
    // dipole on axis
    ASSERT_NEAR(field[0], 0., 1e-6);
    if (z > 0. && z < _length) {
      ASSERT_NEAR(field[1], _tanh_end_straight_dip->GetMagnitude()*
                            _tanh.Function(z-_length/2., 0), 1e-9);
    } else {
      ASSERT_NEAR(field[1], 0., 1e-6);
    }
    ASSERT_NEAR(field[2], 0., 1e-6);
    // maxwellian near axis
    pos[1] += _width/20.1;
    double m1 = _tanh_end_straight_dip->Maxwell2(pos, delta);
    double m2 = (&tanh_qm_plus_dip)->Maxwell2(pos, delta);
    ASSERT_TRUE(fabs(m1) < 1e-3) << "Div_B=" << m1 << " z=" << z << " by="
                               << field[1];
    // higher order term is more maxwellian on axis
    ASSERT_TRUE(fabs(m2) < 1e-5) << "Div_B=" << m1 << " z=" << z << " by="
                               << field[1];
  }
  // check bounding box
  double d = 1e-9;
  double x[7] = {-_width,  -_width/2.-d,  -_width/2.+d, 0.,  _width/2.-d,
                                           _width/2.+d, _width};
  double y[7] = {-_height, -_height/2.-d, -_height/2.+d, 0., _height/2.-d,
                                           _height/2.+d, _height};
  for (int i = 0; i < 7; i++)
    for (int j = 0; j < 7; j++)
      for (double z = -_length; z < _length*2.0001; z += _length/20.1) {
        double pos[4]   = {x[i], y[j], z, 0.};
        double field[6] = {0., 0., 0., 0., 0., 0.};
        _tanh_end_straight_dip->GetFieldValue(pos, field);
        double field_mag2 = field[0]*field[0]+field[1]*field[1]
                                             +field[2]*field[2];
        if (fabs(x[i]) <= _width/2. && fabs(y[j]) <= _height/2. &&
            _tanh_end_straight_dip->GetMagnitude()*
            _tanh.Function(z-_length/2., 0) > 1e-6 && z > 0. && z < _length)
          EXPECT_TRUE(field_mag2>0.) << CLHEP::Hep3Vector(x[i],y[j],z) << " " 
              << field_mag2 << " " << _tanh_end_straight_dip->GetMagnitude()*
                _tanh.Function(z-_length/2., 0);
        else if (fabs(x[i]) > _width/2. && fabs(y[j]) > _height/2.)
          ASSERT_TRUE(field_mag2 < 1e-9);
      }
}

// Again, reasonably thorough tests for tanh quadrupole
// * check that I calculate quad fields appropriately
// * check that higher order terms are indeed more maxwellian than lower order
//   terms.
TEST_F(BTMultipoleTest, GetFieldValueTest_TanhQuad) {
  double dx = 1e-9;
  double delta[4] = {dx, dx, dx, dx};
  // check quadrupole field; check maxwellian on axis;
  // check _qm+1 is more maxwellian on axis
  BTMultipole tanh_qm_plus_quad;
  tanh_qm_plus_quad.Init(2, _field, _length, _height, _width, "", 0.,
                                &_tanh, _qm+1);
  double m1(0.), m2(0.);
  for (double z = -_length; z < _length*2.0001; z += _length/20.) {
    double pos[4]   = {0., 0., z, 0.};
    double field[6] = {0., 0., 0., 0., 0., 0.};
    _tanh_end_straight_quad->GetFieldValue(pos, field);
    // quadrupole near axis
    MemberFunctionConst<const BTMultipole>* func = new MemberFunctionConst
                  <const BTMultipole>(*_tanh_end_straight_quad,
                                      &BTMultipole::GetFieldValue, 4, 6);
    Differentiator* diff = new Differentiator(func,
        std::vector<double>(&delta[0], &delta[0]+4), std::vector<double>(2, 1));
    MAUS::Matrix<double> differentials(6, 5, 0.);
    MAUS::Vector<double> aPoint(pos, 5);
    diff->F(aPoint, differentials);
    for (size_t i = 1; i <= 4; ++i)
      for (size_t j = 1; j <= 3; ++j) {
        if ((z > 0 && z < _length) && 
                                     ((i == 2 && j == 2) || (i == 1 && j == 3)))
          ASSERT_NEAR(differentials(i, j),
                      _tanh_end_straight_quad->GetMagnitude()*
                      _tanh.Function(z-_length/2, 0), 1e-6 )
                          << i << " " << j << " " << z;
        else
          ASSERT_NEAR(differentials(i, j), 0., 1e-12)
                          << i << " " << j << " " << z;
      }
    // maxwellian near axis
    pos[0] = _width/20.1;
    pos[1] = _width/20.1;
    m1 += (_tanh_end_straight_quad)->Maxwell2(pos, delta);
    m2 += (&tanh_qm_plus_quad)->Maxwell2(pos, delta);
  }
  //Higher order should improve Maxwellianness near axis
  ASSERT_TRUE(fabs(m2) < fabs(m1))
                        << m1 << " " << m2;
}

// I don't test so hard for enge - check the field in a few places but that's it
// assume that as I calculate enge derivatives correctly, the field should be
// correct (expansion tested in tanh).
TEST_F(BTMultipoleTest, GetFieldValueTest_Enge) {
  // I only check for enge field on axis
  for (double z = -_length; z < _length*2.0001; z += _length/20.) {
    double pos[4]   = {0., 0., z, 0.};
    double field[6] = {0., 0., 0., 0., 0., 0.};
    _enge_end_straight_dip->GetFieldValue(pos, field);
    ASSERT_NEAR(field[0], 0., 1e-9);
    if (z > 0 && z < _length) {
      ASSERT_NEAR(field[1], _enge_end_straight_dip->GetMagnitude()*
                            _enge.Function(z-_length/2., 0), 1e-9);
    } else {
      ASSERT_NEAR(field[1], 0., 1e-9);
    }
    ASSERT_NEAR(field[2], 0., 1e-9);
  }
}

TEST_F(BTMultipoleTest, PrintTest) {
  std::string name = std::string("Multipole Order: 2 Length: 3 Magnitude: 5")+
              " BendingRadius: 0 Height: 0.4 Width: 0.3 EndField: HardEdged"+
              " scaleFactor: 1 position: (0,0,0) rotationVector: (0,0,1)"+
              " angle: 0 name: \n";
  std::stringstream ss;
  _hard_edged_straight_dip->Print(ss);
  EXPECT_EQ(name, ss.str());
}
}  // namespace

