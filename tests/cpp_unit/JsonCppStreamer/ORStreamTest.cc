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

#include "src/common_cpp/JsonCppStreamer/ORStream.hh"

TEST(ORStreamTest, TestConstructor) {
  orstream os("TestFile.root", "TestTree", "TestTreeTitle", "RECREATE");
  ASSERT_NE(os.m_file, static_cast<void*>(NULL))
    << "Fail: super class not initialised properly."
    << std::endl;
  ASSERT_EQ(os.m_evtCount, 0)
    << "Fail: event counter not initialised properly"
    << std::endl;
  ASSERT_NE(os.m_tree, static_cast<void*>(NULL))
    << "Fail: tree not properly initialised"
    << std::endl;

  // orstream close but dont want to introduce dependence on the close method in unit test
  os.m_file->Close();
  delete os.m_file;
  // delete os.m_tree; // crashes root?! ROOT does this step automatically as part of closing file.
  os.m_file = 0;
  os.m_tree = 0;
  strcpy(os.m_branchName, "");
  os.m_evtCount = 0;
  ///////////////////////////////
}

TEST(ORStreamTest, TestFileOpen) {
  orstream os("TestFile.root", "TestTree", "TestTreeTitle", "RECREATE");
  // orstream close but dont want to introduce dependence on the close method in unit test
  os.m_file->Close();
  delete os.m_file;
  // delete os.m_tree; // crashes root?! ROOT does this step automatically as part of closing file.
  os.m_file = 0;
  os.m_tree = 0;
  strcpy(os.m_branchName, "Test");
  os.m_evtCount = 100;
  //////////////////////////////

  os.open("TestFile.root", "TestTree", "TestTreeTitle", "RECREATE");
  ASSERT_NE(os.m_file, static_cast<void*>(NULL))
    << "Fail: file object not created correctly."
    << std::endl;
  ASSERT_NE(os.m_tree, static_cast<void*>(NULL))
    << "Fail: tree object not created correctly."
    << std::endl;
  ASSERT_EQ(os.m_branchName, std::string(""))
    << "Fail: branch name not reset"
    << std::endl;
  ASSERT_EQ(os.m_evtCount, 0)
    << "Fail: event counter not reset"
    << std::endl;

  // orstream close but dont want to introduce dependence on the close method in unit test
  os.m_file->Close();
  delete os.m_file;
  // delete os.m_tree; // crashes root?! ROOT does this step automatically as part of closing file.
  os.m_file = 0;
  os.m_tree = 0;
  strcpy(os.m_branchName, "");
  os.m_evtCount = 0;
  ///////////////////////////////
}

TEST(ORStreamTest, TestFileClose) {
  orstream os("TestFile.root", "TestTree", "TestTreeTitle", "RECREATE");
  strcpy(os.m_branchName, "HelloWorld");
  os.close();
  ASSERT_EQ(os.m_file, static_cast<void*>(NULL))
    << "Fail: file object not deleted properly."
    << std::endl;
  ASSERT_EQ(os.m_tree, static_cast<void*>(NULL))
    << "Fail: tree object not deleted properly."
    << std::endl;
  ASSERT_EQ(os.m_branchName, std::string(""))
    << "Fail: branch name not reset"
    << std::endl;
}

TEST(ORStreamTest, TestFillEvent) {
  int a = 25;
  int *b = new int(72);
  orstream os("TestFile.root", "TestTree", "TestTreeTitle", "RECREATE");
  os << branchName("TestBranchA") << a;
  os << branchName("TestBranchB") << b;
  os << fillEvent;
  a  = 14;
  *b = 22;
  os << fillEvent;

  ASSERT_EQ(os.m_tree->GetEntriesFast(), 2)
    << "Fail: root tree not filled properly."
    << std::endl;
  ASSERT_EQ(os.m_evtCount, 2)
    << "Fail: event counted not incremented properly."
    << std::endl;

  os.m_tree->Write();
  // orstream close but dont want to introduce dependence on the close method in unit test
  os.m_file->Close();
  delete os.m_file;
  // delete os.m_tree; // crashes root?! ROOT does this step automatically as part of closing file.
  os.m_file = 0;
  os.m_tree = 0;
  strcpy(os.m_branchName, "");
  os.m_evtCount = 0;
  /////////////////////////////////////////

  int * c = new int(0);
  *b = 0;

  TFile f("TestFile.root");
  TTree* tt = reinterpret_cast<TTree*>(f.Get("TestTree"));
  ASSERT_NE(tt, static_cast<void*>(NULL))
    << "Fail: couldn't find tree in the file."
    << std::endl;

  tt->SetBranchAddress("TestBranchA", c);
  tt->SetBranchAddress("TestBranchB", b);

  long nevent = 0;

  while (nevent < tt->GetEntriesFast()) {

    tt->GetEntry(nevent);

    ASSERT_TRUE(*c == 25 || *c == 14)
      << "Fail: incorrect value stored with non-pointer"
      << std::endl;
    ASSERT_TRUE(*b == 72 || *b == 22)
      << "Fail: incorrect value stored with pointer"
      << std::endl;

    ++nevent;
  }
}
