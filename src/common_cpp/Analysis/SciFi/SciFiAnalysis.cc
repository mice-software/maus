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
#include "src/common_cpp/Analysis/SciFi/SciFiAnalysis.hh"

namespace MAUS {

SciFiAnalysis::SciFiAnalysis() : mSciFiData(0) {
  // Do nothing
}

SciFiAnalysis::~SciFiAnalysis() {
  for ( size_t i = 0; i < mSciFiData.size(); ++i ) {
    if (mSciFiData[i]) delete mSciFiData[i];
  }
}

bool SciFiAnalysis::Process(Spill* aSpill) {
  // Check the spill pointer
  if ( !aSpill ) {
    return false;
  } else if ( aSpill->GetDaqEventType() != "physics_event" ) {
    return false;
  }

  // Reduce the SciFi Data and store in the SciFiData object
  for ( size_t i = 0; i < mSciFiData.size(); ++i ) {
    mSciFiData[i]->Clear();
    mSciFiData[i]->Process(aSpill);
  }

  // Update each of the displays objects added
  for ( size_t i = 0; i < mDisplays.size(); ++i ) {
    mDisplays[i]->Fill();
  }
  return true;
}

void SciFiAnalysis::Plot() {
  for ( size_t i = 0; i < mDisplays.size(); ++i ) {
    mDisplays[i]->Plot();
    mDisplays[i]->GetCanvas()->Update();
  }
}

void SciFiAnalysis::Save() {
  for ( size_t i = 0; i < mDisplays.size(); ++i ) {
    mDisplays[i]->Save();
  }
}

void SciFiAnalysis::SetUpDisplays() {
  for ( size_t i = 0; i < mDisplays.size(); ++i ) {
    mSciFiData.push_back(mDisplays[i]->SetUp());
  }
}

} // ~namespace MAUS
