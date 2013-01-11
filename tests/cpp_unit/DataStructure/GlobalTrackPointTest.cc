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
#include "src/common_cpp/DataStructure/GlobalTrackPoint.hh"

#include "gtest/gtest.h"

namespace MAUS {

class GlobalTrackPointTestDS : public ::testing::Test {
  protected:
    GlobalTrackPointTestDS()  {}
    virtual ~GlobalTrackPointTestDS() {}
    virtual void SetUp()    {}
    virtual void TearDown() {}
};

TEST_F(GlobalTrackPointTestDS, test_getters_setters) {
  std::string mapper_name = "0";
  
  double charge = 1.0;

  double x = -2.0;
  double y = -3.0;
  double z = -4.0;
  double t = -5.0;
  TLorentzVector pos(x, y, z, t);

  double ex = -6.0;
  double ey = -7.0;
  double ez = -8.0;
  double et = -9.0;
  TLorentzVector epos(ex, ey, ez, et);

  double px = 2.0;
  double py = 3.0;
  double pz = 4.0;
  double pt = 5.0;
  TLorentzVector mom(px, py, pz, pt);

  double epx = 6.0;
  double epy = 7.0;
  double epz = 8.0;
  double ept = 9.0;
  TLorentzVector emom(epx, epy, epz, ept);

  recon::global::DetectorPoint det = recon::global::kTracker1S1; // 10

  std::string geom_path = "11";

  recon::global::SpacePoint* sp = new recon::global::SpacePoint();

  recon::global::TrackPoint tp;
  tp.set_mapper_name(mapper_name);
  tp.set_charge(charge);
  tp.set_position(pos);
  tp.set_position_error(epos);
  tp.set_momentum(mom);
  tp.set_momentum_error(emom);
  tp.set_detector(det);
  tp.set_geometry_path(geom_path);
  tp.set_spacepoint(sp);

  EXPECT_EQ(mapper_name, tp.get_mapper_name());
  EXPECT_EQ(charge, tp.get_charge());
  EXPECT_EQ(x, tp.get_position().X());
  EXPECT_EQ(y, tp.get_position().Y());
  EXPECT_EQ(z, tp.get_position().Z());
  EXPECT_EQ(t, tp.get_position().T());
  EXPECT_EQ(ex, tp.get_position_error().X());
  EXPECT_EQ(ey, tp.get_position_error().Y());
  EXPECT_EQ(ez, tp.get_position_error().Z());
  EXPECT_EQ(et, tp.get_position_error().T());
  EXPECT_EQ(px, tp.get_momentum().X());
  EXPECT_EQ(py, tp.get_momentum().Y());
  EXPECT_EQ(pz, tp.get_momentum().Z());
  EXPECT_EQ(pt, tp.get_momentum().T());
  EXPECT_EQ(epx, tp.get_momentum_error().X());
  EXPECT_EQ(epy, tp.get_momentum_error().Y());
  EXPECT_EQ(epz, tp.get_momentum_error().Z());
  EXPECT_EQ(ept, tp.get_momentum_error().T());
  EXPECT_EQ(det, tp.get_detector());
  EXPECT_EQ(geom_path, tp.get_geometry_path());
  EXPECT_EQ(sp, tp.get_spacepoint());
}



TEST_F(GlobalTrackPointTestDS, test_default_constructor) {
  recon::global::TrackPoint tp;
  EXPECT_EQ("", tp.get_mapper_name());
  EXPECT_EQ(0., tp.get_charge());
  EXPECT_EQ(0., tp.get_position().X());
  EXPECT_EQ(0., tp.get_position().Y());
  EXPECT_EQ(0., tp.get_position().Z());
  EXPECT_EQ(0., tp.get_position().T());
  EXPECT_EQ(0., tp.get_position_error().X());
  EXPECT_EQ(0., tp.get_position_error().Y());
  EXPECT_EQ(0., tp.get_position_error().Z());
  EXPECT_EQ(0., tp.get_position_error().T());
  EXPECT_EQ(0., tp.get_momentum().X());
  EXPECT_EQ(0., tp.get_momentum().Y());
  EXPECT_EQ(0., tp.get_momentum().Z());
  EXPECT_EQ(0., tp.get_momentum().T());
  EXPECT_EQ(0., tp.get_momentum_error().X());
  EXPECT_EQ(0., tp.get_momentum_error().Y());
  EXPECT_EQ(0., tp.get_momentum_error().Z());
  EXPECT_EQ(0., tp.get_momentum_error().T());
  EXPECT_EQ(recon::global::kUndefined, tp.get_detector());
  EXPECT_EQ("", tp.get_geometry_path());
  EXPECT_TRUE(tp.get_spacepoint() == NULL);
}

TEST_F(GlobalTrackPointTestDS, test_copy_constructor) {
  std::string mapper_name = "0";
  
  double charge = 1.0;

  double x = -2.0;
  double y = -3.0;
  double z = -4.0;
  double t = -5.0;
  TLorentzVector pos(x, y, z, t);

  recon::global::SpacePoint* sp = new recon::global::SpacePoint();

  recon::global::TrackPoint tp1;
  tp1.set_mapper_name(mapper_name);
  tp1.set_charge(charge);
  tp1.set_position(pos);
  tp1.set_spacepoint(sp);

  recon::global::TrackPoint tp2(tp1);

  EXPECT_EQ(mapper_name, tp2.get_mapper_name());
  EXPECT_EQ(charge, tp2.get_charge());
  EXPECT_EQ(x, tp2.get_position().X());
  EXPECT_EQ(y, tp2.get_position().Y());
  EXPECT_EQ(z, tp2.get_position().Z());
  EXPECT_EQ(t, tp2.get_position().T());
  EXPECT_EQ(sp, tp2.get_spacepoint());
}

TEST_F(GlobalTrackPointTestDS, test_assignment_operator) {
  std::string mapper_name = "0";
  
  double charge = 1.0;

  double x = -2.0;
  double y = -3.0;
  double z = -4.0;
  double t = -5.0;
  TLorentzVector pos(x, y, z, t);

  recon::global::SpacePoint* sp = new recon::global::SpacePoint();

  recon::global::TrackPoint tp1;
  tp1.set_mapper_name(mapper_name);
  tp1.set_charge(charge);
  tp1.set_position(pos);
  tp1.set_spacepoint(sp);

  recon::global::TrackPoint tp2 = tp1;

  EXPECT_EQ(mapper_name, tp2.get_mapper_name());
  EXPECT_EQ(charge, tp2.get_charge());
  EXPECT_EQ(x, tp2.get_position().X());
  EXPECT_EQ(y, tp2.get_position().Y());
  EXPECT_EQ(z, tp2.get_position().Z());
  EXPECT_EQ(t, tp2.get_position().T());
  EXPECT_EQ(sp, tp2.get_spacepoint());
}

TEST_F(GlobalTrackPointTestDS, test_clone) {
  std::string mapper_name = "0";
  
  double charge = 1.0;

  double x = -2.0;
  double y = -3.0;
  double z = -4.0;
  double t = -5.0;
  TLorentzVector pos(x, y, z, t);

  recon::global::SpacePoint sp;

  recon::global::TrackPoint *tp1 =
      new recon::global::TrackPoint();
  tp1->set_mapper_name(mapper_name);
  tp1->set_charge(charge);
  tp1->set_position(pos);
  tp1->set_spacepoint(&sp);

  recon::global::TrackPoint *tp2 = tp1->Clone();
  delete tp1;

  EXPECT_EQ(mapper_name, tp2->get_mapper_name());
  EXPECT_EQ(charge, tp2->get_charge());
  EXPECT_EQ(x, tp2->get_position().X());
  EXPECT_EQ(y, tp2->get_position().Y());
  EXPECT_EQ(z, tp2->get_position().Z());
  EXPECT_EQ(t, tp2->get_position().T());

  // As of 29/10/2012. Clone creates and stores a Clone of the
  // spacepoint, rather than a direct copy.  Therefore, this test
  // confirms that they have different pointer values.  This may
  // change in the future.
  EXPECT_NE(&sp, tp2->get_spacepoint());
}

} // ~namespace MAUS
