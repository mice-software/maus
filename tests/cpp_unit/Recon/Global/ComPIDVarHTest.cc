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

#include "Recon/Global/ComPIDVarH.hh"



TEST(ComPIDVarHTestSetUp, TestSetUpAndTearDown) {
	MAUS::recon::global::ComPIDVarH *testComPIDVarH = NULL;
	ASSERT_NO_THROW(testComPIDVarH = new MAUS::recon::global::ComPIDVarH("test",
								       "test"));
	std::string testfile = testComPIDVarH->Get_filename();
	std::string testdir = testComPIDVarH->Get_directory();
	ASSERT_NO_THROW(delete testComPIDVarH);

	EXPECT_EQ(gSystem->Unlink(testfile.c_str()), 0);
        // Unlink dir does not seem to work on some systems
        // suspect NFS mounts or stale handles
        // force system removal
        std::string rmdirCmd = "rm -fr ";
        rmdirCmd += testdir.c_str();
        EXPECT_EQ(system(rmdirCmd.c_str()), 0);
}

class ComPIDVarHTest : public ::testing::Test {

	virtual void SetUp() {
		MAUS::recon::global::ComPIDVarH testComPIDVarH("test", "test");
		testfile = testComPIDVarH.Get_filename();
		testdir = testComPIDVarH.Get_directory();
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

		  MAUS::DataStructure::Global::TrackPoint* tpCkovA =
		    new MAUS::DataStructure::Global::TrackPoint();

		  double num_PES = r.Gaus(30, 2);
		  tpCkovA->set_num_photoelectrons(num_PES);

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

TEST_F(ComPIDVarHTest, CreateFileConstructor) {

	ASSERT_TRUE(fileExists(testfile));
}

TEST_F(ComPIDVarHTest, ReadFileConstructor) {

	file = new TFile(testfile.c_str(), "READ");

	ASSERT_TRUE(file);

	ASSERT_FALSE(file->IsZombie());

	ASSERT_NO_THROW(MAUS::recon::global::ComPIDVarH testComPIDVarH(file, "test", 20, 40, 0, 1));

	ASSERT_ANY_THROW(MAUS::recon::global::ComPIDVarH testComPIDVarH(NULL, "test", 20, 40, 0, 1));

	ASSERT_ANY_THROW(MAUS::recon::global::ComPIDVarH testComPIDVarH(file, "sasquatch", 20, 40, 0, 1));
}

/* N.B. if either FillHist or LogL are failing, check the values of min and max
*	 bin in ComPIDVarH.hh, as if the testTracks fall outside of this range FillHist 
*  and LogL will fail.
*/

TEST_F(ComPIDVarHTest, FillHist) {

	MAUS::recon::global::ComPIDVarH testComPIDVarH("test", "test");
	for (int i = 0; i < 1000; i++) {
		testComPIDVarH.Fill_Hist(testTracks[i]);
	}
	testfile = testComPIDVarH.Get_filename();
	testdir = testComPIDVarH.Get_directory();
	// check histogram exists
	hist = testComPIDVarH.Get_hist();
	ASSERT_TRUE(hist);
	// check number of entries
	ASSERT_EQ(1000, hist->GetEntries());
}

TEST_F(ComPIDVarHTest, LogL) {
	// Constructor called within parentheses so that destructor will be
	// called when it goes out of scope
	 {
		MAUS::recon::global::ComPIDVarH writetestComPIDVarH("test", "test");
		for (int i = 0; i < 1000; i++) {
			writetestComPIDVarH.Fill_Hist(testTracks[i]);
		}
		testfile = writetestComPIDVarH.Get_filename();
		testdir = writetestComPIDVarH.Get_directory();
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

	MAUS::DataStructure::Global::TrackPoint* ctpCkovA =
	  new MAUS::DataStructure::Global::TrackPoint();

	double num_PES = 30;
	ctpCkovA->set_num_photoelectrons(num_PES);

	ctpCkovA->set_detector(MAUS::DataStructure::Global::kCherenkovA);
	ctpCkovA->set_mapper_name("MapCppGlobalTrackMatching-Through");

	checkTrack->AddTrackPoint(ctpCkovA);
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

	ASSERT_NO_THROW(MAUS::recon::global::ComPIDVarH readtestComPIDVarH(file, "test", 30, 50, 0, 140));

	MAUS::recon::global::ComPIDVarH readtestComPIDVarH(file, "test", 30, 50, 0, 140);

	hist = readtestComPIDVarH.Get_hist();
	ASSERT_TRUE(hist);
	// check number of entries in histogram
	// NB as destructor was called above, the number of entries in the histogram
	// will be increased by the number of bins (inc over and underflow) as in
	// ComPIDVarH the behaviour to add one event across all bins is turned on.
	ASSERT_EQ((1000 + hist->GetSize()), hist->GetEntries());
	double prob = readtestComPIDVarH.logL(checkTrack);

	ASSERT_LE(prob, 0);
	ASSERT_TRUE(IsFiniteNumber(prob));
}
