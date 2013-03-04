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

#include "src/common_cpp/DataStructure/KLDigit.hh"

namespace MAUS {

class KLDigitTestDS : public ::testing::Test {
 protected:
  KLDigitTestDS()  {}
  virtual ~KLDigitTestDS() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
};


TEST_F(KLDigitTestDS, test_parameter_constructor) {
  int pmt = 1;
  int charge_mm = 1200;
  int charge_pm = 1201;
  int phys_event_number = 2;
  std::string kl_key = "KLChannelKey 0 1 kl";
  int part_event_number = 10;
  int cell = 9;
  int position_max = 45;

  KLDigit *digit = new KLDigit(pmt, charge_mm, charge_pm, phys_event_number,
			       kl_key, part_event_number, cell, position_max);

  EXPECT_EQ(digit->GetPmt(), pmt);
  EXPECT_EQ(digit->GetChargeMm(), charge_mm);
  EXPECT_EQ(digit->GetChargePm(), charge_pm);
  EXPECT_EQ(digit->GetPhysEventNumber(), phys_event_number);
  EXPECT_EQ(digit->GetKlKey(), kl_key);
  EXPECT_EQ(digit->GetPartEventNumber(), part_event_number);
  EXPECT_EQ(digit->GetCell(), cell);
  EXPECT_EQ(digit->GetPositionMax(), position_max);
}

TEST_F(KLDigitTestDS, test_copy_constructor) {
  int pmt = 1;
  int charge_mm = 1200;
  int charge_pm = 1201;
  int phys_event_number = 2;
  std::string kl_key = "KLChannelKey 0 1 kl";
  int part_event_number = 10;
  int cell = 9;
  int position_max = 45;


  KLDigit *digit1 = new KLDigit(pmt, charge_mm, charge_pm, phys_event_number,
				kl_key, part_event_number, cell, position_max);
  KLDigit *digit2 = new KLDigit(*digit1);
  delete digit1;

  EXPECT_EQ(digit2->GetPmt(), pmt);
  EXPECT_EQ(digit2->GetChargeMm(), charge_mm);
  EXPECT_EQ(digit2->GetChargePm(), charge_pm);
  EXPECT_EQ(digit2->GetPhysEventNumber(), phys_event_number);
  EXPECT_EQ(digit2->GetKlKey(), kl_key);
  EXPECT_EQ(digit2->GetPartEventNumber(), part_event_number);
  EXPECT_EQ(digit2->GetCell(), cell);
  EXPECT_EQ(digit2->GetPositionMax(), position_max);
}

TEST_F(KLDigitTestDS, test_assignment_operator) {
  int pmt = 1;
  int charge_mm = 1200;
  int charge_pm = 1201;
  int phys_event_number = 2;
  std::string kl_key = "KLChannelKey 0 1 kl";
  int part_event_number = 10;
  int cell = 9;
  int position_max = 45;


  KLDigit *digit1 = new KLDigit(pmt, charge_mm, charge_pm, phys_event_number,
				kl_key, part_event_number, cell, position_max);
  KLDigit *digit2 = new KLDigit();
  *digit2 = *digit1;
  delete digit1;

  EXPECT_EQ(digit2->GetPmt(), pmt);
  EXPECT_EQ(digit2->GetChargeMm(), charge_mm);
  EXPECT_EQ(digit2->GetChargePm(), charge_pm);
  EXPECT_EQ(digit2->GetPhysEventNumber(), phys_event_number);
  EXPECT_EQ(digit2->GetKlKey(), kl_key);
  EXPECT_EQ(digit2->GetPartEventNumber(), part_event_number);
  EXPECT_EQ(digit2->GetCell(), cell);
  EXPECT_EQ(digit2->GetPositionMax(), position_max);
}


TEST_F(KLDigitTestDS, test_setters_getters) {
  int pmt = 1;
  int charge_mm = 1200;
  int charge_pm = 1201;
  int phys_event_number = 2;
  std::string kl_key = "KLChannelKey 0 1 kl";
  int part_event_number = 10;
  int cell = 9;
  int position_max = 45;
  KLDigit *digit = new KLDigit();

  digit->SetPmt(pmt);
  digit->SetChargeMm(charge_mm);
  digit->SetChargePm(charge_pm);
  digit->SetPhysEventNumber(phys_event_number);
  digit->SetKlKey(kl_key);
  digit->SetPartEventNumber(part_event_number);
  digit->SetCell(cell);
  digit->SetPositionMax(position_max);

  EXPECT_EQ(digit->GetPmt(), pmt);
  EXPECT_EQ(digit->GetChargeMm(), charge_mm);
  EXPECT_EQ(digit->GetChargePm(), charge_pm);
  EXPECT_EQ(digit->GetPhysEventNumber(), phys_event_number);
  EXPECT_EQ(digit->GetKlKey(), kl_key);
  EXPECT_EQ(digit->GetPartEventNumber(), part_event_number);
  EXPECT_EQ(digit->GetCell(), cell);
  EXPECT_EQ(digit->GetPositionMax(), position_max);
}

} // ~namespace MAUS
