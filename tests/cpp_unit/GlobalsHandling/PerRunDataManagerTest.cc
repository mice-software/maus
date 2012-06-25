/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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

#include "src/legacy/Interface/Squeal.hh"

#include "src/common_cpp/DataStructure/RunHeader.hh"
#include "src/common_cpp/DataStructure/RunFooter.hh"

#include "src/common_cpp/GlobalsHandling/PerRunDataBase.hh"
#include "src/common_cpp/GlobalsHandling/PerRunDataManager.hh"

namespace MAUS {

class DataRunActionTest : public DataRunActionBase {
  public:
    DataRunActionTest() : _my_run_number(0) {reference_count++;}
    ~DataRunActionTest() {reference_count--;}

    DataRunActionTest* Clone() {return new DataRunActionTest();};

    void StartOfRun(RunHeader* run_header) {
        _my_run_number = run_header->GetRunNumber();
    }
    void EndOfRun(RunFooter* run_footer) {
        _my_run_number = -run_footer->GetRunNumber();
    }
    int _my_run_number;
    static int reference_count;

  private:
};

int DataRunActionTest::reference_count = 0;

TEST(DataRunActionManagerTest, TestConstructDestruct) {
    DataRunActionManager* data_run = new DataRunActionManager();
    delete data_run;
}

TEST(DataRunActionManagerTest, TestPushBack) {
    DataRunActionManager* data_run = new DataRunActionManager();
    data_run->PushBack(new DataRunActionTest());
    data_run->PushBack(new DataRunActionTest());
    data_run->PushBack(new DataRunActionTest());
    delete data_run;
    ASSERT_EQ(DataRunActionTest::reference_count, 0);
    data_run = new DataRunActionManager();
    EXPECT_THROW(data_run->PushBack(NULL), Squeal);
    delete data_run;

    DataRunActionTest* data_run_action = new DataRunActionTest();
    data_run = new DataRunActionManager();
    data_run->PushBack(data_run_action);
    EXPECT_THROW(data_run->PushBack(data_run_action), Squeal);
    delete data_run;
}

TEST(DataRunActionManagerTest, TestStartOfRunAction) {
    RunHeader header;
    header.SetRunNumber(10);
    DataRunActionManager* data_run = new DataRunActionManager();
    DataRunActionTest* run_action_1 = new DataRunActionTest();
    DataRunActionTest* run_action_2 = new DataRunActionTest();
    // check it's okay if the run_action_list is empty
    data_run->StartOfRun(&header);
    // check that we update run actions
    data_run->PushBack(run_action_1);
    data_run->PushBack(run_action_2);
    data_run->StartOfRun(&header);
    EXPECT_EQ(run_action_1->_my_run_number, header.GetRunNumber());
    EXPECT_EQ(run_action_2->_my_run_number, header.GetRunNumber());
    delete data_run; // clean up
}

TEST(DataRunActionManagerTest, TestEndOfRunAction) {
    RunFooter footer;
    footer.SetRunNumber(10);
    DataRunActionManager* data_run = new DataRunActionManager();
    DataRunActionTest* run_action_1 = new DataRunActionTest(); 
    DataRunActionTest* run_action_2 = new DataRunActionTest(); 
    // check it's okay if the run_action_list is empty
    data_run->EndOfRun(&footer);
    // check that we update run actions
    data_run->PushBack(run_action_1);
    data_run->PushBack(run_action_2);
    data_run->EndOfRun(&footer);
    EXPECT_EQ(run_action_1->_my_run_number, -footer.GetRunNumber());
    EXPECT_EQ(run_action_2->_my_run_number, -footer.GetRunNumber());
    delete data_run; // clean up
}

TEST(DataRunActionManagerTest, TestSwap) {
    DataRunActionManager* data_run = new DataRunActionManager();
    DataRunActionTest* run_action_1 = new DataRunActionTest();
    DataRunActionTest* run_action_2 = new DataRunActionTest();
    DataRunActionTest* run_action_3 = new DataRunActionTest();
    data_run->PushBack(run_action_1);
    RunHeader header;
    header.SetRunNumber(10);
    data_run->StartOfRun(&header);
    EXPECT_EQ(run_action_1->_my_run_number, 10);
    EXPECT_EQ(run_action_2->_my_run_number, 0);

    data_run->Swap(run_action_1, run_action_2);
    header.SetRunNumber(11);
    data_run->StartOfRun(&header);
    EXPECT_EQ(run_action_1->_my_run_number, 10);
    EXPECT_EQ(run_action_2->_my_run_number, 11);

    // run_action_1 is not in data_run
    EXPECT_THROW(data_run->Swap(run_action_1, run_action_3), Squeal);
    data_run->PushBack(run_action_3);
    // run_action_3 is already in data_run
    EXPECT_THROW(data_run->Swap(run_action_2, run_action_3), Squeal);
    delete data_run;
    delete run_action_1;
}

TEST(DataRunActionManagerTest, TestReferenceCount) {
    EXPECT_EQ(DataRunActionTest::reference_count, 0);
}
}

