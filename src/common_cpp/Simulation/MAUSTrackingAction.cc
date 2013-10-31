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

        Json::Value position(Json::objectValue);
        position["x"] = aTrack->GetPosition().x();
        position["y"] = aTrack->GetPosition().y();
        position["z"] = aTrack->GetPosition().z();

        Json::Value momentum(Json::objectValue);
        momentum["x"] = aTrack->GetMomentum().x();
        momentum["y"] = aTrack->GetMomentum().y();
        momentum["z"] = aTrack->GetMomentum().z();


       // Json::Value spin(Json::objectValue);
       // spin["x"] = aTrack->GetSpin().x();
       // spin["y"] = aTrack->GetSpin().y();
       // spin["z"] = aTrack->GetSpin().z();

        Json::Value json_track(Json::objectValue);
        json_track["initial_position"] = position;
        json_track["initial_momentum"] = momentum;
     //   json_track["initial_spin"] = spin;        
        json_track["particle_id"] = aTrack->GetDefinition()->GetPDGEncoding();
        json_track["track_id"] = aTrack->GetTrackID();
        json_track["parent_track_id"] = aTrack->GetParentID();
        json_track["kill_reason"] = "";
        if (_stepping->GetWillKeepSteps())
            _stepping->SetSteps(Json::Value(Json::arrayValue));
        _tracks.append(json_track);
    }
}

void MAUSTrackingAction::PostUserTrackingAction(const G4Track* aTrack) {
    if (_keepTracks && aTrack) {
        Json::Value json_track = _tracks[_tracks.size()-1];
        if (json_track["track_id"] != aTrack->GetTrackID()) {
            throw Squeal(Squeal::recoverable,
                         "Track ID misalignment",
                         "MAUSTrackingAction::PostUserTrackingAction");
        }

        Json::Value position;
        position["x"] = aTrack->GetPosition().x();
        position["y"] = aTrack->GetPosition().y();
        position["z"] = aTrack->GetPosition().z();

        Json::Value momentum;
        momentum["x"] = aTrack->GetMomentum().x();
        momentum["y"] = aTrack->GetMomentum().y();
        momentum["z"] = aTrack->GetMomentum().z();


    //    Json::Value spin;
      //  spin["x"] = aTrack->GetSpin().x();
        //spin["y"] = aTrack->GetSpin().y();
        //spin["z"] = aTrack->GetSpin().z();

        json_track["final_position"] = position;
        json_track["final_momentum"] = momentum;
  //      json_track["final_spin"] = spin;
        if (_stepping->GetWillKeepSteps())
            json_track["steps"] = _stepping->GetSteps();
        _tracks[_tracks.size()-1] = json_track;
    }
}

void MAUSTrackingAction::SetTracks(Json::Value tracks) {
    if (!tracks.isArray())
        throw(Squeal(Squeal::recoverable,
              "Attempt to set tracks to non-array of type "
              +JsonWrapper::ValueTypeToString(tracks.type()),
              "MAUSTrackingAction::SetTracks()"));
    _tracks = tracks;
}

void MAUSTrackingAction::SetKillReason
                                  (const G4Track* aTrack, std::string reason) {
    for (size_t i = 0; i < _tracks.size(); ++i) {
        if (_tracks[i]["track_id"] == aTrack->GetTrackID()) {
            _tracks[i]["kill_reason"] = reason;
        }
    }
}

}  //  ends MAUS namespace
