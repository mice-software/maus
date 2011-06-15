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

#include <iostream>

#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"
#include "src/common_cpp/Simulation/MAUSTrackingAction.hh"

namespace MAUS {

MAUSTrackingAction::MAUSTrackingAction() : _tracks(), _keepTracks(true),
                                           _stepping(NULL) {
    Json::Value& conf = *MICERun::getInstance()->jsonConfiguration;
    _keepTracks = JsonWrapper::GetProperty
                 (conf, "keep_tracks", JsonWrapper::booleanValue).asBool();

}


void MAUSTrackingAction::PreUserTrackingAction(const G4Track* aTrack) {
    if (_keepTracks) {
        if (_stepping == NULL) {
           _stepping = MAUSGeant4Manager::GetInstance()->GetStepping();
        }

        // I store tracks as an object type rather than array type because I am
        // not convinced that TrackID increments by 1 each time
        std::string name = TrackName(aTrack->GetTrackID());
        if (_tracks[name].type() != Json::nullValue) {
          throw(Squeal(Squeal::recoverable,
                       "Json "+name+" already exists",
                       "MAUSTrackingAction::PreUserTrackingAction"));
        }

        Json::Value position(Json::objectValue);
        position["x"] = aTrack->GetPosition().x();
        position["y"] = aTrack->GetPosition().y();
        position["z"] = aTrack->GetPosition().z();

        Json::Value momentum(Json::objectValue);
        momentum["x"] = aTrack->GetMomentum().x();
        momentum["y"] = aTrack->GetMomentum().y();
        momentum["z"] = aTrack->GetMomentum().z();

        Json::Value json_track(Json::objectValue);
        json_track["initial_position"] = position;
        json_track["initial_momentum"] = momentum;
        json_track["particle_id"] = aTrack->GetDefinition()->GetPDGEncoding();
        json_track["track_id"] = aTrack->GetTrackID();
        json_track["parent_track_id"] = aTrack->GetParentID();
        if (_stepping->GetWillKeepSteps())
            _stepping->SetSteps(Json::Value(Json::arrayValue));
        _tracks[name] = json_track;
    }
}

void MAUSTrackingAction::PostUserTrackingAction(const G4Track* aTrack) {
    if (_keepTracks) {
        std::string name = TrackName(aTrack->GetTrackID());
        Json::Value json_track = _tracks[name];
        if (json_track.type() == Json::nullValue)
          throw(Squeal(Squeal::recoverable,
                       "Failed to find Json "+name,
                       "MAUSTrackingAction::PostUserTrackingAction"));

        Json::Value position;
        position["x"] = aTrack->GetPosition().x();
        position["y"] = aTrack->GetPosition().y();
        position["z"] = aTrack->GetPosition().z();

        Json::Value momentum;
        momentum["x"] = aTrack->GetMomentum().x();
        momentum["y"] = aTrack->GetMomentum().y();
        momentum["z"] = aTrack->GetMomentum().z();

        json_track["final_position"] = position;
        json_track["final_momentum"] = momentum;
        if (_stepping->GetWillKeepSteps())
            json_track["steps"] = _stepping->GetSteps();
        _tracks[name] = json_track;
    }
}

void MAUSTrackingAction::SetTracks(Json::Value tracks) {
    if (!tracks.isObject()) 
        throw(Squeal(Squeal::recoverable, 
              "Attempt to set tracks to non-object type",
              "MAUSTrackingAction::SetTracks()"));
    _tracks = tracks;
}

std::string MAUSTrackingAction::TrackName(int id) {
    std::stringstream ss;
    ss << "track_" << id;
    return ss.str();
}

void MAUSTrackingAction::SetKillReason(std::string reason) {
    _tracks["KillReason"] = Json::Value(reason);
}

}  //  ends MAUS namespace
