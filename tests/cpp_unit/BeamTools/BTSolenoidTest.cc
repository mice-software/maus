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

#include <sys/stat.h>

#include "gtest/gtest.h"

#include "BeamTools/BTSolenoid.hh"

#include "src/legacy/Interface/STLUtils.hh"

namespace {
// Just testing ZMin, ZMax, RMin, RMax functions
class BTSolenoidTest : public ::testing::Test {
 protected:
  BTSolenoidTest() : _numRNodes(4), _numZNodes(5), _numSheets(3),
                     _zExtent(sqrt(30)), _rExtent(sqrt(20))
  {
	  BTSolenoid::SetStaticVariables(_numRNodes, _numZNodes, _numSheets,
                                   _zExtent, _rExtent);

  }
  virtual ~BTSolenoidTest() {
  }
  virtual void SetUp() {}
  virtual void TearDown() {}

  int _numRNodes;
  int _numZNodes;
  int _numSheets;
  double _zExtent;
  double _rExtent;
};

TEST_F(BTSolenoidTest, AccessorsTest) {
  double length=1., radius=3.;
  BTSolenoid sol(length, 2., radius, 1, true, -1, "", "LinearCubic");

  EXPECT_DOUBLE_EQ(sol.ZMin(), -_zExtent*radius-length);
  EXPECT_DOUBLE_EQ(sol.ZMax(), _zExtent*radius+length);
  EXPECT_DOUBLE_EQ(sol.RMin(), 0.);
  EXPECT_DOUBLE_EQ(sol.RMax(), _rExtent*radius);
}

TEST_F(BTSolenoidTest, ReadWriteTest) {
  std::string filename = "${MAUS_ROOT_DIR}/tmp/test.fld";
  std::string f_sub = STLUtils::ReplaceVariables(filename);
  BTSolenoid sol;
  sol.BuildSheets(1., 2., 3., 1., filename);

  struct stat my_stat;
  int stat_out = stat(f_sub.c_str(), &my_stat);
  EXPECT_EQ(stat_out, 0);
}
}

