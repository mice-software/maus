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

#include "src/common_cpp/DataStructure/Step.hh"
#include "src/common_cpp/DataStructure/Track.hh"
#include "src/common_cpp/DataStructure/MCEvent.hh"

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
        throw(Exceptions::Exception(Exceptions::recoverable,
                        "EventAction::SetEvents not called",
                        "EventAction::BeginOfEventAction"));
    _virtPlanes->StartOfEvent();
    if (_tracking->GetWillKeepTracks())
        _tracking->SetTracks(new std::vector<Track>());
    _geometry->ClearSDHits();
}

void MAUSEventAction::EndOfEventAction(const G4Event *anEvent) {
    //  For each detector i
    if (_primary >= _events->size())
        throw(Exceptions::Exception(Exceptions::recoverable,
                     "Ran out of space in event array",
                     "MAUSEventAction::EndOfEventAction"));
    _events->at(_primary)->SetSciFiHits(new std::vector<SciFiHit>());
    _events->at(_primary)->SetSciFiNoiseHits(new std::vector<SciFiNoiseHit>());
    _events->at(_primary)->SetTOFHits(new std::vector<TOFHit>());
    _events->at(_primary)->SetKLHits(new std::vector<KLHit>());
    _events->at(_primary)->SetEMRHits(new std::vector<EMRHit>());
    _events->at(_primary)->SetCkovHits(new std::vector<CkovHit>());
    _events->at(_primary)->SetSpecialVirtualHits(new std::vector<SpecialVirtualHit>());
    for (int i = 0; i < _geometry->GetSDSize(); i++) {
      //  Retrieve detector i's hits
      _geometry->GetSDHits(i, (*_events)[_primary]);
    }
    if (_tracking->GetWillKeepTracks()) {
        (*_events)[_primary]->SetTracks(_tracking->TakeTracks());
    } else {
        (*_events)[_primary]->SetTracks(new std::vector<Track>());
    }
    if (_virtPlanes->GetWillUseVirtualPlanes()) {
        (*_events)[_primary]->SetVirtualHits(_virtPlanes->TakeVirtualHits());
    } else {
        (*_events)[_primary]->SetVirtualHits(new std::vector<VirtualHit>());
    }
    _primary++;
}

void MAUSEventAction::SetEvents(std::vector<MCEvent*>* events) {
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

std::vector<MCEvent*>* MAUSEventAction::TakeEvents() {
  std::vector<MCEvent*>* event_tmp = _events;
  _events = NULL;
  return event_tmp;
}
}

