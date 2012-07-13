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
#include "src/common_cpp/GlobalsHandling/GlobalsManagerFactory.hh"
#include "src/common_cpp/GlobalsHandling/GlobalsManager.hh"

Json::Value SetupConfig();

namespace MAUS {
// GlobalsManager allocates and deletes MICERun ... so need to cache
// that information somewhere to make it available to the other tests...
class GlobalsManagerTest : public ::testing::Test {
  public:
    GlobalsManagerTest() {
        str = JsonWrapper::JsonToString
                      (*GlobalsManager::GetInstance()->GetConfigurationCards());
        // watch out - this breaks the test order!
        // always force GlobalsManagerFactory to not exist at start
        if (GlobalsManager::HasInstance()) {
            GlobalsManagerFactory::DeleteGlobalsManager();
        }
    }

    ~GlobalsManagerTest() {
        // always force GlobalsManagerFactory to exist at end (i.e. leave self-
        // consistent globals universe)
        if (!GlobalsManager::HasInstance()) {
            GlobalsManagerFactory::InitialiseGlobalsManager(str);
        }
    }

    void SetUp() {}
    void TearDown() {}

    std::string str;

  private:
};

// Also here we check that GetProcessDataManager works
TEST_F(GlobalsManagerTest, TestInitialiseDestruct) {
    // Check that we get an exception if initialising with bad data and that
    // _process is still NULL
    EXPECT_THROW
          (GlobalsManagerFactory::InitialiseGlobalsManager("not json"), Squeal);
    EXPECT_THROW(GlobalsManager::GetInstance(), Squeal);
    
    // Check that we don't get an exception if initialising with good data and
    // that _process is not NULL. We will check the member data initialisation
    // later...
    GlobalsManagerFactory::InitialiseGlobalsManager(str);
    EXPECT_NE(GlobalsManager::GetInstance(),
              static_cast<GlobalsManager*>(NULL));

    // Try deleting
    GlobalsManagerFactory::DeleteGlobalsManager();
    // Check that we now get an exception on delete (already deleted)
    EXPECT_THROW(GlobalsManagerFactory::DeleteGlobalsManager(), Squeal);

    // Check that we now get an exception (the pointer allocated to NULL)
    EXPECT_THROW(GlobalsManager::GetInstance(), Squeal);

    // Check that we can reinitialise okay
    GlobalsManagerFactory::InitialiseGlobalsManager(str);
    GlobalsManager::GetInstance();
    EXPECT_NE(GlobalsManager::GetInstance(), 
              static_cast<GlobalsManager*>(NULL));

    // Clean up
    GlobalsManagerFactory::DeleteGlobalsManager();
}

// Check that HasInstance works
TEST_F(GlobalsManagerTest, TestHasInstance) {
    EXPECT_FALSE(GlobalsManager::HasInstance());
    GlobalsManagerFactory::InitialiseGlobalsManager(str);
    EXPECT_TRUE(GlobalsManager::HasInstance());
    GlobalsManagerFactory::DeleteGlobalsManager();
    EXPECT_FALSE(GlobalsManager::HasInstance());   
}

// Go through each accessor one by one, check they don't return NULL
TEST_F(GlobalsManagerTest, TestAccessors) {
    GlobalsManagerFactory::InitialiseGlobalsManager(str);
    EXPECT_NE(GlobalsManager::GetInstance(), 
              static_cast<GlobalsManager*>(NULL));
    EXPECT_TRUE(false);
    GlobalsManagerFactory::DeleteGlobalsManager();    
}
}

