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

#include "Recon/Global/PIDVarD.hh"



TEST(PIDVarDTestSetUp, TestSetUpAndTearDown) {
	MAUS::recon::global::PIDVarD *testPIDVarD = NULL;
	ASSERT_NO_THROW(testPIDVarD = new MAUS::recon::global::PIDVarD("test",
								       "test"));
	std::string testfile = testPIDVarD->Get_filename();
	std::string testdir = testPIDVarD->Get_directory();
	ASSERT_NO_THROW(delete testPIDVarD);

	EXPECT_EQ(gSystem->Unlink(testfile.c_str()), 0);
        // Unlink dir does not seem to work on some systems
        // suspect NFS mounts or stale handles
        // force system removal
        // std::string rmdirCmd = "rm -fr ";
        // rmdirCmd += testdir.c_str();
        // EXPECT_EQ(system(rmdirCmd.c_str()), 0);
}

class PIDVarDTest : public ::testing::Test {

	virtual void SetUp() {
		MAUS::recon::global::PIDVarD testPIDVarD("test", "test");
		testfile = testPIDVarD.Get_filename();
		testdir = testPIDVarD.Get_directory();
		TRandom3 r;
		for (int i = 0; i < 1000; i++) {
		  MAUS::DataStructure::Global::Track* testTrack =
		    new MAUS::DataStructure::Global::Track();
		  MAUS::DataStructure::Global::TrackPoint* tpKL =
		    new MAUS::DataStructure::Global::TrackPoint();

		  double adc_charge_prod = r.Gaus(4000, 10);
		  tpKL->set_ADC_charge_product(adc_charge_prod);

		  tpKL->set_mapper_name("MapCppGlobalTrackMatching-Through");
		  tpKL->set_detector(MAUS::DataStructure::Global::kCalorimeter);
		  testTrack->AddTrackPoint(tpKL);
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

TEST_F(PIDVarDTest, CreateFileConstructor) {

	ASSERT_TRUE(fileExists(testfile));
}

TEST_F(PIDVarDTest, ReadFileConstructor) {

	file = new TFile(testfile.c_str(), "READ");

	ASSERT_TRUE(file);

	ASSERT_FALSE(file->IsZombie());

	ASSERT_NO_THROW(MAUS::recon::global::PIDVarD testPIDVarD(file, "test", 0, 8000));

	ASSERT_ANY_THROW(MAUS::recon::global::PIDVarD testPIDVarD(NULL, "test", 0, 8000));

	ASSERT_ANY_THROW(MAUS::recon::global::PIDVarD testPIDVarD(file, "sasquatch", 0, 8000));
}

/* N.B. if either FillHist or LogL are failing, check the values of min and max
*	 bins in PIDVarD.hh, as if the testTracks fall outside of this range FillHist 
*  and LogL will fail.
*/

TEST_F(PIDVarDTest, FillHist) {

	MAUS::recon::global::PIDVarD testPIDVarD("test", "test");
	for (int i = 0; i < 1000; i++) {
		testPIDVarD.Fill_Hist(testTracks[i]);
	}
	testfile = testPIDVarD.Get_filename();
	testdir = testPIDVarD.Get_directory();
	// check histogram exists
	hist = testPIDVarD.Get_hist();
	ASSERT_TRUE(hist);
	// check number of entries
	ASSERT_EQ(1000, hist->GetEntries());
}

TEST_F(PIDVarDTest, LogL) {
	// Constructor called within parentheses so that destructor will be
	// called when it goes out of scope
	 {
		MAUS::recon::global::PIDVarD writetestPIDVarD("test", "test");
		for (int i = 0; i < 1000; i++) {
			writetestPIDVarD.Fill_Hist(testTracks[i]);
		}
		testfile = writetestPIDVarD.Get_filename();
		testdir = writetestPIDVarD.Get_directory();
	 }

	MAUS::DataStructure::Global::Track* checkTrack =
			new MAUS::DataStructure::Global::Track();
	MAUS::DataStructure::Global::TrackPoint* ctpKL =
			new MAUS::DataStructure::Global::TrackPoint();
	ctpKL->set_detector(MAUS::DataStructure::Global::kCalorimeter);

	double adc_charge_prod = 1000;
	ctpKL->set_ADC_charge_product(adc_charge_prod);
	ctpKL->set_detector(MAUS::DataStructure::Global::kCalorimeter);
	ctpKL->set_mapper_name("MapCppGlobalTrackMatching-Through");
	checkTrack->AddTrackPoint(ctpKL);
	checkTrack->set_mapper_name("MapCppGlobalTrackMatching-Through");

	file = new TFile(testfile.c_str(), "READ");

	ASSERT_TRUE(file);

	ASSERT_FALSE(file->IsZombie());

	ASSERT_NO_THROW(MAUS::recon::global::PIDVarD readtestPIDVarD(file, "test", 0, 8000));

	MAUS::recon::global::PIDVarD readtestPIDVarD(file, "test", 0, 8000);

	hist = readtestPIDVarD.Get_hist();
	ASSERT_TRUE(hist);
	// check number of entries in histogram
	// NB as destructor was called above, the number of entries in the histogram
	// will be increased by the number of bins (inc over and underflow) as in
	// PIDVarD the behaviour to add one event across all bins is turned on.
	ASSERT_EQ((1000 + hist->GetSize()), hist->GetEntries());
	double prob = readtestPIDVarD.logL(checkTrack);

	ASSERT_LE(prob, 0);
	ASSERT_TRUE(IsFiniteNumber(prob));
}
