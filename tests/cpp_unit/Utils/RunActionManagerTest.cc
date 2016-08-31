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

#include "Utils/Exception.hh"

#include "src/common_cpp/DataStructure/RunHeader.hh"
#include "src/common_cpp/DataStructure/RunFooter.hh"

#include "src/common_cpp/Utils/RunActionBase.hh"
#include "src/common_cpp/Utils/RunActionManager.hh"

namespace MAUS {

class RunActionTest : public RunActionBase {
  public:
    RunActionTest() : _my_run_number(0) {reference_count++;}
    ~RunActionTest() {reference_count--;}

    RunActionTest* Clone() {
        return new RunActionTest();
    }

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

int RunActionTest::reference_count = 0;

TEST(RunActionManagerTest, TestConstructDestruct) {
    RunActionManager* data_run = new RunActionManager();
    delete data_run;
}

TEST(RunActionManagerTest, TestPushBack) {
    RunActionManager* data_run = new RunActionManager();
    data_run->PushBack(new RunActionTest());
    data_run->PushBack(new RunActionTest());
    data_run->PushBack(new RunActionTest());
    delete data_run;
    ASSERT_EQ(RunActionTest::reference_count, 0);
    data_run = new RunActionManager();
    EXPECT_THROW(data_run->PushBack(NULL), MAUS::Exceptions::Exception);
    delete data_run;

    RunActionTest* data_run_action = new RunActionTest();
    data_run = new RunActionManager();
    data_run->PushBack(data_run_action);
    EXPECT_THROW(data_run->PushBack(data_run_action), MAUS::Exceptions::Exception);
    delete data_run;
}

TEST(RunActionManagerTest, TestStartOfRunAction) {
    RunHeader header;
    header.SetRunNumber(10);
    RunActionManager* data_run = new RunActionManager();
    RunActionTest* run_action_1 = new RunActionTest();
    RunActionTest* run_action_2 = new RunActionTest();
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

TEST(RunActionManagerTest, TestEndOfRunAction) {
    RunFooter footer;
    footer.SetRunNumber(10);
    RunActionManager* data_run = new RunActionManager();
    RunActionTest* run_action_1 = new RunActionTest();
    RunActionTest* run_action_2 = new RunActionTest();
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

TEST(RunActionManagerTest, TestSwap) {
    RunActionManager* data_run = new RunActionManager();
    RunActionTest* run_action_1 = new RunActionTest();
    RunActionTest* run_action_2 = new RunActionTest();
    RunActionTest* run_action_3 = new RunActionTest();
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
    EXPECT_THROW(data_run->Swap(run_action_1, run_action_3), MAUS::Exceptions::Exception);
    data_run->PushBack(run_action_3);
    // run_action_3 is already in data_run
    EXPECT_THROW(data_run->Swap(run_action_2, run_action_3), MAUS::Exceptions::Exception);
    delete data_run;
    delete run_action_1;
}

TEST(RunActionManagerTest, TestReferenceCount) {
    EXPECT_EQ(RunActionTest::reference_count, 0);
}
}

