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

#include "Recon/Global/ComPIDVarB.hh"



TEST(ComPIDVarBTestSetUp, TestSetUpAndTearDown) {
	MAUS::recon::global::ComPIDVarB *testComPIDVarB = NULL;
	ASSERT_NO_THROW(testComPIDVarB = new MAUS::recon::global::ComPIDVarB("test",
								       "test"));
	std::string testfile = testComPIDVarB->Get_filename();
	std::string testdir = testComPIDVarB->Get_directory();
	ASSERT_NO_THROW(delete testComPIDVarB);

	EXPECT_EQ(gSystem->Unlink(testfile.c_str()), 0);
        // Unlink dir does not seem to work on some systems
        // suspect NFS mounts or stale handles
        // force system removal
        std::string rmdirCmd = "rm -fr ";
        rmdirCmd += testdir.c_str();
        EXPECT_EQ(system(rmdirCmd.c_str()), 0);
}

class ComPIDVarBTest : public ::testing::Test {

	virtual void SetUp() {
		MAUS::recon::global::ComPIDVarB testComPIDVarB("test", "test");
		testfile = testComPIDVarB.Get_filename();
		testdir = testComPIDVarB.Get_directory();
		TRandom3 r;
		for (int i = 0; i < 1000; i++) {
		  MAUS::DataStructure::Global::Track* testTrack =
		    new MAUS::DataStructure::Global::Track();
		  MAUS::DataStructure::Global::TrackPoint* tp1 =
		    new MAUS::DataStructure::Global::TrackPoint();
		  MAUS::DataStructure::Global::TrackPoint* tp2 =
		    new MAUS::DataStructure::Global::TrackPoint();
		  MAUS::DataStructure::Global::TrackPoint* tpKL =
		    new MAUS::DataStructure::Global::TrackPoint();
		  tpKL->set_detector(MAUS::DataStructure::Global::kCalorimeter);
		  tp1->set_detector(MAUS::DataStructure::Global::kTOF1);
		  tp2->set_detector(MAUS::DataStructure::Global::kTOF2);

		  double adc_charge_prod = r.Gaus(1000, 10);
		  tpKL->set_ADC_charge_product(adc_charge_prod);

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
		  tpKL->set_mapper_name("MapCppGlobalTrackMatching-Through");
		  testTrack->AddTrackPoint(tpKL);
		  testTrack->AddTrackPoint(tp1);
		  testTrack->AddTrackPoint(tp2);
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

TEST_F(ComPIDVarBTest, CreateFileConstructor) {

	ASSERT_TRUE(fileExists(testfile));
}

TEST_F(ComPIDVarBTest, ReadFileConstructor) {

	file = new TFile(testfile.c_str(), "READ");

	ASSERT_TRUE(file);

	ASSERT_FALSE(file->IsZombie());

	ASSERT_NO_THROW(MAUS::recon::global::ComPIDVarB
			testComPIDVarB(file, "test", 20, 40, 0, 8000));

	ASSERT_ANY_THROW(MAUS::recon::global::ComPIDVarB
			 testComPIDVarB(NULL, "test", 20, 40, 0, 8000));

	ASSERT_ANY_THROW(MAUS::recon::global::ComPIDVarB
			 testComPIDVarB(file, "sasquatch", 20, 40, 0, 8000));
}

/* N.B. if either FillHist or LogL are failing, check the values of min and max
*	 bin in ComPIDVarB.hh, as if the testTracks fall outside of this range FillHist 
*  and LogL will fail.
*/

TEST_F(ComPIDVarBTest, FillHist) {

	MAUS::recon::global::ComPIDVarB testComPIDVarB("test", "test");
	for (int i = 0; i < 1000; i++) {
		testComPIDVarB.Fill_Hist(testTracks[i]);
	}
	testfile = testComPIDVarB.Get_filename();
	testdir = testComPIDVarB.Get_directory();
	// check histogram exists
	hist = testComPIDVarB.Get_hist();
	ASSERT_TRUE(hist);
	// check number of entries
	ASSERT_EQ(1000, hist->GetEntries());
}

TEST_F(ComPIDVarBTest, LogL) {
	// Constructor called within parentheses so that destructor will be
	// called when it goes out of scope
	 {
		MAUS::recon::global::ComPIDVarB writetestComPIDVarB("test", "test");
		for (int i = 0; i < 1000; i++) {
			writetestComPIDVarB.Fill_Hist(testTracks[i]);
		}
		testfile = writetestComPIDVarB.Get_filename();
		testdir = writetestComPIDVarB.Get_directory();
	 }

	MAUS::DataStructure::Global::Track* checkTrack =
			new MAUS::DataStructure::Global::Track();
	MAUS::DataStructure::Global::TrackPoint* ctp1 =
			new MAUS::DataStructure::Global::TrackPoint();
	MAUS::DataStructure::Global::TrackPoint* ctp2 =
			new MAUS::DataStructure::Global::TrackPoint();
	MAUS::DataStructure::Global::TrackPoint* ctpKL =
			new MAUS::DataStructure::Global::TrackPoint();
	ctpKL->set_detector(MAUS::DataStructure::Global::kCalorimeter);
	ctp1->set_detector(MAUS::DataStructure::Global::kTOF1);
	ctp2->set_detector(MAUS::DataStructure::Global::kTOF2);

	double adc_charge_prod = 1000;
	ctpKL->set_ADC_charge_product(adc_charge_prod);

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

	ctp1->set_position(cpos1);
	ctp2->set_position(cpos2);
	ctpKL->set_mapper_name("MapCppGlobalTrackMatching-Through");
	checkTrack->AddTrackPoint(ctpKL);
	ctp1->set_mapper_name("MapCppGlobalTrackMatching-Through");
	ctp2->set_mapper_name("MapCppGlobalTrackMatching-Through");
	checkTrack->AddTrackPoint(ctp1);
	checkTrack->AddTrackPoint(ctp2);
	checkTrack->set_mapper_name("MapCppGlobalTrackMatching-Through");

	file = new TFile(testfile.c_str(), "READ");

	ASSERT_TRUE(file);

	ASSERT_FALSE(file->IsZombie());

	ASSERT_NO_THROW(MAUS::recon::global::ComPIDVarB readtestComPIDVarB(file, "test", 30, 50, 0, 8000));

	MAUS::recon::global::ComPIDVarB readtestComPIDVarB(file, "test", 30, 50, 0, 8000);

	hist = readtestComPIDVarB.Get_hist();
	ASSERT_TRUE(hist);
	// check number of entries in histogram
	// NB as destructor was called above, the number of entries in the histogram
	// will be increased by the number of bins (inc over and underflow) as in
	// ComPIDVarB the behaviour to add one event across all bins is turned on.
	ASSERT_EQ((1000 + hist->GetSize()), hist->GetEntries());
	double prob = readtestComPIDVarB.logL(checkTrack);

	ASSERT_LE(prob, 0);
	ASSERT_TRUE(IsFiniteNumber(prob));
}
