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

#include "Recon/Global/PIDVarB.hh"



TEST(PIDVarBTestSetUp, TestSetUpAndTearDown) {
	MAUS::recon::global::PIDVarB *testPIDVarB = NULL;
	ASSERT_NO_THROW(testPIDVarB = new MAUS::recon::global::PIDVarB("test",
								       "test"));
	std::string testfile = testPIDVarB->Get_filename();
	std::string testdir = testPIDVarB->Get_directory();
	ASSERT_NO_THROW(delete testPIDVarB);

	EXPECT_EQ(gSystem->Unlink(testfile.c_str()), 0);
        // Unlink dir does not seem to work on some systems
        // suspect NFS mounts or stale handles
        // force system removal
        // std::string rmdirCmd = "rm -fr ";
        // rmdirCmd += testdir.c_str();
        // EXPECT_EQ(system(rmdirCmd.c_str()), 0);
}

class PIDVarBTest : public ::testing::Test {

	virtual void SetUp() {
		MAUS::recon::global::PIDVarB testPIDVarB("test", "test");
		testfile = testPIDVarB.Get_filename();
		testdir = testPIDVarB.Get_directory();
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
		  MAUS::DataStructure::Global::TrackPoint* tpTracker =
		    new MAUS::DataStructure::Global::TrackPoint();
		  tpTracker->set_detector(MAUS::DataStructure::Global::kTracker0_1);

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

		  double px = 0.0;
		  double py = 0.0;
		  double pz = 200.0;
		  double E = 226.0;
		  TLorentzVector tpTrackerMom(px, py, pz, E);

		  tp0->set_position(pos0);
		  tp1->set_position(pos1);
		  tpTracker->set_momentum(tpTrackerMom);
		  tp0->set_mapper_name("MapCppGlobalTrackMatching");
		  tp1->set_mapper_name("MapCppGlobalTrackMatching");
		  tpTracker->set_mapper_name("MapCppGlobalTrackMatching");
		  testTrack->AddTrackPoint(tp0);
		  testTrack->AddTrackPoint(tp1);
		  testTrack->AddTrackPoint(tpTracker);
		  testTrack->set_mapper_name("MapCppGlobalTrackMatching");
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

TEST_F(PIDVarBTest, CreateFileConstructor) {

	ASSERT_TRUE(fileExists(testfile));
}

TEST_F(PIDVarBTest, ReadFileConstructor) {

	file = new TFile(testfile.c_str(), "READ");

	ASSERT_TRUE(file);

	ASSERT_FALSE(file->IsZombie());

	ASSERT_NO_THROW(MAUS::recon::global::PIDVarB testPIDVarB(file, "test", 50, 350, 20, 40));

	ASSERT_ANY_THROW(MAUS::recon::global::PIDVarB testPIDVarB(NULL, "test", 50, 350, 20, 40));

	ASSERT_ANY_THROW(MAUS::recon::global::PIDVarB testPIDVarB(file, "sasquatch", 50, 350, 20, 40));
}

/* N.B. if either FillHist or LogL are failing, check the values of min and max
*	 bins in PIDVarB.hh, as if the testTracks fall outside of this range FillHist 
*  and LogL will fail.
*/

TEST_F(PIDVarBTest, FillHist) {

	MAUS::recon::global::PIDVarB testPIDVarB("test", "test");
	for (int i = 0; i < 1000; i++) {
		testPIDVarB.Fill_Hist(testTracks[i]);
	}
	testfile = testPIDVarB.Get_filename();
	testdir = testPIDVarB.Get_directory();
	// check histogram exists
	hist = testPIDVarB.Get_hist();
	ASSERT_TRUE(hist);
	// check number of entries
	ASSERT_EQ(1000, hist->GetEntries());
}

TEST_F(PIDVarBTest, LogL) {
	// Constructor called within parentheses so that destructor will be
	// called when it goes out of scope
	 {
		MAUS::recon::global::PIDVarB writetestPIDVarB("test", "test");
		for (int i = 0; i < 1000; i++) {
			writetestPIDVarB.Fill_Hist(testTracks[i]);
		}
		testfile = writetestPIDVarB.Get_filename();
		testdir = writetestPIDVarB.Get_directory();
	 }

	MAUS::DataStructure::Global::Track* checkTrack =
			new MAUS::DataStructure::Global::Track();
	MAUS::DataStructure::Global::TrackPoint* ctp0 =
			new MAUS::DataStructure::Global::TrackPoint();
	MAUS::DataStructure::Global::TrackPoint* ctp1 =
			new MAUS::DataStructure::Global::TrackPoint();
	MAUS::DataStructure::Global::TrackPoint* ctpTracker =
	                new MAUS::DataStructure::Global::TrackPoint();
	ctp0->set_detector(MAUS::DataStructure::Global::kTOF0);
	ctp1->set_detector(MAUS::DataStructure::Global::kTOF1);
	ctpTracker->set_detector(MAUS::DataStructure::Global::kTracker0_1);

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

	double px = 0.0;
	double py = 0.0;
	double pz = 200.0;
	double E = 226.0;
	TLorentzVector ctpTrackerMom(px, py, pz, E);

	ctp0->set_position(cpos0);
	ctp1->set_position(cpos1);
	ctpTracker->set_momentum(ctpTrackerMom);
	ctp0->set_mapper_name("MapCppGlobalTrackMatching");
	ctp1->set_mapper_name("MapCppGlobalTrackMatching");
	ctpTracker->set_mapper_name("MapCppGlobalTrackMatching");
	checkTrack->AddTrackPoint(ctp0);
	checkTrack->AddTrackPoint(ctp1);
	checkTrack->AddTrackPoint(ctpTracker);
	checkTrack->set_mapper_name("MapCppGlobalTrackMatching");

	file = new TFile(testfile.c_str(), "READ");

	ASSERT_TRUE(file);

	ASSERT_FALSE(file->IsZombie());

	ASSERT_NO_THROW(MAUS::recon::global::PIDVarB readtestPIDVarB(file, "test", 50, 350, 20, 40));

	MAUS::recon::global::PIDVarB readtestPIDVarB(file, "test", 50, 350, 20, 40);

	hist = readtestPIDVarB.Get_hist();
	ASSERT_TRUE(hist);
	// check number of entries in histogram
	// NB as destructor was called above, the number of entries in the histogram
	// will be increased by the number of bins (inc over and underflow) as in
	// PIDVarB the behaviour to add one event across all bins is turned on.
	ASSERT_EQ((1000 + hist->GetSize()), hist->GetEntries());
	double prob = readtestPIDVarB.logL(checkTrack);

	ASSERT_LE(prob, 0);
	ASSERT_TRUE(IsFiniteNumber(prob));
}
