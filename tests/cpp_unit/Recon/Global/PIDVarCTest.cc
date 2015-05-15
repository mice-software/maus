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

#include "Recon/Global/PIDVarC.hh"



TEST(PIDVarCTestSetUp, TestSetUpAndTearDown) {
	MAUS::recon::global::PIDVarC *testPIDVarC = NULL;
	ASSERT_NO_THROW(testPIDVarC = new MAUS::recon::global::PIDVarC("test",
								       "test"));
	std::string testfile = testPIDVarC->Get_filename();
	std::string testdir = testPIDVarC->Get_directory();
	ASSERT_NO_THROW(delete testPIDVarC);

	EXPECT_EQ(gSystem->Unlink(testfile.c_str()), 0);
        // Unlink dir does not seem to work on some systems
        // suspect NFS mounts or stale handles
        // force system removal
        // std::string rmdirCmd = "rm -fr ";
        // rmdirCmd += testdir.c_str();
        // EXPECT_EQ(system(rmdirCmd.c_str()), 0);
}

class PIDVarCTest : public ::testing::Test {

	virtual void SetUp() {
		MAUS::recon::global::PIDVarC testPIDVarC("test", "test");
		testfile = testPIDVarC.Get_filename();
		testdir = testPIDVarC.Get_directory();
		TRandom3 r;
		for (int i = 0; i < 1000; i++) {
		  MAUS::DataStructure::Global::Track* testTrack =
		    new MAUS::DataStructure::Global::Track();
		  MAUS::DataStructure::Global::TrackPoint* tpKL =
		    new MAUS::DataStructure::Global::TrackPoint();
		  tpKL->set_detector(MAUS::DataStructure::Global::kCalorimeter);
		  MAUS::DataStructure::Global::TrackPoint* tpTracker =
		    new MAUS::DataStructure::Global::TrackPoint();
		  tpTracker->set_detector(MAUS::DataStructure::Global::kTracker1_1);

		  double adc_charge_prod = r.Gaus(1000, 10);
		  tpKL->set_ADC_charge_product(adc_charge_prod);

		  double px = 0.0;
		  double py = 0.0;
		  double pz = 200.0;
		  double E = 226.0;
		  TLorentzVector tpTrackerMom(px, py, pz, E);

		  tpTracker->set_momentum(tpTrackerMom);
		  tpKL->set_mapper_name("MapCppGlobalTrackMatching");
		  tpTracker->set_mapper_name("MapCppGlobalTrackMatching");
		  testTrack->AddTrackPoint(tpKL);
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

TEST_F(PIDVarCTest, CreateFileConstructor) {

	ASSERT_TRUE(fileExists(testfile));
}

TEST_F(PIDVarCTest, ReadFileConstructor) {

	file = new TFile(testfile.c_str(), "READ");

	ASSERT_TRUE(file);

	ASSERT_FALSE(file->IsZombie());

	ASSERT_NO_THROW(MAUS::recon::global::PIDVarC testPIDVarC(file, "test", 50, 350, 0, 8000));

	ASSERT_ANY_THROW(MAUS::recon::global::PIDVarC testPIDVarC(NULL, "test", 50, 350, 0, 8000));

	ASSERT_ANY_THROW(MAUS::recon::global::PIDVarC testPIDVarC(file, "sasquatch", 50, 350, 0, 8000));
}

/* N.B. if either FillHist or LogL are failing, check the values of min and max
*	 bins in PIDVarC.hh, as if the testTracks fall outside of this range FillHist 
*  and LogL will fail.
*/

TEST_F(PIDVarCTest, FillHist) {

	MAUS::recon::global::PIDVarC testPIDVarC("test", "test");
	for (int i = 0; i < 1000; i++) {
		testPIDVarC.Fill_Hist(testTracks[i]);
	}
	testfile = testPIDVarC.Get_filename();
	testdir = testPIDVarC.Get_directory();
	// check histogram exists
	hist = testPIDVarC.Get_hist();
	ASSERT_TRUE(hist);
	// check number of entries
	ASSERT_EQ(1000, hist->GetEntries());
}

TEST_F(PIDVarCTest, LogL) {
	// Constructor called within parentheses so that destructor will be
	// called when it goes out of scope
	 {
		MAUS::recon::global::PIDVarC writetestPIDVarC("test", "test");
		for (int i = 0; i < 1000; i++) {
			writetestPIDVarC.Fill_Hist(testTracks[i]);
		}
		testfile = writetestPIDVarC.Get_filename();
		testdir = writetestPIDVarC.Get_directory();
	 }

	MAUS::DataStructure::Global::Track* checkTrack =
			new MAUS::DataStructure::Global::Track();
	MAUS::DataStructure::Global::TrackPoint* ctpKL =
			new MAUS::DataStructure::Global::TrackPoint();
	MAUS::DataStructure::Global::TrackPoint* ctpTracker =
	                new MAUS::DataStructure::Global::TrackPoint();
	ctpKL->set_detector(MAUS::DataStructure::Global::kCalorimeter);
	ctpTracker->set_detector(MAUS::DataStructure::Global::kTracker1_1);

	double adc_charge_prod = 1000;
	ctpKL->set_ADC_charge_product(adc_charge_prod);

	double px = 0.0;
	double py = 0.0;
	double pz = 200.0;
	double E = 226.0;
	TLorentzVector ctpTrackerMom(px, py, pz, E);

	ctpTracker->set_momentum(ctpTrackerMom);
	ctpKL->set_mapper_name("MapCppGlobalTrackMatching");
	ctpTracker->set_mapper_name("MapCppGlobalTrackMatching");
	checkTrack->AddTrackPoint(ctpKL);
	checkTrack->AddTrackPoint(ctpTracker);
	checkTrack->set_mapper_name("MapCppGlobalTrackMatching");

	file = new TFile(testfile.c_str(), "READ");

	ASSERT_TRUE(file);

	ASSERT_FALSE(file->IsZombie());

	ASSERT_NO_THROW(MAUS::recon::global::PIDVarC readtestPIDVarC(file, "test", 50, 350, 0, 8000));

	MAUS::recon::global::PIDVarC readtestPIDVarC(file, "test", 50, 350, 0, 8000);

	hist = readtestPIDVarC.Get_hist();
	ASSERT_TRUE(hist);
	// check number of entries in histogram
	// NB as destructor was called above, the number of entries in the histogram
	// will be increased by the number of bins (inc over and underflow) as in
	// PIDVarC the behaviour to add one event across all bins is turned on.
	ASSERT_EQ((1000 + hist->GetSize()), hist->GetEntries());
	double prob = readtestPIDVarC.logL(checkTrack);

	ASSERT_LE(prob, 0);
	ASSERT_TRUE(IsFiniteNumber(prob));
}
