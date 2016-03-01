/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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

#include <cstdlib>
#include <ctime>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>
#include "float.h"
#include "TLorentzVector.h"
#include "TRandom3.h"

#include "gtest/gtest.h"

#include "Recon/Global/PIDVarA.hh"



TEST(PIDVarATestSetUp, TestSetUpAndTearDown) {
	MAUS::recon::global::PIDVarA *testPIDVarA = NULL;
	ASSERT_NO_THROW(testPIDVarA = new MAUS::recon::global::PIDVarA("test",
								       "test"));
	std::string testfile = testPIDVarA->Get_filename();
	std::string testdir = testPIDVarA->Get_directory();
	ASSERT_NO_THROW(delete testPIDVarA);

	EXPECT_EQ(gSystem->Unlink(testfile.c_str()), 0);
        // Unlink dir does not seem to work on some systems
        // suspect NFS mounts or stale handles
        // force system removal

        // std::string rmdirCmd = "rm -fr ";
        // rmdirCmd += testdir.c_str();
        // EXPECT_EQ(system(rmdirCmd.c_str()), 0);
}

class PIDVarATest : public ::testing::Test {

	virtual void SetUp() {
		MAUS::recon::global::PIDVarA testPIDVarA("test", "test");
		testfile = testPIDVarA.Get_filename();
		testdir = testPIDVarA.Get_directory();
		TRandom3 r;
		for (int i = 0; i < 1000; i++) {
		  MAUS::DataStructure::Global::Track* testTrack =
		    new MAUS::DataStructure::Global::Track();
		  MAUS::DataStructure::Global::TrackPoint* tp0 =
		    new MAUS::DataStructure::Global::TrackPoint();
		  MAUS::DataStructure::Global::TrackPoint* tp1 =
		    new MAUS::DataStructure::Global::TrackPoint();
		  tp0->set_detector(MAUS::DataStructure::Global::kTOF0);
		  tp1->set_detector(MAUS::DataStructure::Global::kTOF1);

		  double x0 = 0.0;
		  double y0 = 0.0;
		  double z0 = 0.0;
		  double t0 = r.Gaus(15, 2);
		  TLorentzVector pos0(x0, y0, z0, t0);

		  double x1 = 0.0;
		  double y1 = 0.0;
		  double z1 = 0.0;
		  double t1 = r.Gaus(45, 2);
		  TLorentzVector pos1(x1, y1, z1, t1);

		  tp0->set_position(pos0);
		  tp1->set_position(pos1);
		  tp0->set_mapper_name("MapCppGlobalTrackMatching-Through");
		  tp1->set_mapper_name("MapCppGlobalTrackMatching-Through");
		  testTrack->AddTrackPoint(tp0);
		  testTrack->AddTrackPoint(tp1);
		  testTrack->set_mapper_name("MapCppGlobalTrackMatching-Through");
		  testTracks.push_back(testTrack);
		}
	}

	virtual void TearDown() {
		gSystem->Unlink(testfile.c_str());
		gSystem->Unlink(testdir.c_str());
	}

 public:
	bool fileExists(std::string filename) {
		TFile f(filename.c_str(), "READ");
		return !f.IsZombie();
	}

	bool IsFiniteNumber(double x) {
        return (x <= DBL_MAX && x >= -DBL_MAX);
    }

	std::string testfile;
	std::string testdir;
	TFile *file;
	TH1F *hist;
	std::string histname;
	std::vector<MAUS::DataStructure::Global::Track*> testTracks;
};

TEST_F(PIDVarATest, CreateFileConstructor) {

	ASSERT_TRUE(fileExists(testfile));
}

TEST_F(PIDVarATest, ReadFileConstructor) {

	file = new TFile(testfile.c_str(), "READ");

	ASSERT_TRUE(file);

	ASSERT_FALSE(file->IsZombie());

	ASSERT_NO_THROW(MAUS::recon::global::PIDVarA testPIDVarA(file, "test", 20, 40));

	ASSERT_ANY_THROW(MAUS::recon::global::PIDVarA testPIDVarA(NULL, "test", 20, 40));

	ASSERT_ANY_THROW(MAUS::recon::global::PIDVarA testPIDVarA(file, "sasquatch", 20, 40));
}

/* N.B. if either FillHist or LogL are failing, check the values of min and max
*	 bin in PIDVarA.hh, as if the testTracks fall outside of this range FillHist 
*  and LogL will fail.
*/

TEST_F(PIDVarATest, FillHist) {

	MAUS::recon::global::PIDVarA testPIDVarA("test", "test");
	for (int i = 0; i < 1000; i++) {
		testPIDVarA.Fill_Hist(testTracks[i]);
	}
	testfile = testPIDVarA.Get_filename();
	testdir = testPIDVarA.Get_directory();
	// check histogram exists
	hist = testPIDVarA.Get_hist();
	ASSERT_TRUE(hist);
	// check number of entries
	ASSERT_EQ(1000, hist->GetEntries());
}

TEST_F(PIDVarATest, LogL) {
	// Constructor called within parentheses so that destructor will be
	// called when it goes out of scope
	 {
		MAUS::recon::global::PIDVarA writetestPIDVarA("test", "test");
		for (int i = 0; i < 1000; i++) {
			writetestPIDVarA.Fill_Hist(testTracks[i]);
		}
		testfile = writetestPIDVarA.Get_filename();
		testdir = writetestPIDVarA.Get_directory();
	 }

	MAUS::DataStructure::Global::Track* checkTrack =
			new MAUS::DataStructure::Global::Track();
	MAUS::DataStructure::Global::TrackPoint* ctp0 =
			new MAUS::DataStructure::Global::TrackPoint();
	MAUS::DataStructure::Global::TrackPoint* ctp1 =
			new MAUS::DataStructure::Global::TrackPoint();
	ctp0->set_detector(MAUS::DataStructure::Global::kTOF0);
	ctp1->set_detector(MAUS::DataStructure::Global::kTOF1);

	double x0 = 0.0;
	double y0 = 0.0;
	double z0 = 0.0;
	double t0 = 15.0;
	TLorentzVector cpos0(x0, y0, z0, t0);

	double x1 = 0.0;
	double y1 = 0.0;
	double z1 = 0.0;
	double t1 = 45.0;
	TLorentzVector cpos1(x1, y1, z1, t1);

	ctp0->set_position(cpos0);
	ctp1->set_position(cpos1);
	ctp0->set_mapper_name("MapCppGlobalTrackMatching-Through");
	ctp1->set_mapper_name("MapCppGlobalTrackMatching-Through");
	checkTrack->AddTrackPoint(ctp0);
	checkTrack->AddTrackPoint(ctp1);
	checkTrack->set_mapper_name("MapCppGlobalTrackMatching-Through");

	file = new TFile(testfile.c_str(), "READ");

	ASSERT_TRUE(file);

	ASSERT_FALSE(file->IsZombie());

	ASSERT_NO_THROW(MAUS::recon::global::PIDVarA readtestPIDVarA(file, "test", 20, 40));

	MAUS::recon::global::PIDVarA readtestPIDVarA(file, "test", 20, 40);

	hist = readtestPIDVarA.Get_hist();
	ASSERT_TRUE(hist);
	// check number of entries in histogram
	// NB as destructor was called above, the number of entries in the histogram
	// will be increased by the number of bins (inc over and underflow) as in
	// PIDVarA the behaviour to add one event across all bins is turned on.
	ASSERT_EQ((1000 + hist->GetSize()), hist->GetEntries());
	double prob = readtestPIDVarA.logL(checkTrack);

	ASSERT_LE(prob, 0);
	ASSERT_TRUE(IsFiniteNumber(prob));
}
