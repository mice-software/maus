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

#include "TFile.h"
#include "TTree.h"

#include "gtest/gtest.h"

#include "src/common_cpp/JsonCppStreamer/IRStream.hh"
#include "src/common_cpp/JsonCppStreamer/OneArgManip.hh"

TEST(IRStreamTest, TestConstructor) {
  TFile f("TestFile.root", "RECREATE");
  TTree t("TestTree", "treetitle");
  t.Write();
  f.Close();
  // explicitly allocate memory so we can run destructor explicitly
  irstream* is = new irstream("TestFile.root", "TestTree");

  ASSERT_EQ(is->m_evtCount, 0)
    << "Fail: event counter not initialised properly"
    << std::endl;
  ASSERT_NE(is->m_file, static_cast<void*>(NULL))
    << "Fail: file object not initialised properly"
    << std::endl;
  ASSERT_NE(is->m_tree, static_cast<void*>(NULL))
    << "Fail: tree object not initialised properly"
    << std::endl;
  is->close();
  delete is;
}

TEST(IRStreamTest, TestFileOpen) {
  TFile f("TestFile.root", "RECREATE");
  TTree t("TestTree", "treetitle");
  t.Write();
  f.Close();
  irstream is("TestFile.root", "TestTree");
  // irstream close but dont want to introduce dependence on the close method in unit test
  is.m_file->Close();
  strcpy(is.m_branchName, "Test");
  delete is.m_file;
  // delete m_tree; // crashes root?! ROOT must do this step automatically as part of closing file
  is.m_file = 0;
  is.m_tree = 0;
  is.m_evtCount = 100;
  /////////////////////////////////

  is.open("TestFile.root", "TestTree");

  ASSERT_EQ(is.m_evtCount, 0)
    << "Fail: event counter not reset"
    << std::endl;
  ASSERT_NE(is.m_file, static_cast<void*>(NULL))
    << "Fail: file object not initialised properly"
    << std::endl;
  ASSERT_NE(is.m_tree, static_cast<void*>(NULL))
    << "Fail: tree object not initialised properly"
    << std::endl;
  ASSERT_NE(is.m_branchName, "")
    << "Fail: branch name not reset"
    << std::endl;

  // irstream close but dont want to introduce dependence on the close method in unit test
  is.m_file->Close();
  strcpy(is.m_branchName, "");
  delete is.m_file;
  // delete m_tree; // crashes root?! ROOT does this step automatically as part of closing file
  is.m_file = 0;
  is.m_tree = 0;
  /////////////////////////////////
}

TEST(IRStreamTest, TestFileClose) {
  TFile f("TestFile.root", "RECREATE");
  TTree t("TestTree", "treetitle");
  t.Write();
  f.Close();
  irstream is("TestFile.root", "TestTree");

  is.close();

  ASSERT_NE(is.m_branchName, "")
    << "Fail: did not reset the branch name."
    << std::endl;
  ASSERT_EQ(is.m_file, static_cast<void*>(NULL))
    << "Fail: did not delete the file object."
    << std::endl;
  ASSERT_EQ(is.m_tree, static_cast<void*>(NULL))
    << "Fail: did not delete the tree object"
    << std::endl;
}

TEST(IRStreamTest, TestReadEvent ) {
  TFile f("TestFile.root", "RECREATE");
  TTree t("TestTree", "treetitle");
  Int_t* a = new Int_t(17);
  Int_t* b = new Int_t(23);
  t.Branch("testA", a, "testA/I");
  t.Branch("testB", b, "testB/I");
  t.Fill();
  *a = 26;
  *b = 19;
  t.Fill();
  t.Write();
  f.Close();

  a = new(a) Int_t(0);
  delete b;

  irstream is("TestFile.root", "TestTree");
  int c = 0;
  is >> branchName("testA") >> a;
  is >> branchName("testB") >> c;

  int evtCount = 0;
  while (is >> readEvent) {
    ++evtCount;
    ASSERT_EQ(evtCount, is.m_evtCount)
      << "Fail: didn't increment the read event counter properly."
      << std::endl;
    ASSERT_TRUE(*a == 17 || *a == 26)
      << "Fail: didn't read in the correct values from pointer."
      << std::endl;
    ASSERT_TRUE(c == 23 ||  c == 19)
      << "Fail: didn't read in the correct values from non-pointer."
      << std::endl;
  }

  // irstream close but dont want to introduce dependence on the close method in unit test
  is.m_file->Close();
  strcpy(is.m_branchName, "");
  delete is.m_file;
  // delete m_tree; // crashes root?! ROOT does this step automatically as part of closing file
  is.m_file = 0;
  is.m_tree = 0;
  /////////////////////////////////
  delete a;
}
