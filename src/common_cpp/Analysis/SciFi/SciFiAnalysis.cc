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

SciFiAnalysis::SciFiAnalysis( std::vector<SciFiDataBase*> aSciFiData) : mSciFiData(aSciFiData) {
  // Do nothing
}

SciFiAnalysis::~SciFiAnalysis() {
  // Do nothing
}

bool SciFiAnalysis::Accumulate(Spill* aSpill)
{
  if ( !aSpill ) {
    std::cerr << "WARNING: Empty Spill pointer passed";
    return false;
  }

  mSciFiData->Accumulate(aSpill);
  return true;
}

void SciFiAnalysis::Plot() {
  std::vector<SciFiDisplayBase*>::iterator lDisplay;
  for ( mDisplays.begin(); lDisplay != mDisplays.end(); ++ lDisplay ) {
   (*lDisplay)->Plot(mSciFiData);
  }
}



} // ~namespace MAUS
