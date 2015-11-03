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

#include "Recon/Global/PIDVarG.hh"



TEST(PIDVarGTestSetUp, TestSetUpAndTearDown) {
	MAUS::recon::global::PIDVarG *testPIDVarG = NULL;
	ASSERT_NO_THROW(testPIDVarG = new MAUS::recon::global::PIDVarG("test",
								       "test"));
	std::string testfile = testPIDVarG->Get_filename();
	std::string testdir = testPIDVarG->Get_directory();
	ASSERT_NO_THROW(delete testPIDVarG);

	EXPECT_EQ(gSystem->Unlink(testfile.c_str()), 0);
        // Unlink dir does not seem to work on some systems
        // suspect NFS mounts or stale handles
        // force system removal
        // std::string rmdirCmd = "rm -fr ";
        // rmdirCmd += testdir.c_str();
        // EXPECT_EQ(system(rmdirCmd.c_str()), 0);
}

class PIDVarGTest : public ::testing::Test {

	virtual void SetUp() {
		MAUS::recon::global::PIDVarG testPIDVarG("test", "test");
		testfile = testPIDVarG.Get_filename();
		testdir = testPIDVarG.Get_directory();
		TRandom3 r;
		for (int i = 0; i < 1000; i++) {
		  MAUS::DataStructure::Global::Track* testTrack =
		    new MAUS::DataStructure::Global::Track();
		  MAUS::DataStructure::Global::TrackPoint* tpEMR =
		    new MAUS::DataStructure::Global::TrackPoint();

		  double emr_density = r.Gaus(0.5, 0.1);
		  testTrack->set_emr_plane_density(emr_density);

		  tpEMR->set_mapper_name("MapCppGlobalTrackMatching");
		  tpEMR->set_detector(MAUS::DataStructure::Global::kEMR);
		  testTrack->AddTrackPoint(tpEMR);
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
	TH1F *hist;
	std::string histname;
	std::vector<MAUS::DataStructure::Global::Track*> testTracks;
};

TEST_F(PIDVarGTest, CreateFileConstructor) {

	ASSERT_TRUE(fileExists(testfile));
}

TEST_F(PIDVarGTest, ReadFileConstructor) {

	file = new TFile(testfile.c_str(), "READ");

	ASSERT_TRUE(file);

	ASSERT_FALSE(file->IsZombie());

	ASSERT_NO_THROW(MAUS::recon::global::PIDVarG testPIDVarG(file, "test", 0, 1));

	ASSERT_ANY_THROW(MAUS::recon::global::PIDVarG testPIDVarG(NULL, "test", 0, 1));

	ASSERT_ANY_THROW(MAUS::recon::global::PIDVarG testPIDVarG(file, "sasquatch", 0, 1));
}

/* N.B. if either FillHist or LogL are failing, check the values of min and max
*	 bins in PIDVarG.hh, as if the testTracks fall outside of this range FillHist 
*  and LogL will fail.
*/

TEST_F(PIDVarGTest, FillHist) {

	MAUS::recon::global::PIDVarG testPIDVarG("test", "test");
	for (int i = 0; i < 1000; i++) {
		testPIDVarG.Fill_Hist(testTracks[i]);
	}
	testfile = testPIDVarG.Get_filename();
	testdir = testPIDVarG.Get_directory();
	// check histogram exists
	hist = testPIDVarG.Get_hist();
	ASSERT_TRUE(hist);
	// check number of entries
	ASSERT_EQ(1000, hist->GetEntries());
}

TEST_F(PIDVarGTest, LogL) {
	// Constructor called within parentheses so that destructor will be
	// called when it goes out of scope
	 {
		MAUS::recon::global::PIDVarG writetestPIDVarG("test", "test");
		for (int i = 0; i < 1000; i++) {
			writetestPIDVarG.Fill_Hist(testTracks[i]);
		}
		testfile = writetestPIDVarG.Get_filename();
		testdir = writetestPIDVarG.Get_directory();
	 }

	MAUS::DataStructure::Global::Track* checkTrack =
			new MAUS::DataStructure::Global::Track();
	MAUS::DataStructure::Global::TrackPoint* ctpEMR =
			new MAUS::DataStructure::Global::TrackPoint();
	ctpEMR->set_detector(MAUS::DataStructure::Global::kCalorimeter);

	double emr_density = 0.5;
	checkTrack->set_emr_plane_density(emr_density);
	ctpEMR->set_detector(MAUS::DataStructure::Global::kEMR);
	ctpEMR->set_mapper_name("MapCppGlobalTrackMatching");
	checkTrack->AddTrackPoint(ctpEMR);
	checkTrack->set_mapper_name("MapCppGlobalTrackMatching");

	file = new TFile(testfile.c_str(), "READ");

	ASSERT_TRUE(file);

	ASSERT_FALSE(file->IsZombie());

	ASSERT_NO_THROW(MAUS::recon::global::PIDVarG readtestPIDVarG(file, "test", 0, 1));

	MAUS::recon::global::PIDVarG readtestPIDVarG(file, "test", 0, 1);

	hist = readtestPIDVarG.Get_hist();
	ASSERT_TRUE(hist);
	// check number of entries in histogram
	// NB as destructor was called above, the number of entries in the histogram
	// will be increased by the number of bins (inc over and underflow) as in
	// PIDVarG the behaviour to add one event across all bins is turned on.
	ASSERT_EQ((1000 + hist->GetSize()), hist->GetEntries());
	double prob = readtestPIDVarG.logL(checkTrack);

	ASSERT_LE(prob, 0);
	ASSERT_TRUE(IsFiniteNumber(prob));
}
