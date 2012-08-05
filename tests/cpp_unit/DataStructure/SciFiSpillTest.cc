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

#include "src/common_cpp/DataStructure/SciFiSpill.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/SciFiDigit.hh"

namespace MAUS {

class SciFiSpillTestDS : public ::testing::Test {
 protected:
  SciFiSpillTestDS()  {}
  virtual ~SciFiSpillTestDS() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
};

TEST_F(SciFiSpillTestDS, test_default_contructor) {
  SciFiSpill spill;
  EXPECT_EQ(spill.events().size(), 0);
}

TEST_F(SciFiSpillTestDS, test_copy_contructor) {
  SciFiSpill *spill1 = new SciFiSpill();
  SciFiEvent *event = new SciFiEvent();
  SciFiDigit *digit = new SciFiDigit();

  int tracker = 1;
  digit->set_tracker(tracker);

  event->add_digit(digit);
  spill1->add_event(event);

  SciFiSpill *spill2 = new SciFiSpill(*spill1);

  delete event;
  delete spill1;

  ASSERT_EQ(spill2->events().size(), 1);
  ASSERT_EQ(spill2->events()[0]->digits().size(), 1);
  ASSERT_TRUE(spill2->events()[0]->digits()[0]);
  EXPECT_EQ(spill2->events()[0]->digits()[0]->get_tracker(), tracker);
}

TEST_F(SciFiSpillTestDS, test_assignment_operator) {
  SciFiSpill *spill1 = new SciFiSpill();
  SciFiEvent *event = new SciFiEvent();
  SciFiDigit *digit = new SciFiDigit();

  int tracker = 1;
  digit->set_tracker(tracker);

  event->add_digit(digit);
  spill1->add_event(event);

  SciFiSpill *spill2 = new SciFiSpill();

  *spill2 = *spill1;

  delete event;
  delete spill1;

  ASSERT_EQ(spill2->events().size(), 1);
  ASSERT_EQ(spill2->events()[0]->digits().size(), 1);
  ASSERT_TRUE(spill2->events()[0]->digits()[0]);
  EXPECT_EQ(spill2->events()[0]->digits()[0]->get_tracker(), tracker);
}


TEST_F(SciFiSpillTestDS, test_add_methods) {
  SciFiSpill spill;
  SciFiEvent* event = new SciFiEvent();
  SciFiDigit* digit = new SciFiDigit();

  int tracker = 1;
  digit->set_tracker(tracker);

  event->add_digit(digit);
  spill.add_event(event);

  // Check _scifevents and _scifidigits have size 1, and digit pointer is not NULL
  ASSERT_EQ(spill.events().size(), 1);
  ASSERT_EQ(spill.events()[0]->digits().size(), 1);
  ASSERT_TRUE(spill.events()[0]->digits()[0]);
  EXPECT_EQ(spill.events()[0]->digits()[0]->get_tracker(), tracker);
}

} // ~MAUS namespace
