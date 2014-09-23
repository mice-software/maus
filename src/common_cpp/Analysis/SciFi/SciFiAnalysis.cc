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

bool SciFiAnalysis::Accumulate(Spill* aSpill)
{
  // Check the spill pointer
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

  // Reduce the SciFi Data and store in the SciFiData object
  mSciFiData->Accumulate(aSpill);

  // Update each of the displays objects added
  std::vector<SciFiDisplayBase*>::iterator lDisplay;
  for ( mDisplays.begin(); lDisplay != mDisplays.end(); ++lDisplay ) {
    (*lDisplay)->Update();
  }
  return true;
}

void SciFiAnalysis::Plot() {
  std::vector<SciFiDisplayBase*>::iterator lDisplay;
  for ( mDisplays.begin(); lDisplay != mDisplays.end(); ++lDisplay ) {
    (*lDisplay)->Plot();
  }
}

void SciFiAnalysis::SetUpDisplays() {
  if (mSciFiData) delete mSciFiData;

  for ( size_t i = 0; i < mDisplays.size(); ++i ) {
    mSciFiData = mDisplays[i]->SetUp();
  }
}

} // ~namespace MAUS
