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

#include "Recon/Global/ComPIDVarC.hh"



TEST(ComPIDVarCTestSetUp, TestSetUpAndTearDown) {
	MAUS::recon::global::ComPIDVarC *testComPIDVarC = NULL;
	ASSERT_NO_THROW(testComPIDVarC = new MAUS::recon::global::ComPIDVarC("test",
								       "test"));
	std::string testfile = testComPIDVarC->Get_filename();
	std::string testdir = testComPIDVarC->Get_directory();
	ASSERT_NO_THROW(delete testComPIDVarC);

	EXPECT_EQ(gSystem->Unlink(testfile.c_str()), 0);
        // Unlink dir does not seem to work on some systems
        // suspect NFS mounts or stale handles
        // force system removal
        // std::string rmdirCmd = "rm -fr ";
        // rmdirCmd += testdir.c_str();
        // EXPECT_EQ(system(rmdirCmd.c_str()), 0);
}

class ComPIDVarCTest : public ::testing::Test {

	virtual void SetUp() {
		MAUS::recon::global::ComPIDVarC testComPIDVarC("test", "test");
		testfile = testComPIDVarC.Get_filename();
		testdir = testComPIDVarC.Get_directory();
		TRandom3 r;
		for (int i = 0; i < 1000; i++) {
		  MAUS::DataStructure::Global::Track* testTrack =
		    new MAUS::DataStructure::Global::Track();
		  MAUS::DataStructure::Global::TrackPoint* tpKL =
		    new MAUS::DataStructure::Global::TrackPoint();
		  tpKL->set_detector(MAUS::DataStructure::Global::kCalorimeter);
		  double adc_charge_prod = r.Gaus(1000, 10);
		  tpKL->set_ADC_charge_product(adc_charge_prod);

		  tpKL->set_mapper_name("MapCppGlobalTrackMatching-Through");
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

TEST_F(ComPIDVarCTest, CreateFileConstructor) {

	ASSERT_TRUE(fileExists(testfile));
}

TEST_F(ComPIDVarCTest, ReadFileConstructor) {

	file = new TFile(testfile.c_str(), "READ");

	ASSERT_TRUE(file);

	ASSERT_FALSE(file->IsZombie());

	ASSERT_NO_THROW(MAUS::recon::global::ComPIDVarC testComPIDVarC(file, "test",  0, 8000));

	ASSERT_ANY_THROW(MAUS::recon::global::ComPIDVarC testComPIDVarC(NULL, "test", 0, 8000));

	ASSERT_ANY_THROW(MAUS::recon::global::ComPIDVarC testComPIDVarC(file, "sasquatch", 0, 8000));
}

/* N.B. if either FillHist or LogL are failing, check the values of min and max
*	 bin in ComPIDVarC.hh, as if the testTracks fall outside of this range FillHist 
*  and LogL will fail.
*/

TEST_F(ComPIDVarCTest, FillHist) {

	MAUS::recon::global::ComPIDVarC testComPIDVarC("test", "test");
	for (int i = 0; i < 1000; i++) {
		testComPIDVarC.Fill_Hist(testTracks[i]);
	}
	testfile = testComPIDVarC.Get_filename();
	testdir = testComPIDVarC.Get_directory();
	// check histogram exists
	hist = testComPIDVarC.Get_hist();
	ASSERT_TRUE(hist);
	// check number of entries
	ASSERT_EQ(1000, hist->GetEntries());
}

TEST_F(ComPIDVarCTest, LogL) {
	// Constructor called within parentheses so that destructor will be
	// called when it goes out of scope
	 {
		MAUS::recon::global::ComPIDVarC writetestComPIDVarC("test", "test");
		for (int i = 0; i < 1000; i++) {
			writetestComPIDVarC.Fill_Hist(testTracks[i]);
		}
		testfile = writetestComPIDVarC.Get_filename();
		testdir = writetestComPIDVarC.Get_directory();
	 }

	MAUS::DataStructure::Global::Track* checkTrack =
			new MAUS::DataStructure::Global::Track();
	MAUS::DataStructure::Global::TrackPoint* ctpKL =
			new MAUS::DataStructure::Global::TrackPoint();
	ctpKL->set_detector(MAUS::DataStructure::Global::kCalorimeter);

	double adc_charge_prod = 1000;
	ctpKL->set_ADC_charge_product(adc_charge_prod);

	ctpKL->set_mapper_name("MapCppGlobalTrackMatching-Through");
	checkTrack->AddTrackPoint(ctpKL);
	checkTrack->set_mapper_name("MapCppGlobalTrackMatching-Through");

	file = new TFile(testfile.c_str(), "READ");

	ASSERT_TRUE(file);

	ASSERT_FALSE(file->IsZombie());

	ASSERT_NO_THROW(MAUS::recon::global::ComPIDVarC readtestComPIDVarC(file, "test", 0, 8000));

	MAUS::recon::global::ComPIDVarC readtestComPIDVarC(file, "test", 0, 8000);

	hist = readtestComPIDVarC.Get_hist();
	ASSERT_TRUE(hist);
	// check number of entries in histogram
	// NB as destructor was called above, the number of entries in the histogram
	// will be increased by the number of bins (inc over and underflow) as in
	// ComPIDVarC the behaviour to add one event across all bins is turned on.
	ASSERT_EQ((1000 + hist->GetSize()), hist->GetEntries());
	double prob = readtestComPIDVarC.logL(checkTrack);

	ASSERT_LE(prob, 0);
	ASSERT_TRUE(IsFiniteNumber(prob));
}
