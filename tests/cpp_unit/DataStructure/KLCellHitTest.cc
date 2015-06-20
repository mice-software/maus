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

#include "gtest/gtest.h"

#include "src/common_cpp/DataStructure/KLCellHit.hh"

namespace MAUS {

class KLCellHitTestDS : public ::testing::Test {
 protected:
  KLCellHitTestDS()  {}
  virtual ~KLCellHitTestDS() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
};


TEST_F(KLCellHitTestDS, test_parameter_constructor) {
  int cell = 9;
  int phys_event_number = 2;
  int charge = 2000;
  int charge_product = 1000;
  std::string detector = "kl";
  int part_event_number = 10;
  double global_pos_x = 10.;
  double global_pos_y = -88.;
  double global_pos_z = 100.;
  double local_pos_x = 20.;
  double local_pos_y = -44.;
  double local_pos_z = 200.;
  double error_x = 100.;
  double error_y = 22.;
  double error_z = 20.;
  bool flag = 1;

  KLCellHit *cellhit = new KLCellHit(cell, phys_event_number, charge, charge_product,
				     detector, part_event_number,
				     global_pos_x, global_pos_y, global_pos_z,
				     local_pos_x, local_pos_y, local_pos_z,
				     error_x, error_y, error_z, flag );

  EXPECT_EQ(cellhit->GetCell(), cell);
  EXPECT_EQ(cellhit->GetPhysEventNumber(), phys_event_number);
  EXPECT_EQ(cellhit->GetCharge(), charge);
  EXPECT_EQ(cellhit->GetChargeProduct(), charge_product);
  EXPECT_EQ(cellhit->GetDetector(), detector);
  EXPECT_EQ(cellhit->GetPartEventNumber(), part_event_number);
  EXPECT_EQ(cellhit->GetGlobalPosX(), global_pos_x);
  EXPECT_EQ(cellhit->GetGlobalPosY(), global_pos_y);
  EXPECT_EQ(cellhit->GetGlobalPosZ(), global_pos_z);
  EXPECT_EQ(cellhit->GetLocalPosX(), local_pos_x);
  EXPECT_EQ(cellhit->GetLocalPosY(), local_pos_y);
  EXPECT_EQ(cellhit->GetLocalPosZ(), local_pos_z);
  EXPECT_EQ(cellhit->GetErrorX(), error_x);
  EXPECT_EQ(cellhit->GetErrorY(), error_y);
  EXPECT_EQ(cellhit->GetErrorZ(), error_z);
  EXPECT_EQ(cellhit->GetFlag(), flag);
}

TEST_F(KLCellHitTestDS, test_copy_constructor) {
  int cell = 9;
  int phys_event_number = 2;
  int charge = 2000;
  int charge_product = 1000;
  std::string detector = "kl";
  int part_event_number = 10;
  double global_pos_x = 10;
  double global_pos_y = -88.;
  double global_pos_z = 100;
  double local_pos_x = 20;
  double local_pos_y = -44;
  double local_pos_z = 200;
  double error_x = 100.;
  double error_y = 22.;
  double error_z = 20.;
  bool flag = 1;

  KLCellHit *cellhit1 = new KLCellHit(cell, phys_event_number, charge, charge_product,
				      detector, part_event_number,
				      global_pos_x, global_pos_y, global_pos_z,
				      local_pos_x, local_pos_y, local_pos_z,
				      error_x, error_y, error_z, flag );
  KLCellHit *cellhit2 = new KLCellHit(*cellhit1);
  delete cellhit1;

  EXPECT_EQ(cellhit2->GetCell(), cell);
  EXPECT_EQ(cellhit2->GetPhysEventNumber(), phys_event_number);
  EXPECT_EQ(cellhit2->GetCharge(), charge);
  EXPECT_EQ(cellhit2->GetChargeProduct(), charge_product);
  EXPECT_EQ(cellhit2->GetDetector(), detector);
  EXPECT_EQ(cellhit2->GetPartEventNumber(), part_event_number);
  EXPECT_EQ(cellhit2->GetGlobalPosX(), global_pos_x);
  EXPECT_EQ(cellhit2->GetGlobalPosY(), global_pos_y);
  EXPECT_EQ(cellhit2->GetGlobalPosZ(), global_pos_z);
  EXPECT_EQ(cellhit2->GetLocalPosX(), local_pos_x);
  EXPECT_EQ(cellhit2->GetLocalPosY(), local_pos_y);
  EXPECT_EQ(cellhit2->GetLocalPosZ(), local_pos_z);
  EXPECT_EQ(cellhit2->GetErrorX(), error_x);
  EXPECT_EQ(cellhit2->GetErrorY(), error_y);
  EXPECT_EQ(cellhit2->GetErrorZ(), error_z);
  EXPECT_EQ(cellhit2->GetFlag(), flag);
}

TEST_F(KLCellHitTestDS, test_assignment_operator) {
  int cell = 9;
  int phys_event_number = 2;
  int charge = 2000;
  int charge_product = 1000;
  std::string detector = "kl";
  int part_event_number = 10;
  double global_pos_x = 10;
  double global_pos_y = -88.;
  double global_pos_z = 100;
  double local_pos_x = 20;
  double local_pos_y = -44;
  double local_pos_z = 200;
  double error_x = 100.;
  double error_y = 22.;
  double error_z = 20.;
  bool flag = 1;

  KLCellHit *cellhit1 = new KLCellHit(cell, phys_event_number, charge, charge_product,
				      detector, part_event_number,
				      global_pos_x, global_pos_y, global_pos_z,
				      local_pos_x, local_pos_y, local_pos_z,
				      error_x, error_y, error_z, flag );
  KLCellHit *cellhit2 = new KLCellHit();
  *cellhit2 = *cellhit1;
  delete cellhit1;

  EXPECT_EQ(cellhit2->GetCell(), cell);
  EXPECT_EQ(cellhit2->GetPhysEventNumber(), phys_event_number);
  EXPECT_EQ(cellhit2->GetCharge(), charge);
  EXPECT_EQ(cellhit2->GetChargeProduct(), charge_product);
  EXPECT_EQ(cellhit2->GetDetector(), detector);
  EXPECT_EQ(cellhit2->GetPartEventNumber(), part_event_number);
  EXPECT_EQ(cellhit2->GetGlobalPosX(), global_pos_x);
  EXPECT_EQ(cellhit2->GetGlobalPosY(), global_pos_y);
  EXPECT_EQ(cellhit2->GetGlobalPosZ(), global_pos_z);
  EXPECT_EQ(cellhit2->GetLocalPosX(), local_pos_x);
  EXPECT_EQ(cellhit2->GetLocalPosY(), local_pos_y);
  EXPECT_EQ(cellhit2->GetLocalPosZ(), local_pos_z);
  EXPECT_EQ(cellhit2->GetErrorX(), error_x);
  EXPECT_EQ(cellhit2->GetErrorY(), error_y);
  EXPECT_EQ(cellhit2->GetErrorZ(), error_z);
  EXPECT_EQ(cellhit2->GetFlag(), flag);
}


TEST_F(KLCellHitTestDS, test_setters_getters) {
  int cell = 9;
  int phys_event_number = 2;
  int charge = 2000;
  int charge_product = 1000;
  std::string detector = "kl";
  int part_event_number = 10;
  double global_pos_x = 10;
  double global_pos_y = -88.;
  double global_pos_z = 100;
  double local_pos_x = 20;
  double local_pos_y = -44;
  double local_pos_z = 200;
  double error_x = 100.;
  double error_y = 22.;
  double error_z = 20.;
  bool flag = 1;
  KLCellHit *cellhit = new KLCellHit();

  cellhit->SetCell(cell);
  cellhit->SetPhysEventNumber(phys_event_number);
  cellhit->SetCharge(charge);
  cellhit->SetChargeProduct(charge_product);
  cellhit->SetDetector(detector);
  cellhit->SetPartEventNumber(part_event_number);
  cellhit->SetGlobalPosX(global_pos_x);
  cellhit->SetGlobalPosY(global_pos_y);
  cellhit->SetGlobalPosZ(global_pos_z);
  cellhit->SetLocalPosX(local_pos_x);
  cellhit->SetLocalPosY(local_pos_y);
  cellhit->SetLocalPosZ(local_pos_z);
  cellhit->SetErrorX(local_pos_x);
  cellhit->SetErrorY(local_pos_y);
  cellhit->SetErrorZ(local_pos_z);
  cellhit->SetFlag(flag);

  EXPECT_EQ(cellhit->GetCell(), cell);
  EXPECT_EQ(cellhit->GetPhysEventNumber(), phys_event_number);
  EXPECT_EQ(cellhit->GetCharge(), charge);
  EXPECT_EQ(cellhit->GetChargeProduct(), charge_product);
  EXPECT_EQ(cellhit->GetDetector(), detector);
  EXPECT_EQ(cellhit->GetPartEventNumber(), part_event_number);
  EXPECT_EQ(cellhit->GetGlobalPosX(), global_pos_x);
  EXPECT_EQ(cellhit->GetGlobalPosY(), global_pos_y);
  EXPECT_EQ(cellhit->GetGlobalPosZ(), global_pos_z);
  EXPECT_EQ(cellhit->GetLocalPosX(), local_pos_x);
  EXPECT_EQ(cellhit->GetLocalPosY(), local_pos_y);
  EXPECT_EQ(cellhit->GetLocalPosZ(), local_pos_z);
  EXPECT_EQ(cellhit->GetErrorX(), error_x);
  EXPECT_EQ(cellhit->GetErrorY(), error_y);
  EXPECT_EQ(cellhit->GetErrorZ(), error_z);
  EXPECT_EQ(cellhit->GetFlag(), flag);
}

} // ~namespace MAUS
