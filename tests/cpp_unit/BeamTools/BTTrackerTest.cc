// Copyright 2008-2011 Chris Rogers
//
// This file is a part of MAUS
//
// MAUS is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MAUS is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MAUS in the doc folder.  If not, see
// <http://www.gnu.org/licenses/>.


#include "CLHEP/Random/RandFlat.h"

#include "TCanvas.h"
#include "TH2D.h"
#include "TStyle.h"

#include "gtest/gtest.h"

#include "Utils/Squeak.hh"

#include "BeamTools/BTSolenoid.hh"
#include "BeamTools/BTFieldConstructor.hh"
#include "BeamTools/BTTracker.hh"
#include "BeamTools/BTFieldGroup.hh"
#include "Utils/Exception.hh"


namespace {
const double mu_mass = 105.658367;
BTSolenoid *  _sol = NULL;
std::string _var[] = {"t", "x", "y", "z", "energy", "px", "py", "pz"};

class BTTrackerTest : public ::testing::Test {
 protected:
  BTTrackerTest();
  ~BTTrackerTest() {
//    delete _sol;
    delete _rot_pb;
  }
  std::vector<double> MakeVector
      (double t, double x, double y, double z, double px, double py, double pz);
  std::string Print8Vector(double x[8]);

  std::vector< std::vector<double> > _particles;

  double _zStep;
  double _endZ;
  double _endT;
  double _tStep;


  BTPillBox*    _pb;
  BTFieldGroup* _rot_pb;

};

BTTrackerTest::BTTrackerTest() {
  BTFieldConstructor::SetDefaults();
  _zStep = 100.;
  _endZ  = 1000.;
  _endT  = 3.34445340;
  _tStep = _endT*(_zStep/_endZ);
  _particles.push_back(MakeVector(0., 0., 10.,  -220.,  0., 0.,  200.) );
  _particles.push_back(MakeVector(0., 0.,  0.,     0., 100., 0.,  2000.) );
  _particles.push_back(MakeVector(0., 0.,  0.,     0.,  0., 0., -200.) );
  _particles.push_back(MakeVector(0., 0.,  0.,     0.,  0., 0.,    0.) );
  if(_sol == NULL)
    _sol = new BTSolenoid    (5000., 21.3, 258., 150.5, false, 1e-5, "testcoil1", "LinearCubic");
  _pb  = new BTPillBox     (1000., 0.01, 600.); //length, peak field [GV], radius (electrostatic) 
  _rot_pb = new BTFieldGroup();
  _rot_pb->AddField(_pb, Hep3Vector(), HepRotationY(90.*CLHEP::degree));
  BTTracker::absoluteError(1e-8);
  BTTracker::relativeError(1e-8);

}

std::vector<double> BTTrackerTest::MakeVector
     (double t, double x, double y, double z, double px, double py, double pz) {
  double  energy = sqrt(px*px + py*py + pz*pz + mu_mass*mu_mass);
  std::vector<double> vec(8);
  vec[0] = t;
  vec[1] = x;
  vec[2] = y;
  vec[3] = z;
  vec[4] = energy;
  vec[5] = px;
  vec[6] = py;
  vec[7] = pz;
  return vec;
}

std::string BTTrackerTest::Print8Vector(double x[8]) {
  std::stringstream ioss;
  for (size_t i=0; i<8; ++i) {
    ioss << _var[i] << ": " << x[i] << "  ";
  }
  return ioss.str();
}

TEST_F(BTTrackerTest, SolenoidTest_z) {
  double results_sol_z[] = {
    3.7725,  -0.0030,  10.0031,   780.0000,  226.1939, -0.0024,   0.0036,  200.0000,
    3.3445,  47.0543, -14.5537,  1000.0000, 2005.2839, 82.5650, -56.3923, 2000.0007,
    -3.7725,  0.0000,   0.0000,  1000.0000,  226.1939,  0.0000,   0.0000, -200.0000,
    0.0000,   0.0000,   0.0000,     0.0000,  105.6584,  0.0000,   0.0000,    0.0000
  };
  for (size_t p = 0; p < _particles.size(); ++p) {
    std::vector<double> x = std::vector<double>(_particles[p]);
    try {
      for (double z = x[3]; z-_particles[p][3] < _endZ+_endZ/1e3; 
                                                                  z += _zStep) {
        BTTracker::integrate(z, &x[0], _sol, BTTracker::z, 0.1, 1.);
      }
    }
    catch(MAUS::Exceptions::Exception exc) {EXPECT_EQ(p, (size_t) 3);}  // p of particle 3 is 0 => exception
    for(size_t i=0; i<8; ++i) {
      ASSERT_NEAR(results_sol_z[p*8+i], x[i], 1e-3) << "particle " << p 
          << " var " << _var[i] << "\nParticle coords: " << Print8Vector(&x[0]);
    }
  }
}

TEST_F(BTTrackerTest, SolenoidTest_u) {
  double results_sol_u[] = {
    3.7725, -0.0030,  10.0031,  780.0000, 226.1939,  -0.0024,   0.0036,  200.0000,
    3.3445, 47.0543, -14.5537, 1000.0000, 2005.2839, 82.5650, -56.3923, 2000.0007,
   -3.7725,  0.0000,   0.0000, 1000.0000,  226.1939,  0.0000,   0.0000, -200.0000,
    0.0000,  0.0000,   0.0000,    0.0000,  105.6584,  0.0000,   0.0000,    0.0000
  };
  for (size_t p = 0; p < _particles.size(); ++p) {
    std::vector<double> x = std::vector<double>(_particles[p]);
    try {
      for (double u = x[3]; fabs(u)-_endZ-_particles[p][3] < _endZ/1e3; 
                                                                  u += _zStep) {
        BTTracker::integrate(u, &x[0], _sol, BTTracker::u, 0.1, 1.);
      }
    }
    catch(MAUS::Exceptions::Exception exc) {EXPECT_EQ(p, (size_t) 3);}  // p of particle 3 is 0 => exception
    for(size_t i=0; i<8; ++i) {
      ASSERT_NEAR(results_sol_u[p*8+i], x[i], 1e-3) << "particle " << p 
          << " var " << _var[i] << "\nParticle coords: " << Print8Vector(&x[0]);
    }
    CLHEP::Hep3Vector origin(x[1],x[2],x[3]);
    try {
      for (double u = 0; u-_endZ < _endZ/1e3; u += _zStep) {
        BTTracker::integrate(u, &x[0], _sol, BTTracker::u, 0.1, 1.,
                                      origin, CLHEP::HepRotationY(180*degree));
      }
    }
    catch(MAUS::Exceptions::Exception exc) {EXPECT_EQ(p, (size_t) 3);}  // p of particle 3 is 0 => exception
    for(size_t i=0; i<8; ++i) {
      ASSERT_NEAR(_particles[p][i], x[i], 1e-3) << "particle " << p 
          << " var " << _var[i] << "\nParticle coords: " << Print8Vector(&x[0]);
    }
  }
}

TEST_F(BTTrackerTest, SolenoidTest_t) {
  double results[] = {
    3.7725085528427788617, -0.0030, 10.0031, 780.0000, 226.1939, -0.0024, 0.0036, 200.0000,
    3.3444534351854149357, 47.0543, -14.5537, 1000.0000, 2005.2839, 82.5650, -56.3923, 2000.0007,
    0.0000, 0.0000, 0.0000, 0.0000, 226.1939, 0.0000, 0.0000, -200.0000,
    0.0000, 0.0000, 0.0000, 0.0000, 105.6584, 0.0000, 0.0000,    0.0000
  };
  for (size_t p = 0; p < _particles.size(); ++p) {
    std::vector<double> x = std::vector<double>(_particles[p]);
    try {
      double t_m = results[8*p+0];
      double t_step = (t_m-x[0])/100.;
      if (t_step < 1e-3) t_step = 0.001;
      for (double t = x[0]; t < t_m+t_m/1e4; t += t_step) {
        BTTracker::integrate(t, &x[0], _sol, BTTracker::t, 0.01, 1.);
      }
      for (size_t i=0; i<8; ++i) {
        ASSERT_NEAR(results[p*8+i], x[i], 1e-2) << "particle " << p 
          << " var " << _var[i] << "\nParticle coords: " << Print8Vector(&x[0]);
      }
    }
    // pz of particle 2,3 <= 0 =>Exception
    catch(MAUS::Exceptions::Exception exc) {EXPECT_TRUE(p == 2 || p == 2);}
  }
}

//////////////////////////// PILLBOX /////////////////////////////

TEST_F(BTTrackerTest, PillBoxTest_z) {
  double results[] = {
     3.7739, -27.3603, 10.0000,  780.0000,  226.3839, -9.2733, 0.0000,  200.0000,
     3.3441,  47.8946,  0.0000, 1000.0000, 2004.9930, 93.9845, 0.0000, 2000.0000,
    -3.7735,  23.7533,  0.0000, 1000.0000,  226.2957, -6.7869, 0.0000, -200.0000,
     0.0000,   0.0000,  0.0000,    0.0000,  105.6584,  0.0000, 0.0000,    0.0000
  };
  for (size_t p = 0; p < _particles.size(); ++p) {
    std::vector<double> x = std::vector<double>(_particles[p]);
    try {
      for (double z = x[3]; z-_particles[p][3] < _endZ+_endZ/1e3; 
                                                                  z += _zStep) {
        BTTracker::integrate(z, &x[0], _rot_pb, BTTracker::z, 0.1, 1.);
      }
    }
    catch(MAUS::Exceptions::Exception exc) {EXPECT_EQ(p, size_t(3));}  // p of particle 3 is 0 => exception
    for(size_t i=0; i<8; ++i) {
      ASSERT_NEAR(results[p*8+i], x[i], 1e-3) << "particle " << p 
          << " var " << _var[i] << "\nParticle coords: " << Print8Vector(&x[0]);
    }
  }
}

TEST_F(BTTrackerTest, PillBoxTest_u) {
  double results[] = {
     3.7739, -27.3603, 10.0000,  780.0000,  226.3839, -9.2733, 0.0000,  200.0000,
     3.3441,  47.8946,  0.0000, 1000.0000, 2004.9930, 93.9845, 0.0000, 2000.0000,
    -3.7735, -23.7533,  0.0000, 1000.0000,  226.2957,  6.7869, 0.0000, -200.0000,
     0.0000,   0.0000,  0.0000,    0.0000,  105.6584,  0.0000, 0.0000,    0.0000
  };
  for (size_t p = 0; p < _particles.size(); ++p) {
    std::vector<double> x = std::vector<double>(_particles[p]);
    try {
      for (double u = x[3]; fabs(u)-_endZ-_particles[p][3] < _endZ/1e3; 
                                                                  u += _zStep) {
        BTTracker::integrate(u, &x[0], _rot_pb, BTTracker::u, 0.1, 1.);
      }
    }
    catch(MAUS::Exceptions::Exception exc) {EXPECT_EQ(p, size_t(3));}  // p of particle 3 is 0 => exception
    for(size_t i=0; i<8; ++i) {
      ASSERT_NEAR(results[p*8+i], x[i], 1e-3) << "(no rot) particle " << p 
          << " var " << _var[i] << "\nParticle coords: " << Print8Vector(&x[0]);
    }
    CLHEP::Hep3Vector origin(x[1],x[2],x[3]);
    try {
      for (double u = 0; u-_endZ < _endZ/1e3; u += _zStep) {
        BTTracker::integrate(u, &x[0], _rot_pb, BTTracker::u, 0.1, 1.,
                                      origin, CLHEP::HepRotationY(180*degree));
      }
    }
    catch(MAUS::Exceptions::Exception exc) {EXPECT_EQ(p, size_t(3));}  // p of particle 3 is 0 => exception
    for(size_t i=0; i<8; ++i) {
      ASSERT_NEAR(_particles[p][i], x[i], 1e-1) << "(rot) particle " << p 
          << " var " << _var[i] << "\nParticle coords: " << Print8Vector(&x[0]);
    }
  }
}
// Particle coords: t: 9.52933e-07  x: -0.0139323  y: 10  z: -220  energy: 226.194  px: 0.00339696  py: 0  pz: 200  

TEST_F(BTTrackerTest, PillBoxTest_t) {
  double results[] = {
    3.773946426431249, -27.3644, 10.0000,  779.9998,  226.3841, -9.2780, 0.0000,  200.0000,
    3.3441123976151883,  47.8896,  0.0000, 1000.0002, 2004.9918, 93.9598, 0.0000, 2000,
    0.0000, 0.0000, 0.0000, 0.0000,  226.194,  0.0000, 0.0000, -200.0000,
    0.0000, 0.0000, 0.0000, 0.0000,  105.6584,  0.0000, 0.0000,    0.0000
  };
  for (size_t p = 0; p < _particles.size(); ++p) {
    std::vector<double> x = std::vector<double>(_particles[p]);
    try {
      double t_m = results[8*p+0];
      double t_step = (t_m-x[0])/100.;
      if (t_step < 1e-3) t_step = 0.001;
      for (double t = x[0]; t < t_m+t_m/1e4; t += t_step) {
        BTTracker::integrate(t, &x[0], _rot_pb, BTTracker::t, 0.001, 1.);
      }
      for(size_t i=0; i<8; ++i) {
        ASSERT_NEAR(results[p*8+i], x[i], 1e-1) << "particle " << p << " t_m " << t_m 
          << " var " << _var[i] << "\nParticle coords: " << Print8Vector(&x[0]);
      }
    }
    // pz of particle 2,3 <= 0 =>Exception
    catch(MAUS::Exceptions::Exception exc) {EXPECT_TRUE(p == 2 || p == 2);}
  }
}

}

