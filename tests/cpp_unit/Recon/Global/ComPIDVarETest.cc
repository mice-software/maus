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

#include "Recon/Global/ComPIDVarE.hh"



TEST(ComPIDVarETestSetUp, TestSetUpAndTearDown) {
	MAUS::recon::global::ComPIDVarE *testComPIDVarE = NULL;
	ASSERT_NO_THROW(testComPIDVarE = new MAUS::recon::global::ComPIDVarE("test",
								       "test"));
	std::string testfile = testComPIDVarE->Get_filename();
	std::string testdir = testComPIDVarE->Get_directory();
	ASSERT_NO_THROW(delete testComPIDVarE);

	EXPECT_EQ(gSystem->Unlink(testfile.c_str()), 0);
        // Unlink dir does not seem to work on some systems
        // suspect NFS mounts or stale handles
        // force system removal
        std::string rmdirCmd = "rm -fr ";
        rmdirCmd += testdir.c_str();
        EXPECT_EQ(system(rmdirCmd.c_str()), 0);
}

class ComPIDVarETest : public ::testing::Test {

	virtual void SetUp() {
		MAUS::recon::global::ComPIDVarE testComPIDVarE("test", "test");
		testfile = testComPIDVarE.Get_filename();
		testdir = testComPIDVarE.Get_directory();
		TRandom3 r;
		for (int i = 0; i < 1000; i++) {
		  MAUS::DataStructure::Global::Track* testTrack =
		    new MAUS::DataStructure::Global::Track();
		  MAUS::DataStructure::Global::TrackPoint* tp1 =
		    new MAUS::DataStructure::Global::TrackPoint();
		  MAUS::DataStructure::Global::TrackPoint* tp2 =
		    new MAUS::DataStructure::Global::TrackPoint();
		  tp1->set_detector(MAUS::DataStructure::Global::kTOF1);
		  tp2->set_detector(MAUS::DataStructure::Global::kTOF2);

		  MAUS::DataStructure::Global::TrackPoint* tpEMR =
		    new MAUS::DataStructure::Global::TrackPoint();

		  double emr_range = r.Gaus(50, 10);
		  testTrack->set_emr_range_primary(emr_range);

		  double x1 = 0.0;
		  double y1 = 0.0;
		  double z1 = 0.0;
		  double t1 = r.Gaus(45, 2);
		  TLorentzVector pos1(x1, y1, z1, t1);

		  double x2 = 0.0;
		  double y2 = 0.0;
		  double z2 = 0.0;
		  double t2 = r.Gaus(85, 2);
		  TLorentzVector pos2(x2, y2, z2, t2);

		  tp1->set_position(pos1);
		  tp2->set_position(pos2);
		  tp1->set_mapper_name("MapCppGlobalTrackMatching-Through");
		  tp2->set_mapper_name("MapCppGlobalTrackMatching-Through");
		  testTrack->AddTrackPoint(tp1);
		  testTrack->AddTrackPoint(tp2);
		  tpEMR->set_mapper_name("MapCppGlobalTrackMatching-Through");
		  tpEMR->set_detector(MAUS::DataStructure::Global::kEMR);
		  testTrack->AddTrackPoint(tpEMR);
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

TEST_F(ComPIDVarETest, CreateFileConstructor) {

	ASSERT_TRUE(fileExists(testfile));
}

TEST_F(ComPIDVarETest, ReadFileConstructor) {

	file = new TFile(testfile.c_str(), "READ");

	ASSERT_TRUE(file);

	ASSERT_FALSE(file->IsZombie());

	ASSERT_NO_THROW(MAUS::recon::global::ComPIDVarE testComPIDVarE(file, "test", 20, 40, 0, 1000));

	ASSERT_ANY_THROW(MAUS::recon::global::ComPIDVarE testComPIDVarE(NULL, "test", 20, 40, 0, 1000));

	ASSERT_ANY_THROW(MAUS::recon::global::ComPIDVarE testComPIDVarE(file, "sasquatch", 20, 40, 0, 1000));
}

/* N.B. if either FillHist or LogL are failing, check the values of min and max
*	 bin in ComPIDVarE.hh, as if the testTracks fall outside of this range FillHist 
*  and LogL will fail.
*/

TEST_F(ComPIDVarETest, FillHist) {

	MAUS::recon::global::ComPIDVarE testComPIDVarE("test", "test");
	for (int i = 0; i < 1000; i++) {
		testComPIDVarE.Fill_Hist(testTracks[i]);
	}
	testfile = testComPIDVarE.Get_filename();
	testdir = testComPIDVarE.Get_directory();
	// check histogram exists
	hist = testComPIDVarE.Get_hist();
	ASSERT_TRUE(hist);
	// check number of entries
	ASSERT_EQ(1000, hist->GetEntries());
}

TEST_F(ComPIDVarETest, LogL) {
	// Constructor called within parentheses so that destructor will be
	// called when it goes out of scope
	 {
		MAUS::recon::global::ComPIDVarE writetestComPIDVarE("test", "test");
		for (int i = 0; i < 1000; i++) {
			writetestComPIDVarE.Fill_Hist(testTracks[i]);
		}
		testfile = writetestComPIDVarE.Get_filename();
		testdir = writetestComPIDVarE.Get_directory();
	 }

	MAUS::DataStructure::Global::Track* checkTrack =
			new MAUS::DataStructure::Global::Track();
	MAUS::DataStructure::Global::TrackPoint* ctp1 =
			new MAUS::DataStructure::Global::TrackPoint();
	MAUS::DataStructure::Global::TrackPoint* ctp2 =
			new MAUS::DataStructure::Global::TrackPoint();
	ctp1->set_detector(MAUS::DataStructure::Global::kTOF1);
	ctp2->set_detector(MAUS::DataStructure::Global::kTOF2);

	double x1 = 0.0;
	double y1 = 0.0;
	double z1 = 0.0;
	double t1 = 15.0;
	TLorentzVector cpos1(x1, y1, z1, t1);

	double x2 = 0.0;
	double y2 = 0.0;
	double z2 = 0.0;
	double t2 = 55.0;
	TLorentzVector cpos2(x2, y2, z2, t2);

	MAUS::DataStructure::Global::TrackPoint* ctpEMR =
			new MAUS::DataStructure::Global::TrackPoint();
	ctpEMR->set_detector(MAUS::DataStructure::Global::kCalorimeter);

	double emr_range = 500;
	checkTrack->set_emr_range_primary(emr_range);
	ctpEMR->set_detector(MAUS::DataStructure::Global::kEMR);
	ctpEMR->set_mapper_name("MapCppGlobalTrackMatching-Through");
	checkTrack->AddTrackPoint(ctpEMR);
	ctp1->set_position(cpos1);
	ctp2->set_position(cpos2);
	ctp1->set_mapper_name("MapCppGlobalTrackMatching-Through");
	ctp2->set_mapper_name("MapCppGlobalTrackMatching-Through");
	checkTrack->AddTrackPoint(ctp1);
	checkTrack->AddTrackPoint(ctp2);
	checkTrack->set_mapper_name("MapCppGlobalTrackMatching-Through");

	file = new TFile(testfile.c_str(), "READ");

	ASSERT_TRUE(file);

	ASSERT_FALSE(file->IsZombie());

	ASSERT_NO_THROW(MAUS::recon::global::ComPIDVarE readtestComPIDVarE(file, "test", 30, 50, 0, 1000));

	MAUS::recon::global::ComPIDVarE readtestComPIDVarE(file, "test", 30, 50, 0, 1000);

	hist = readtestComPIDVarE.Get_hist();
	ASSERT_TRUE(hist);
	// check number of entries in histogram
	// NB as destructor was called above, the number of entries in the histogram
	// will be increased by the number of bins (inc over and underflow) as in
	// ComPIDVarE the behaviour to add one event across all bins is turned on.
	ASSERT_EQ((1000 + hist->GetSize()), hist->GetEntries());
	double prob = readtestComPIDVarE.logL(checkTrack);

	ASSERT_LE(prob, 0);
	ASSERT_TRUE(IsFiniteNumber(prob));
}
