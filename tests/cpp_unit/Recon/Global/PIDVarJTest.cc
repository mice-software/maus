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

#include "Recon/Global/PIDVarJ.hh"



TEST(PIDVarJTestSetUp, TestSetUpAndTearDown) {
	MAUS::recon::global::PIDVarJ *testPIDVarJ = NULL;
	ASSERT_NO_THROW(testPIDVarJ = new MAUS::recon::global::PIDVarJ("test",
								       "test"));
	std::string testfile = testPIDVarJ->Get_filename();
	std::string testdir = testPIDVarJ->Get_directory();
	ASSERT_NO_THROW(delete testPIDVarJ);

	EXPECT_EQ(gSystem->Unlink(testfile.c_str()), 0);
        // Unlink dir does not seem to work on some systems
        // suspect NFS mounts or stale handles
        // force system removal
        // std::string rmdirCmd = "rm -fr ";
        // rmdirCmd += testdir.c_str();
        // EXPECT_EQ(system(rmdirCmd.c_str()), 0);
}

class PIDVarJTest : public ::testing::Test {

	virtual void SetUp() {
		MAUS::recon::global::PIDVarJ testPIDVarJ("test", "test");
		testfile = testPIDVarJ.Get_filename();
		testdir = testPIDVarJ.Get_directory();
		TRandom3 r;
		for (int i = 0; i < 1000; i++) {
		  MAUS::DataStructure::Global::Track* testTrack =
		    new MAUS::DataStructure::Global::Track();
		  MAUS::DataStructure::Global::TrackPoint* tpTracker =
		    new MAUS::DataStructure::Global::TrackPoint();
		  tpTracker->set_detector(MAUS::DataStructure::Global::kTracker0_1);
		  MAUS::DataStructure::Global::TrackPoint* tpCkovB =
		    new MAUS::DataStructure::Global::TrackPoint();

		  double num_PES = r.Gaus(30, 2);
		  tpCkovB->set_num_photoelectrons(num_PES);

		  double px = 0.0;
		  double py = 0.0;
		  double pz = 200.0;
		  double E = 226.0;
		  TLorentzVector tpTrackerMom(px, py, pz, E);

		  tpTracker->set_momentum(tpTrackerMom);
		  tpTracker->set_mapper_name("MapCppGlobalTrackMatching-Through");
		  testTrack->AddTrackPoint(tpTracker);
		  tpCkovB->set_mapper_name("MapCppGlobalTrackMatching-Through");
		  tpCkovB->set_detector(MAUS::DataStructure::Global::kCherenkovB);
		  testTrack->AddTrackPoint(tpCkovB);
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
	TH2F *hist;
	std::string histname;
	std::vector<MAUS::DataStructure::Global::Track*> testTracks;
};

TEST_F(PIDVarJTest, CreateFileConstructor) {

	ASSERT_TRUE(fileExists(testfile));
}

TEST_F(PIDVarJTest, ReadFileConstructor) {

	file = new TFile(testfile.c_str(), "READ");

	ASSERT_TRUE(file);

	ASSERT_FALSE(file->IsZombie());

	ASSERT_NO_THROW(MAUS::recon::global::PIDVarJ testPIDVarJ(file, "test", 50, 350, 0, 1000));

	ASSERT_ANY_THROW(MAUS::recon::global::PIDVarJ testPIDVarJ(NULL, "test", 50, 350, 0, 1000));

	ASSERT_ANY_THROW(MAUS::recon::global::PIDVarJ testPIDVarJ(file, "sasquatch", 50, 350, 0, 1000));
}

/* N.B. if either FillHist or LogL are failing, check the values of min and max
*	 bins in PIDVarJ.hh, as if the testTracks fall outside of this range FillHist 
*  and LogL will fail.
*/

TEST_F(PIDVarJTest, FillHist) {

	MAUS::recon::global::PIDVarJ testPIDVarJ("test", "test");
	for (int i = 0; i < 1000; i++) {
		testPIDVarJ.Fill_Hist(testTracks[i]);
	}
	testfile = testPIDVarJ.Get_filename();
	testdir = testPIDVarJ.Get_directory();
	// check histogram exists
	hist = testPIDVarJ.Get_hist();
	ASSERT_TRUE(hist);
	// check number of entries
	ASSERT_EQ(1000, hist->GetEntries());
}

TEST_F(PIDVarJTest, LogL) {
	// Constructor called within parentheses so that destructor will be
	// called when it goes out of scope
	 {
		MAUS::recon::global::PIDVarJ writetestPIDVarJ("test", "test");
		for (int i = 0; i < 1000; i++) {
			writetestPIDVarJ.Fill_Hist(testTracks[i]);
		}
		testfile = writetestPIDVarJ.Get_filename();
		testdir = writetestPIDVarJ.Get_directory();
	 }

	MAUS::DataStructure::Global::Track* checkTrack =
			new MAUS::DataStructure::Global::Track();
	MAUS::DataStructure::Global::TrackPoint* ctpTracker =
	                new MAUS::DataStructure::Global::TrackPoint();
	ctpTracker->set_detector(MAUS::DataStructure::Global::kTracker0_1);

	double px = 0.0;
	double py = 0.0;
	double pz = 200.0;
	double E = 226.0;
	TLorentzVector ctpTrackerMom(px, py, pz, E);

	ctpTracker->set_momentum(ctpTrackerMom);

	MAUS::DataStructure::Global::TrackPoint* ctpCkovB =
	  new MAUS::DataStructure::Global::TrackPoint();

	double num_PES = 30;
	ctpCkovB->set_num_photoelectrons(num_PES);

	ctpCkovB->set_detector(MAUS::DataStructure::Global::kCherenkovB);
	ctpCkovB->set_mapper_name("MapCppGlobalTrackMatching-Through");

	checkTrack->AddTrackPoint(ctpCkovB);
	ctpTracker->set_mapper_name("MapCppGlobalTrackMatching-Through");
	checkTrack->AddTrackPoint(ctpTracker);
	checkTrack->set_mapper_name("MapCppGlobalTrackMatching-Through");

	file = new TFile(testfile.c_str(), "READ");

	ASSERT_TRUE(file);

	ASSERT_FALSE(file->IsZombie());

	ASSERT_NO_THROW(MAUS::recon::global::PIDVarJ readtestPIDVarJ(file, "test", 50, 350, 0, 140));

	MAUS::recon::global::PIDVarJ readtestPIDVarJ(file, "test", 50, 350, 0, 140);

	hist = readtestPIDVarJ.Get_hist();
	ASSERT_TRUE(hist);
	// check number of entries in histogram
	// NB as destructor was called above, the number of entries in the histogram
	// will be increased by the number of bins (inc over and underflow) as in
	// PIDVarJ the behaviour to add one event across all bins is turned on.
	ASSERT_EQ((1000 + hist->GetSize()), hist->GetEntries());
	double prob = readtestPIDVarJ.logL(checkTrack);

	ASSERT_LE(prob, 0);
	ASSERT_TRUE(IsFiniteNumber(prob));
}
