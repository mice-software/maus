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

#include "Recon/Global/PIDVarI.hh"



TEST(PIDVarITestSetUp, TestSetUpAndTearDown) {
	MAUS::recon::global::PIDVarI *testPIDVarI = NULL;
	ASSERT_NO_THROW(testPIDVarI = new MAUS::recon::global::PIDVarI("test",
								       "test"));
	std::string testfile = testPIDVarI->Get_filename();
	std::string testdir = testPIDVarI->Get_directory();
	ASSERT_NO_THROW(delete testPIDVarI);

	EXPECT_EQ(gSystem->Unlink(testfile.c_str()), 0);
        // Unlink dir does not seem to work on some systems
        // suspect NFS mounts or stale handles
        // force system removal
        // std::string rmdirCmd = "rm -fr ";
        // rmdirCmd += testdir.c_str();
        // EXPECT_EQ(system(rmdirCmd.c_str()), 0);
}

class PIDVarITest : public ::testing::Test {

	virtual void SetUp() {
		MAUS::recon::global::PIDVarI testPIDVarI("test", "test");
		testfile = testPIDVarI.Get_filename();
		testdir = testPIDVarI.Get_directory();
		TRandom3 r;
		for (int i = 0; i < 1000; i++) {
		  MAUS::DataStructure::Global::Track* testTrack =
		    new MAUS::DataStructure::Global::Track();
		  MAUS::DataStructure::Global::TrackPoint* tpTracker =
		    new MAUS::DataStructure::Global::TrackPoint();
		  tpTracker->set_detector(MAUS::DataStructure::Global::kTracker0_1);
		  MAUS::DataStructure::Global::TrackPoint* tpCkovA =
		    new MAUS::DataStructure::Global::TrackPoint();

		  double num_PES = r.Gaus(30, 2);
		  tpCkovA->set_num_photoelectrons(num_PES);

		  double px = 0.0;
		  double py = 0.0;
		  double pz = 200.0;
		  double E = 226.0;
		  TLorentzVector tpTrackerMom(px, py, pz, E);

		  tpTracker->set_momentum(tpTrackerMom);
		  tpTracker->set_mapper_name("MapCppGlobalTrackMatching-Through");
		  testTrack->AddTrackPoint(tpTracker);
		  tpCkovA->set_mapper_name("MapCppGlobalTrackMatching-Through");
		  tpCkovA->set_detector(MAUS::DataStructure::Global::kCherenkovA);
		  testTrack->AddTrackPoint(tpCkovA);
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

TEST_F(PIDVarITest, CreateFileConstructor) {

	ASSERT_TRUE(fileExists(testfile));
}

TEST_F(PIDVarITest, ReadFileConstructor) {

	file = new TFile(testfile.c_str(), "READ");

	ASSERT_TRUE(file);

	ASSERT_FALSE(file->IsZombie());

	ASSERT_NO_THROW(MAUS::recon::global::PIDVarI testPIDVarI(file, "test", 50, 350, 0, 1000));

	ASSERT_ANY_THROW(MAUS::recon::global::PIDVarI testPIDVarI(NULL, "test", 50, 350, 0, 1000));

	ASSERT_ANY_THROW(MAUS::recon::global::PIDVarI testPIDVarI(file, "sasquatch", 50, 350, 0, 1000));
}

/* N.B. if either FillHist or LogL are failing, check the values of min and max
*	 bins in PIDVarI.hh, as if the testTracks fall outside of this range FillHist 
*  and LogL will fail.
*/

TEST_F(PIDVarITest, FillHist) {

	MAUS::recon::global::PIDVarI testPIDVarI("test", "test");
	for (int i = 0; i < 1000; i++) {
		testPIDVarI.Fill_Hist(testTracks[i]);
	}
	testfile = testPIDVarI.Get_filename();
	testdir = testPIDVarI.Get_directory();
	// check histogram exists
	hist = testPIDVarI.Get_hist();
	ASSERT_TRUE(hist);
	// check number of entries
	ASSERT_EQ(1000, hist->GetEntries());
}

TEST_F(PIDVarITest, LogL) {
	// Constructor called within parentheses so that destructor will be
	// called when it goes out of scope
	 {
		MAUS::recon::global::PIDVarI writetestPIDVarI("test", "test");
		for (int i = 0; i < 1000; i++) {
			writetestPIDVarI.Fill_Hist(testTracks[i]);
		}
		testfile = writetestPIDVarI.Get_filename();
		testdir = writetestPIDVarI.Get_directory();
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

	MAUS::DataStructure::Global::TrackPoint* ctpCkovA =
	  new MAUS::DataStructure::Global::TrackPoint();

	double num_PES = 30;
	ctpCkovA->set_num_photoelectrons(num_PES);

	ctpCkovA->set_detector(MAUS::DataStructure::Global::kCherenkovA);
	ctpCkovA->set_mapper_name("MapCppGlobalTrackMatching-Through");

	checkTrack->AddTrackPoint(ctpCkovA);
	ctpTracker->set_mapper_name("MapCppGlobalTrackMatching-Through");
	checkTrack->AddTrackPoint(ctpTracker);
	checkTrack->set_mapper_name("MapCppGlobalTrackMatching-Through");

	file = new TFile(testfile.c_str(), "READ");

	ASSERT_TRUE(file);

	ASSERT_FALSE(file->IsZombie());

	ASSERT_NO_THROW(MAUS::recon::global::PIDVarI readtestPIDVarI(file, "test", 50, 350, 0, 140));

	MAUS::recon::global::PIDVarI readtestPIDVarI(file, "test", 50, 350, 0, 140);

	hist = readtestPIDVarI.Get_hist();
	ASSERT_TRUE(hist);
	// check number of entries in histogram
	// NB as destructor was called above, the number of entries in the histogram
	// will be increased by the number of bins (inc over and underflow) as in
	// PIDVarI the behaviour to add one event across all bins is turned on.
	ASSERT_EQ((1000 + hist->GetSize()), hist->GetEntries());
	double prob = readtestPIDVarI.logL(checkTrack);

	ASSERT_LE(prob, 0);
	ASSERT_TRUE(IsFiniteNumber(prob));
}
