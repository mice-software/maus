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
#include "src/legacy/Interface/MICERun.hh"

#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/GlobalsHandling/ProcessDataManager.hh"

Json::Value SetupConfig();

namespace MAUS {
MICERun a_test_run; // gak MICERun

// PerProcessDataManager allocates and deletes MICERun ... so need to cache
// that information somewhere to make it available to the other tests...
class PerProcessDataManagerTest : public ::testing::Test {
  public:
    PerProcessDataManagerTest() {
        a_test_run.miceModule = MICERun::getInstance()->miceModule;
        a_test_run.btFieldConstructor = MICERun::getInstance()->btFieldConstructor;
        a_test_run.jsonConfiguration =  MICERun::getInstance()->jsonConfiguration;
        str = JsonWrapper::JsonToString(*a_test_run.jsonConfiguration);
    }

    ~PerProcessDataManagerTest() {
        MICERun::getInstance()->miceModule = a_test_run.miceModule;
        MICERun::getInstance()->btFieldConstructor = a_test_run.btFieldConstructor;
        MICERun::getInstance()->jsonConfiguration = a_test_run.jsonConfiguration;
    }

    void SetUp() {}
    void TearDown() {}

    std::string str;

  private:
};

// Also here we check that GetProcessDataManager works
TEST_F(PerProcessDataManagerTest, TestInitialiseDestruct) {
    // Check that we get an exception if initialising with bad data and that
    // _process is still NULL
    EXPECT_THROW(PerProcessDataManager::Initialise("not json"), Squeal);
    EXPECT_THROW(PerProcessDataManager::GetInstance(), Squeal);
    
    // Check that we don't get an exception if initialising with good data and
    // that _process is not NULL. We will check the member data initialisation
    // later...
    PerProcessDataManager::Initialise(str);
    EXPECT_NE(PerProcessDataManager::GetInstance(), 
              static_cast<PerProcessDataManager*>(NULL));

    // Try deleting
    delete PerProcessDataManager::GetInstance();

    // Check that we now get an exception (the pointer allocated to NULL)
    EXPECT_THROW(PerProcessDataManager::GetInstance(), Squeal);

    // Check that we can reinitialise okay
    PerProcessDataManager::Initialise(str);
    PerProcessDataManager::GetInstance();
    EXPECT_NE(PerProcessDataManager::GetInstance(), 
              static_cast<PerProcessDataManager*>(NULL));

    // Clean up
    delete PerProcessDataManager::GetInstance();
}

// Check that HasInstance works
TEST_F(PerProcessDataManagerTest, TestHasInstance) {
    EXPECT_FALSE(PerProcessDataManager::HasInstance());
    PerProcessDataManager::Initialise(str);
    EXPECT_TRUE(PerProcessDataManager::HasInstance());
    delete PerProcessDataManager::GetInstance();
    EXPECT_FALSE(PerProcessDataManager::HasInstance());   
}

// Go through each accessor one by one, check they don't return NULL
TEST_F(PerProcessDataManagerTest, TestAccessors) {
    PerProcessDataManager::Initialise(str);
    EXPECT_NE(PerProcessDataManager::GetInstance(), 
              static_cast<PerProcessDataManager*>(NULL));
    
}
}

