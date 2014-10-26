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
#include "src/common_cpp/Analysis/SciFi/SciFiDataMomentum.hh"

namespace MAUS {

SciFiDataMomentum::SciFiDataMomentum() {
  // Do nothing
}

SciFiDataMomentum::~SciFiDataMomentum() {
  // Do nothing
}

bool SciFiDataMomentum::Process(Spill* aSpill) {
  if (aSpill != NULL && aSpill->GetDaqEventType() == "physics_event") {
    // Loop over MC events in the aSpill
    for (size_t iMaSFEvent = 0; iMaSFEvent < aSpill->GetMCEvents()->size(); ++iMaSFEvent) {
      MCEvent *aMcEvent = (*aSpill->GetMCEvents())[iMaSFEvent];
      SciFiEvent *aSFEvent = (*aSpill->GetReconEvents())[iMaSFEvent]->GetSciFiEvent();
      ReduceData(aMcEvent, aSFEvent);
    }
  } else {
    std::cerr << "SciFiDataMomentum: Not a usable spill" << std::endl;
    return false;
  }
  return true;
}
}
