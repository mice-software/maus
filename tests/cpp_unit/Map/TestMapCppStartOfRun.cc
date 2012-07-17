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
#include "json/json.h"

#include "src/common_cpp/DataStructure/RunHeader.hh"
#include "src/common_cpp/DataStructure/RunFooter.hh"
#include "src/common_cpp/GlobalsHandling/RunActionBase.hh"
#include "src/common_cpp/GlobalsHandling/RunActionManager.hh"
#include "src/common_cpp/GlobalsHandling/GlobalsManager.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/map/MapCppStartOfRun/MapCppStartOfRun.hh"

namespace MAUS {
class TestRunAction : public RunActionBase {
  public:
    TestRunAction() {}
    ~TestRunAction() {}
    RunActionBase* Clone() {return new TestRunAction();}
    void StartOfRun(RunHeader* head) {_head_run_number = head->GetRunNumber();}
    void EndOfRun(RunFooter* head) {}

    int _head_run_number;
};

TEST(MapCppStartOfRunTest, ConstructorBirthDeath) {
    MapCppStartOfRun my_map;
    EXPECT_TRUE(my_map.birth(""));
    EXPECT_TRUE(my_map.death());
}

TEST(MapCppStartOfRunTest, ProcessBadInputTest) {
    MapCppStartOfRun my_map;
    EXPECT_TRUE(my_map.birth(""));
    Json::Value proc = JsonWrapper::StringToJson(my_map.process(""));
    EXPECT_EQ(proc["errors"].type(), Json::objectValue);
    proc = JsonWrapper::StringToJson(my_map.process("{}"));
    EXPECT_EQ(proc["errors"].type(), Json::objectValue);
}

TEST(MapCppStartOfRunTest, ProcessGoodInputTest) {
    MapCppStartOfRun my_map;
    TestRunAction* my_action = new TestRunAction();
    GlobalsManager::GetInstance()->GetRunActionManager()->
                                                  PushBack(my_action);
    my_map.process("{\"run_number\":909}");
    EXPECT_EQ(my_action->_head_run_number, 909);    
        
}
}
