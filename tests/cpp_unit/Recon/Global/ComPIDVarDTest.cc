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

#include "Recon/Global/ComPIDVarD.hh"



TEST(ComPIDVarDTestSetUp, TestSetUpAndTearDown) {
	MAUS::recon::global::ComPIDVarD *testComPIDVarD = NULL;
	ASSERT_NO_THROW(testComPIDVarD = new MAUS::recon::global::ComPIDVarD("test",
								       "test"));
	std::string testfile = testComPIDVarD->Get_filename();
	std::string testdir = testComPIDVarD->Get_directory();
	ASSERT_NO_THROW(delete testComPIDVarD);

	EXPECT_EQ(gSystem->Unlink(testfile.c_str()), 0);
        // Unlink dir does not seem to work on some systems
        // suspect NFS mounts or stale handles
        // force system removal
        // std::string rmdirCmd = "rm -fr ";
        // rmdirCmd += testdir.c_str();
        // EXPECT_EQ(system(rmdirCmd.c_str()), 0);
}

class ComPIDVarDTest : public ::testing::Test {

	virtual void SetUp() {
		MAUS::recon::global::ComPIDVarD testComPIDVarD("test", "test");
		testfile = testComPIDVarD.Get_filename();
		testdir = testComPIDVarD.Get_directory();
		TRandom3 r;
		for (int i = 0; i < 1000; i++) {
		  MAUS::DataStructure::Global::Track* testTrack =
		    new MAUS::DataStructure::Global::Track();
		  MAUS::DataStructure::Global::TrackPoint* tpEMR =
		    new MAUS::DataStructure::Global::TrackPoint();

		  double emr_range = r.Gaus(50, 10);
		  testTrack->set_emr_range_primary(emr_range);

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
	TH1F *hist;
	std::string histname;
	std::vector<MAUS::DataStructure::Global::Track*> testTracks;
};

TEST_F(ComPIDVarDTest, CreateFileConstructor) {

	ASSERT_TRUE(fileExists(testfile));
}

TEST_F(ComPIDVarDTest, ReadFileConstructor) {

	file = new TFile(testfile.c_str(), "READ");

	ASSERT_TRUE(file);

	ASSERT_FALSE(file->IsZombie());

	ASSERT_NO_THROW(MAUS::recon::global::ComPIDVarD testComPIDVarD(file, "test", 0, 8000));

	ASSERT_ANY_THROW(MAUS::recon::global::ComPIDVarD testComPIDVarD(NULL, "test", 0, 8000));

	ASSERT_ANY_THROW(MAUS::recon::global::ComPIDVarD testComPIDVarD(file, "sasquatch", 0, 8000));
}

/* N.B. if either FillHist or LogL are failing, check the values of min and max
*	 bins in ComPIDVarD.hh, as if the testTracks fall outside of this range FillHist 
*  and LogL will fail.
*/

TEST_F(ComPIDVarDTest, FillHist) {

	MAUS::recon::global::ComPIDVarD testComPIDVarD("test", "test");
	for (int i = 0; i < 1000; i++) {
		testComPIDVarD.Fill_Hist(testTracks[i]);
	}
	testfile = testComPIDVarD.Get_filename();
	testdir = testComPIDVarD.Get_directory();
	// check histogram exists
	hist = testComPIDVarD.Get_hist();
	ASSERT_TRUE(hist);
	// check number of entries
	ASSERT_EQ(1000, hist->GetEntries());
}

TEST_F(ComPIDVarDTest, LogL) {
	// Constructor called within parentheses so that destructor will be
	// called when it goes out of scope
	 {
		MAUS::recon::global::ComPIDVarD writetestComPIDVarD("test", "test");
		for (int i = 0; i < 1000; i++) {
			writetestComPIDVarD.Fill_Hist(testTracks[i]);
		}
		testfile = writetestComPIDVarD.Get_filename();
		testdir = writetestComPIDVarD.Get_directory();
	 }

	MAUS::DataStructure::Global::Track* checkTrack =
			new MAUS::DataStructure::Global::Track();
	MAUS::DataStructure::Global::TrackPoint* ctpEMR =
			new MAUS::DataStructure::Global::TrackPoint();
	ctpEMR->set_detector(MAUS::DataStructure::Global::kCalorimeter);

	double emr_range = 50;
	checkTrack->set_emr_range_primary(emr_range);
	ctpEMR->set_detector(MAUS::DataStructure::Global::kEMR);
	ctpEMR->set_mapper_name("MapCppGlobalTrackMatching-Through");
	checkTrack->AddTrackPoint(ctpEMR);
	checkTrack->set_mapper_name("MapCppGlobalTrackMatching-Through");

	file = new TFile(testfile.c_str(), "READ");

	ASSERT_TRUE(file);

	ASSERT_FALSE(file->IsZombie());

	ASSERT_NO_THROW(MAUS::recon::global::ComPIDVarD readtestComPIDVarD(file, "test", 0, 1000));

	MAUS::recon::global::ComPIDVarD readtestComPIDVarD(file, "test", 0, 1000);

	hist = readtestComPIDVarD.Get_hist();
	ASSERT_TRUE(hist);
	// check number of entries in histogram
	// NB as destructor was called above, the number of entries in the histogram
	// will be increased by the number of bins (inc over and underflow) as in
	// ComPIDVarD the behaviour to add one event across all bins is turned on.
	ASSERT_EQ((1000 + hist->GetSize()), hist->GetEntries());
	double prob = readtestComPIDVarD.logL(checkTrack);

	ASSERT_LE(prob, 0);
	ASSERT_TRUE(IsFiniteNumber(prob));
}
