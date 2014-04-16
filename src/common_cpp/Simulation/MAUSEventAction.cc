/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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

#include <vector>

#include "src/common_cpp/Utils/Exception.hh"

#include "src/common_cpp/Simulation/DetectorConstruction.hh"
#include "src/common_cpp/Simulation/MAUSEventAction.hh"
#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"
#include "src/common_cpp/Simulation/MAUSTrackingAction.hh"
#include "src/common_cpp/Simulation/MAUSSteppingAction.hh"
#include "src/common_cpp/Simulation/VirtualPlanes.hh"

namespace MAUS {

MAUSEventAction::MAUSEventAction()
  : _primary(0), _g4manager(NULL), _virtPlanes(NULL), _tracking(NULL),
    _stepping(NULL), _geometry(NULL) {
}

void MAUSEventAction::BeginOfEventAction(const G4Event *anEvent) {
    if (_virtPlanes == NULL ||
        _tracking == NULL ||
        _stepping == NULL ||
        _geometry == NULL)
        throw(Exception(Exception::recoverable,
                        "EventAction::SetEvents not called",
                        "EventAction::BeginOfEventAction"));
    _virtPlanes->StartOfEvent();
    if (_tracking->GetWillKeepTracks())
        _tracking->SetTracks(Json::Value(Json::arrayValue));
    if (_stepping->GetWillKeepSteps())
        _stepping->SetSteps(Json::Value(Json::arrayValue));
    _geometry->ClearSDHits();
}

void MAUSEventAction::EndOfEventAction(const G4Event *anEvent) {
    //  For each detector i
    if (_primary >= _events.size())
        throw(Exception(Exception::recoverable,
                     "Ran out of space in event array",
                     "MAUSEventAction::EndOfEventAction"));
    for (int i = 0; i < _geometry->GetSDSize(); i++) {
      //  Retrieve detector i's hits
      Json::Value hits = _geometry->GetSDHits(i);
      _events[_primary] = JsonWrapper::ObjectMerge(_events[_primary], hits);
    }
    if (_tracking->GetWillKeepTracks())
        _events[_primary]["tracks"] = _tracking->GetTracks();
    if (_virtPlanes->GetWillUseVirtualPlanes())
        _events[_primary]["virtual_hits"] = _virtPlanes->GetVirtualHits();
    _primary++;
}

void MAUSEventAction::SetEvents(Json::Value events) {
    if (!events.isArray())
        throw(Exception(Exception::recoverable, "Particles must be an array value",
                     "MAUSEventAction::SetEvent"));
    for (size_t i = 0; i < events.size(); ++i) {
        if (!events[i].isObject())
            throw(Exception(Exception::recoverable,
                         "Each Particle must be an object value",
                         "MAUSEventAction::SetEvent"));
    }
    _events = events;
    _primary = 0;

    // get again on each new run - support dynamic changing of stuff (though
    // geant4 may not)
    _g4manager = MAUSGeant4Manager::GetInstance();
    _virtPlanes = _g4manager->GetVirtualPlanes();
    _tracking = _g4manager->GetTracking();
    _geometry = _g4manager->GetGeometry();
    _stepping = _g4manager->GetStepping();
}
}

