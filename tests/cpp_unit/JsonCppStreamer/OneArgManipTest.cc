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

#include <TFile.h>
#include <TTree.h>

#include <cstring>
#include "gtest/gtest.h"

#include "src/common_cpp/JsonCppStreamer/RStream.hh"
#include "src/common_cpp/JsonCppStreamer/OneArgManip.hh"

class tmprstream : public rstream {
 public:
  tmprstream(const char* a,
	     const char* b) : rstream(a, b) {}
    void close() {m_file->Close();}
    bool operator!=(tmprstream& rs) {
      return &rs != this;
    }
    void operator=(const tmprstream& rs) {
      m_file = rs.m_file;
      snprintf(m_branchName, sizeof(m_branchName), rs.m_branchName);
      m_tree = rs.m_tree;
    }
 private:
  FRIEND_TEST(OneArgManipTest, TestOperator);
};


bool functionPointerPred(rstream& (*pointer1)(rstream&, const char*),
			 rstream& (*pointer2)(rstream&, const char*)) {
  return pointer1 == pointer2;
}

TEST(OneArgManipTest, TestConstructor ) {
    typedef rstream& (*manip_pointer)(rstream&, const char*);

    manip_pointer p = &rstream::setBranch;
    oneArgManip<const char*>* a = new oneArgManip<const char*>(p, "TestConstructor");


    ASSERT_PRED2(functionPointerPred, a->m_manip, p)
      << "Fail: manip function pointers not initialised properly"
      << std::endl;
    ASSERT_EQ(a->m_value, "TestConstructor")
      << "Fail: manip value not initialised properly"
      << std::endl;

    delete a;
    a = 0;
}

bool charArrayPred(const char* a, const char* b) {
  return !strcmp(a, b);
}

TEST(OneArgManipTest, TestOperator ) {
    typedef rstream& (*manip_pointer)(rstream&, const char*);

    manip_pointer p = &rstream::setBranch;
    oneArgManip<const char*> a(p, "TestOperator");

    tmprstream r1("TestFile", "RECREATE");
    tmprstream* r2 = reinterpret_cast<tmprstream*>(&a(r1));


    ASSERT_FALSE(*r2 != r1)
      << "Fail: rstream object not properly returned."
      << std::endl;
    ASSERT_PRED2(charArrayPred, r1.m_branchName, "TestOperator")
      << "Fail: manipulator value not passed properly"
      << std::endl;
}

TEST(OneArgManipTest, TestBranchName ) {

  oneArgManip<const char*>* a = branchName("TestBranchName");

  ASSERT_NE(a, static_cast<void*>(NULL))
    << "Fail: failed to construct a oneArgManip object."
    << std::endl;
}
