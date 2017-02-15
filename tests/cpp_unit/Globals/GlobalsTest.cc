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

#include <typeinfo>

#include "gtest/gtest.h"

#include "src/legacy/Interface/MICERun.hh"
#include "src/legacy/Interface/dataCards.hh"
#include "src/legacy/Config/MiceModule.hh"

#include "src/common_cpp/Utils/Exception.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/RunActionManager.hh"
#include "src/common_cpp/Utils/Globals.hh"

#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"

#include "src/common_cpp/Globals/GlobalsManager.hh"

Json::Value SetupConfig();

// tests Globals and Globals

namespace MAUS {
// Globals allocates and deletes MICERun ... so need to cache
// that information somewhere to make it available to the other tests...
class GlobalsTest : public ::testing::Test {
  public:
    GlobalsTest() {
        str = JsonWrapper::JsonToString
                      (*Globals::GetInstance()->GetConfigurationCards());
        // watch out - this breaks the test order!
        // always force GlobalsManager to not exist at start
        root = MiceModule::deepCopy
                                  (*Globals::GetMonteCarloMiceModules(), false);
        if (Globals::HasInstance()) {
            GlobalsManager::DeleteGlobals();
        }
    }

    ~GlobalsTest() {
        // always force GlobalsManager to exist at end (i.e. leave self-
        // consistent globals universe)
        if (!Globals::HasInstance()) {
            GlobalsManager::InitialiseGlobals(str);
        }
        GlobalsManager::SetMonteCarloMiceModules(root);
    }

    void SetUp() {}
    void TearDown() {}

    std::string str;
  private:
    MiceModule* root;
};

// Also here we check that GetProcessDataManager works
TEST_F(GlobalsTest, TestInitialiseDestruct) {
    // Check that we get an exception if initialising with bad data and that
    // _process is still NULL
    EXPECT_THROW
          (GlobalsManager::InitialiseGlobals("not json"), MAUS::Exceptions::Exception);
    EXPECT_THROW(Globals::GetInstance(), MAUS::Exceptions::Exception);

    // Check that we don't get an exception if initialising with good data and
    // that _process is not NULL. We will check the member data initialisation
    // later...
    GlobalsManager::InitialiseGlobals(str);
    EXPECT_NE(Globals::GetInstance(),
              static_cast<Globals*>(NULL));

    // Try deleting
    GlobalsManager::DeleteGlobals();
    // Check that we now get an exception on delete (already deleted)
    EXPECT_THROW(GlobalsManager::DeleteGlobals(), MAUS::Exceptions::Exception);

    // Check that we now get an exception (the pointer allocated to NULL)
    EXPECT_THROW(Globals::GetInstance(), MAUS::Exceptions::Exception);

    // Check that we can reinitialise okay
    GlobalsManager::InitialiseGlobals(str);
    Globals::GetInstance();
    EXPECT_NE(Globals::GetInstance(),
              static_cast<Globals*>(NULL));

    // Clean up
    GlobalsManager::DeleteGlobals();
}

// Check that HasInstance works
TEST_F(GlobalsTest, TestHasInstance) {
    EXPECT_FALSE(Globals::HasInstance());
    GlobalsManager::InitialiseGlobals(str);
    EXPECT_TRUE(Globals::HasInstance());
    GlobalsManager::DeleteGlobals();
    EXPECT_FALSE(Globals::HasInstance());
}

// check that we can Get and Set okay. Use test as sample data - note
// Globals now owns memory allocated by test (if not pointing at NULL).
template <class TEMP>
void test_accessors(TEMP* GetFunction(),
                    void SetFunction(TEMP* tmp),
                    TEMP* test) {
    TEMP* my_temp = GetFunction();
    // check that default is non-NULL
    ASSERT_NE(my_temp, static_cast<TEMP*>(NULL)) << typeid(test).name();
    // check that we can reallocate to NULL
    SetFunction(NULL);
    EXPECT_EQ(GetFunction(), static_cast<TEMP*>(NULL)) << typeid(test).name();
    // check that we can reallocate from NULL (no double free)
    SetFunction(test);
    EXPECT_EQ(GetFunction(), static_cast<TEMP*>(test)) << typeid(test).name();
    // leave as NULL (check DeleteGlobals() copes okay in this case)
    SetFunction(NULL);
}


// Go through each accessor one by one, check they don't return NULL
TEST_F(GlobalsTest, TestAccessors) {
    GlobalsManager::InitialiseGlobals(str);
    EXPECT_NE(Globals::GetInstance(),
              static_cast<Globals*>(NULL));
    test_accessors(Globals::GetRunActionManager,
                   GlobalsManager::SetRunActionManager,
                   static_cast<RunActionManager*>(NULL));
    test_accessors(Globals::GetLegacyCards,
                   GlobalsManager::SetLegacyCards, new dataCards());
    test_accessors(Globals::GetMonteCarloMiceModules,
                   GlobalsManager::SetMonteCarloMiceModules,
                   new MiceModule("Test.dat"));
    test_accessors(Globals::GetReconstructionMiceModules,
                   GlobalsManager::SetReconstructionMiceModules,
                   new MiceModule());
/*
    test_accessors(Globals::GetBTFieldConstructor,
                   GlobalsManager::SetBTFieldConstructor,
                   new BTFieldConstructor());
    test_accessors(Globals::GetErrorHandler,
                   GlobalsManager::SetErrorHandler,
                   static_cast<CppErrorHandler*>(NULL));
    test_accessors(Globals::GetConfigurationCards,
                   GlobalsManager::SetConfigurationCards,
                   new Json::Value());
    test_accessors(Globals::GetGeant4Manager,
                   GlobalsManager::SetGeant4Manager,
                   static_cast<MAUSGeant4Manager*>(NULL));
*/
    GlobalsManager::DeleteGlobals();
}

TEST_F(GlobalsTest, TestResetFields) {
    std::string mod_name = std::string(getenv("MAUS_ROOT_DIR"))+
                           std::string("/tests/cpp_unit/Globals/QuadTest.dat");
    // initialise; check field = 0
    GlobalsManager::InitialiseGlobals(str);
    double point[] = {500., 0., 0., 0.};
    double field[] = {0., 0., 0., 0., 0., 0.};
    Globals::GetMCFieldConstructor()->GetFieldValue(point, field);
    EXPECT_DOUBLE_EQ(field[1], 0.);
    // now reset fields; check field != 0
    MiceModule* test_mod = new MiceModule(mod_name);
    GlobalsManager::SetMonteCarloMiceModules(test_mod);
    Globals::GetMCFieldConstructor()->GetFieldValue(point, field);
    // check field value
    EXPECT_DOUBLE_EQ(fabs(field[0]), 1.);
    EXPECT_DOUBLE_EQ(fabs(field[1]), 2.);
    EXPECT_DOUBLE_EQ(fabs(field[2]), 3.);
}

TEST_F(GlobalsTest, TestResetGeant4Geometry) {
// Check that Virtual Planes are rebuilt okay
// Check that Materials and volumes are rebuilt okay
// Check that sensitive detectors are rebuilt okay
}

// Version number is tested in PyGlobals
}

