/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "src/common_cpp/DataStructure/GlobalSpacePoint.hh"

#include "gtest/gtest.h"

namespace MAUS {

class GlobalSpacePointTestDS : public ::testing::Test {
  protected:
    GlobalSpacePointTestDS()  {}
    virtual ~GlobalSpacePointTestDS() {}
    virtual void SetUp()    {}
    virtual void TearDown() {}
};

TEST_F(GlobalSpacePointTestDS, test_getters_setters) {
  double charge = 1.0;

  double x = 2.0;
  double y = 3.0;
  double z = 4.0;
  double t = 5.0;
  TLorentzVector pos(x, y, z, t);

  double ex = 6.0;
  double ey = 7.0;
  double ez = 8.0;
  double et = 9.0;
  TLorentzVector epos(ex, ey, ez, et);

  recon::global::DetectorPoint det = recon::global::kTracker1S1; // 10

  std::string geom_path = "11";

  recon::global::SpacePoint sp;
  sp.set_charge(charge);
  sp.set_position(pos);
  sp.set_position_error(epos);
  sp.set_detector(det);
  sp.set_geometry_path(geom_path);

  EXPECT_EQ(charge, sp.get_charge());
  EXPECT_EQ(x, sp.get_position().X());
  EXPECT_EQ(y, sp.get_position().Y());
  EXPECT_EQ(z, sp.get_position().Z());
  EXPECT_EQ(t, sp.get_position().T());
  EXPECT_EQ(ex, sp.get_position_error().X());
  EXPECT_EQ(ey, sp.get_position_error().Y());
  EXPECT_EQ(ez, sp.get_position_error().Z());
  EXPECT_EQ(et, sp.get_position_error().T());
  EXPECT_EQ(det, sp.get_detector());
  EXPECT_EQ(geom_path, sp.get_geometry_path());
}

TEST_F(GlobalSpacePointTestDS, test_default_constructor) {
  recon::global::SpacePoint sp;
  EXPECT_EQ(0., sp.get_charge());
  EXPECT_EQ(0., sp.get_position().X());
  EXPECT_EQ(0., sp.get_position().Y());
  EXPECT_EQ(0., sp.get_position().Z());
  EXPECT_EQ(0., sp.get_position().T());
  EXPECT_EQ(0., sp.get_position_error().X());
  EXPECT_EQ(0., sp.get_position_error().Y());
  EXPECT_EQ(0., sp.get_position_error().Z());
  EXPECT_EQ(0., sp.get_position_error().T());
  EXPECT_EQ(recon::global::kUndefined, sp.get_detector());
  EXPECT_EQ("", sp.get_geometry_path());
}

TEST_F(GlobalSpacePointTestDS, test_copy_constructor) {
  double charge = 1.0;

  recon::global::SpacePoint sp1;
  sp1.set_charge(charge);

  recon::global::SpacePoint sp2(sp1);

  EXPECT_EQ(charge, sp2.get_charge());
}

TEST_F(GlobalSpacePointTestDS, test_assignment_operator) {
  double charge = 1.0;

  recon::global::SpacePoint sp1;
  sp1.set_charge(charge);

  recon::global::SpacePoint sp2 = sp1;

  EXPECT_EQ(charge, sp2.get_charge());
}

TEST_F(GlobalSpacePointTestDS, test_clone) {
  double charge = 1.0;
  recon::global::SpacePoint *sp1 =
      new recon::global::SpacePoint();
  sp1->set_charge(charge);

  recon::global::SpacePoint *sp2 = sp1->Clone();

  EXPECT_EQ(charge, sp2->get_charge());
  EXPECT_NE(sp1, sp2);
}

} // ~namespace MAUS
