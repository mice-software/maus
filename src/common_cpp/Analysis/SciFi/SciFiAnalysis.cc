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

SciFiAnalysis::SciFiAnalysis() : mSciFiData(NULL) {
  // Do nothing
}

SciFiAnalysis::~SciFiAnalysis() {
  if (mSciFiData) delete mSciFiData;
}

bool SciFiAnalysis::Accumulate(Spill* aSpill) {
  // Check the spill pointer
  std::cerr << "SciFiAnalysis: Checking spill pointer...\n";
  if ( !aSpill ) {
    std::cerr << "SciFiAnalysis: Warning: Empty Spill pointer passed\n";
    return false;
  } else if ( aSpill->GetDaqEventType() != "physics_event" ) {
    std::cerr << "SciFiAnalysis: Spill is not a physics event, skipping\n";
    return false;
  } else if ( !mSciFiData ) {
     std::cerr << "SciFiAnalysis: Warning: SciFiData member not set up\n";
    return false;
  }
  std::cerr << "Pass\n";

  // Reduce the SciFi Data and store in the SciFiData object
  mSciFiData->Process(aSpill);
  std::cerr << "SciFiAnalysis: Accumulate finished\n";

  // Update each of the displays objects added
  std::vector<SciFiDisplayBase*>::iterator lDisplay;
  std::cerr << "SciFiAnalysis: Looping over displays...\n";
  for ( size_t i = 0; i < mDisplays.size(); ++i ) {
    std::cerr << "SciFiAnalysis: Updating display...\n";
    mDisplays[i]->Update();
    std::cerr << "SciFiAnalysis: Updating display complete\n";
  }
  std::cerr << "SciFiAnalysis: Displays loop finished\n";
  return true;
}

void SciFiAnalysis::Plot() {
  std::vector<SciFiDisplayBase*>::iterator lDisplay;
  for ( size_t i = 0; i < mDisplays.size(); ++i ) {
    mDisplays[i]->Plot();
  }
}

void SciFiAnalysis::SetUpDisplays() {
  if (mSciFiData) delete mSciFiData;

  for ( size_t i = 0; i < mDisplays.size(); ++i ) {
    mSciFiData = mDisplays[i]->SetUp();
  }
}

} // ~namespace MAUS
