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


// MAUS headers
#include "src/common_cpp/Analysis/SciFi/SciFiDisplayMomentumResiduals.hh"
#include "src/common_cpp/Plotting/SciFi/SciFiAnalysisTools.hh"
#include "src/common_cpp/Recon/SciFi/SciFiLookup.hh"
#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/DataStructure/MCEvent.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/SciFiHelicalPRTrack.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiCluster.hh"
#include "src/common_cpp/DataStructure/SciFiDigit.hh"
#include "src/common_cpp/DataStructure/Hit.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"

namespace MAUS {

SciFiDisplayMomentumResiduals::SciFiDisplayMomentumResiduals() : mTree(NULL),
                                                                 mData(NULL) {
  // Do nothing
}

SciFiDisplayMomentumResiduals::~SciFiDisplayMomentumResiduals() {
  if (mTree) delete mTree;
}

void SciFiDisplayMomentumResiduals::Plot(TCanvas* aCanvas) {
  std::cerr << "I am a placeholder\n";
}

SciFiDataBase* SciFiDisplayMomentumResiduals::SetUp() {
  SciFiDataBase* data = SetUpSciFiData();
  SetUpRoot();
  return data;
}

SciFiDataBase* SciFiDisplayMomentumResiduals::SetUpSciFiData() {
  mData = new SciFiDataMomentum();
  return mData;
}

bool SciFiDisplayMomentumResiduals::SetUpRoot() {
  if (mData) {
    mTree = new TTree("mTree", "SciFi Momentum Data");
    mTree->Branch("SpillNumber", &mData->mDataStruct.SpillNumber, "SpillNumber/I");
    mTree->Branch("TrackerNumber", &mData->mDataStruct.TrackerNumber, "TrackerNumber/I");
    mTree->Branch("NumberOfPoints", &mData->mDataStruct.NumberOfPoints, "NumberOfPoints/I");
    mTree->Branch("Charge", &mData->mDataStruct.Charge, "Charge/I");
    mTree->Branch("PtMc", &mData->mDataStruct.PtMc, "PtMc/D");
    mTree->Branch("PzMc", &mData->mDataStruct.PzMc, "PtMz/D");
    mTree->Branch("PtRec", &mData->mDataStruct.PtRec, "PtRec/D");
    mTree->Branch("PzRec", &mData->mDataStruct.PzRec, "PtRec/D");
    return true;
  } else {
    std::cerr << "SciFiDisplayMomentumResiduals: mData not set, please set and try again\n";
    return false;
  }
}

void SciFiDisplayMomentumResiduals::Update() {
  if (mTree) {
    mTree->Fill();
  } else {
    std::cerr << "SciFiDisplayMomentumResiduals: Warning, local ROOT Tree not setup\n";
  }
}

}
