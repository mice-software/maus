// MAUS WARNING: THIS IS LEGACY CODE.
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

#include <string>
#include <iostream>

#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"
#include "src/common_cpp/Simulation/MAUSTrackingAction.hh"

namespace MAUS {

MAUSTrackingAction::MAUSTrackingAction() : _tracks(), _keepTracks(true),
                                           _stepping(NULL) {
    Json::Value& conf = *Globals::GetInstance()->GetConfigurationCards();
    _keepTracks = JsonWrapper::GetProperty
                 (conf, "keep_tracks", JsonWrapper::booleanValue).asBool() ||
                  JsonWrapper::GetProperty
                 (conf, "keep_steps", JsonWrapper::booleanValue).asBool();
}


void MAUSTrackingAction::PreUserTrackingAction(const G4Track* aTrack) {
    if (_keepTracks) {
        if (_stepping == NULL) {
           _stepping = MAUSGeant4Manager::GetInstance()->GetStepping();
        }

        Track track;
        ThreeVector pos(aTrack->GetPosition().x(),
                        aTrack->GetPosition().y(),
                        aTrack->GetPosition().z());
        track.SetInitialPosition(pos);
        ThreeVector mom(aTrack->GetMomentum().x(),
                        aTrack->GetMomentum().y(),
                        aTrack->GetMomentum().z());
        track.SetInitialMomentum(mom);
        track.SetParticleId(aTrack->GetDefinition()->GetPDGEncoding());
        track.SetTrackId(aTrack->GetTrackID());
        track.SetParentTrackId(aTrack->GetParentID());
        track.SetKillReason("");
        _tracks->push_back(track);
    }
}

void MAUSTrackingAction::PostUserTrackingAction(const G4Track* aTrack) {
    if (_keepTracks && aTrack) {
        if (_tracks->back().GetTrackId() != aTrack->GetTrackID()) {
            throw MAUS::Exception(Exception::recoverable,
                         "Track ID misalignment",
                         "MAUSTrackingAction::PostUserTrackingAction");
        }

        std::cerr << "MAUSTrackingAction PostUser1 " << aTrack->GetPosition() << " " << aTrack->GetMomentum() << std::endl;
        ThreeVector pos(aTrack->GetPosition().x(),
                        aTrack->GetPosition().y(),
                        aTrack->GetPosition().z());
        _tracks->back().SetFinalPosition(pos);
        ThreeVector mom(aTrack->GetMomentum().x(),
                        aTrack->GetMomentum().y(),
                        aTrack->GetMomentum().z());
        _tracks->back().SetFinalMomentum(mom);

        if (_stepping->GetWillKeepSteps()) {
            _tracks->back().SetSteps(_stepping->TakeSteps());
        }
    }
}

void MAUSTrackingAction::SetTracks(std::vector<Track>* tracks) {
    if (_tracks != NULL)
        delete _tracks;
    _tracks = tracks;
}

void MAUSTrackingAction::SetKillReason
                                  (const G4Track* aTrack, std::string reason) {
    for (size_t i = 0; i < _tracks->size(); ++i) {
        if ((*_tracks)[i].GetTrackId() == aTrack->GetTrackID()) {
            (*_tracks)[i].SetKillReason(reason);
        }
    }
}

}  //  ends MAUS namespace
