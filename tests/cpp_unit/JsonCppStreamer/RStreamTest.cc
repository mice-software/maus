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

class temprstream : public rstream {
 public:
  temprstream(const char* a,
	      const char* b) : rstream(a, b) {}
    void close() {m_file->Close();}
 private:
  FRIEND_TEST(RStreamTest, TestConstructor);
  FRIEND_TEST(RStreamTest, TestIsOpen);
  FRIEND_TEST(RStreamTest, TestSetBranch);
  FRIEND_TEST(RStreamTest, TestAttachBranch);
};

TEST(RStreamTest, TestConstructor) {
    temprstream a("TestFile.root", "RECREATE");
    ASSERT_NE(a.m_file,       static_cast<void*>(NULL))
      << "Fail: file object not correctly created."
      << std::endl;
    ASSERT_NE(a.m_branchName, static_cast<void*>(NULL))
      << "Fail: branch name memory not correctly allocated"
      << std::endl;
    ASSERT_NE(a.m_branchName, "")
      << "Fail. branch name not set to default value."
      << std::endl;
}

TEST(RStreamTest, TestIsOpen) {
    temprstream a("TestFile.root", "RECREATE");
    ASSERT_TRUE(a.is_open())
      << "Fail: reporting file closed when open"
      << std::endl;
    TFile * f = a.m_file;
    a.m_file = 0;
    ASSERT_FALSE(a.is_open())
      << "Fail: reporting file open when null pointer."
      << std::endl;
    a.m_file = f;
    a.close();
    ASSERT_FALSE(a.is_open())
      << "Fail: reporting file open when closed."
      << std::endl;
}

TEST(RStreamTest, TestSetBranch) {
    temprstream a("TestFile", "RECREATE");
    rstream::setBranch(a, "TestingBranch");

    ASSERT_EQ(std::string(a.m_branchName), "TestingBranch")
      << "Fail: Branch name not set correctly."
      << std::endl;
}

TEST(RStreamTest, TestAttachBranch) {
    temprstream t("TestFile.root", "RECREATE");

    // test non pointer version
    ///////////////////////////////////////////
    t.m_tree = new TTree("TestTree", "The Tree");

    int a = 9;
    std::vector<int> b;
    b.push_back(14);
    b.push_back(27);
    t.attachBranch(a, false, false);
    ASSERT_EQ(t.m_tree->GetNbranches(), 0 )
      << "Fail: Should return without doing anything if branch not set."
      << std::endl;

    strcpy(t.m_branchName, "TestBranch1");
    t.attachBranch(a, false, false);
    ASSERT_EQ(t.m_tree->GetNbranches(), 0 )
      << "Fail: Should return without doing anything if not allowed to create new branches."
      << std::endl;

    strcpy(t.m_branchName, "TestBranch1");
    t.attachBranch(a, false, true);
    ASSERT_EQ(t.m_tree->GetNbranches(), 1 )
      << "Fail: Should have created single pointer branch."
      << std::endl;

    strcpy(t.m_branchName, "TestBranch2");
    t.attachBranch(b, true, false);
    ASSERT_EQ(t.m_tree->GetNbranches(), 1 )
      << "Fail: Should have not found branch."
      << std::endl;

    strcpy(t.m_branchName, "TestBranch2");
    t.attachBranch(b, true, true);
    ASSERT_EQ(t.m_tree->GetNbranches(), 2 )
      << "Fail: Should have created double pointer branch."
      << std::endl;

    int c = 10;
    std::vector<int> d;
    d.push_back(13);
    d.push_back(22);

    strcpy(t.m_branchName, "TestBranch1");
    t.attachBranch(c, false, true);
    ASSERT_EQ(t.m_tree->GetNbranches(), 2 )
      << "Fail: Should have reset single pointer branch."
      << std::endl;

    strcpy(t.m_branchName, "TestBranch2");
    t.attachBranch(d, true, true);
    ASSERT_EQ(t.m_tree->GetNbranches(), 2 )
      << "Fail: Should have reset double pointer branch."
      << std::endl;

    // test pointer version
    ///////////////////////////////////////////
    t.m_tree->Delete();
    t.m_tree = new TTree("TestTree", "The Tree");
    int *e = new int(9);
    std::vector<int>* f = new std::vector<int>();
    f->push_back(14);
    f->push_back(27);
    t.attachBranch(e, false, false);
    ASSERT_EQ(t.m_tree->GetNbranches(), 0 )
      << "Fail: Should return without doing anything if branch not set."
      << std::endl;

    strcpy(t.m_branchName, "TestBranch1");
    t.attachBranch(e, false, false);
    ASSERT_EQ(t.m_tree->GetNbranches(), 0 )
      << "Fail: Should return without doing anything if not allowed to create new branches."
      << std::endl;

    strcpy(t.m_branchName, "TestBranch1");
    t.attachBranch(e, false, true);
    ASSERT_EQ(t.m_tree->GetNbranches(), 1 )
      << "Fail: Should have created single pointer branch."
      << std::endl;

    strcpy(t.m_branchName, "TestBranch2");
    t.attachBranch(f, true, false);
    ASSERT_EQ(t.m_tree->GetNbranches(), 1 )
      << "Fail: Should have not found branch."
      << std::endl;

    strcpy(t.m_branchName, "TestBranch2");
    t.attachBranch(f, true, true);
    ASSERT_EQ(t.m_tree->GetNbranches(), 2 )
      << "Fail: Should have created double pointer branch."
      << std::endl;

    int* g = new int(10);
    std::vector<int>* h = new std::vector<int>();
    h->push_back(13);
    h->push_back(22);

    strcpy(t.m_branchName, "TestBranch1");
    t.attachBranch(g, false, true);
    ASSERT_EQ(t.m_tree->GetNbranches(), 2 )
      << "Fail: Should have reset single pointer branch."
      << std::endl;


    strcpy(t.m_branchName, "TestBranch2");
    t.attachBranch(h, true, true);
    ASSERT_EQ(t.m_tree->GetNbranches(), 2 )
      << "Fail: Should have reset double pointer branch."
      << std::endl;

    t.m_file->Close();
    delete e;
    e = 0;
    delete f;
    f = 0;
    delete g;
    g = 0;
    delete h;
    h = 0;
}
