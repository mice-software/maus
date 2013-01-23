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
 */

#include <algorithm>
#include "src/common_cpp/DataStructure/GlobalEvent.hh"


namespace MAUS {


GlobalEvent::GlobalEvent() :
    _primarychains(NULL), _tracks(NULL),
    _trackpoints(NULL), _spacepoints(NULL) {
  _primarychains = new GlobalPrimaryChainPArray();
  _tracks        = new GlobalTrackPArray();
  _trackpoints   = new GlobalTrackPointPArray();
  _spacepoints   = new GlobalSpacePointPArray();
}

GlobalEvent::GlobalEvent(const GlobalEvent& globalevent) :
    _primarychains(NULL), _tracks(NULL),
    _trackpoints(NULL), _spacepoints(NULL) {
  *this = globalevent;
}

GlobalEvent& GlobalEvent::operator=(const GlobalEvent& globalevent) {
  if (this == &globalevent) {
    return *this;
  }

  if (_primarychains != NULL) {
    if(!_primarychains->empty())
      for(size_t i = 0; i < _primarychains->size(); ++i)
        delete _primarychains->at(i);
    delete _primarychains;
  }
  if (globalevent._primarychains == NULL) {
    _primarychains = NULL;
  } else {
    _primarychains = new GlobalPrimaryChainPArray();
    GlobalPrimaryChainPArray* old_primarychain =
        globalevent._primarychains;
    for(size_t i = 0; i < old_primarychain->size(); ++i)
      _primarychains->push_back(old_primarychain->at(i)->Clone());
  }

  if (_tracks != NULL) {
    for(size_t i = 0; i < _tracks->size(); ++i)
      delete _tracks->at(i);
    delete _tracks;
  }
  if (globalevent._tracks == NULL) {
    _tracks = NULL;
  } else {
    _tracks = new GlobalTrackPArray();
    GlobalTrackPArray* old_tracks =
        globalevent._tracks;
    for(size_t i = 0; i < old_tracks->size(); ++i)
      _tracks->push_back(old_tracks->at(i)->Clone());
  }

  if (_trackpoints != NULL) {
    for(size_t i = 0; i < _trackpoints->size(); ++i)
      delete _trackpoints->at(i);
    delete _trackpoints;
  }
  if (globalevent._trackpoints == NULL) {
    _trackpoints = NULL;
  } else {
    _trackpoints = new GlobalTrackPointPArray();
    GlobalTrackPointPArray* old_trackpoints =
        globalevent._trackpoints;
    for(size_t i = 0; i < old_trackpoints->size(); ++i)
      _trackpoints->push_back(old_trackpoints->at(i)->Clone());
  }

  if (_spacepoints != NULL) {
    for(size_t i = 0; i < _spacepoints->size(); ++i)
      delete _spacepoints->at(i);
    delete _spacepoints;
  }
  if (globalevent._spacepoints == NULL) {
    _spacepoints = NULL;
  } else {
    _spacepoints = new GlobalSpacePointPArray();
    GlobalSpacePointPArray* old_spacepoints =
        globalevent._spacepoints;
    for(size_t i = 0; i < old_spacepoints->size(); ++i)
      _spacepoints->push_back(old_spacepoints->at(i)->Clone());
  }

  return *this;
}

GlobalEvent::~GlobalEvent() {
  if (_primarychains != NULL) {
    for(size_t i = 0; i < _primarychains->size(); ++i){
      delete _primarychains->at(i);
    }
    delete _primarychains;
  }

  if (_tracks != NULL) {
    for(size_t i = 0; i < _tracks->size(); ++i){
      delete _tracks->at(i);
    }
    delete _tracks;
  }

  if (_trackpoints != NULL) {
    for(size_t i = 0; i < _trackpoints->size(); ++i){
      delete _trackpoints->at(i);
    }
    delete _trackpoints;
  }

  if (_spacepoints != NULL) {
    for(size_t i = 0; i < _spacepoints->size(); ++i){
      delete _spacepoints->at(i);
    }
    delete _spacepoints;
  }
  
}

bool GlobalEvent::add_primarychain_check(MAUS::GlobalPrimaryChain* pchain) {
  // Check if the provided pchain matches an existing entry (this is
  // to save repeated entries and wasted disk space).
  MAUS::GlobalPrimaryChainPArray::iterator pc_iter =
      std::find(_primarychains->begin(), _primarychains->end(), pchain);
  
  bool exists = (pc_iter != _primarychains->end());
  if(!exists)
    add_primarychain(pchain);

  return exists;
}

void GlobalEvent::add_primarychain_recursive(MAUS::GlobalPrimaryChain* pchain) {
  // Add the primary chain, checking if it already exists in chain.
  bool already_added = add_primarychain_check(pchain);

  // If the chain had been added, then we will loop over the tracks
  // and add them too.
  if(!already_added) {
    std::vector<MAUS::recon::global::TRefTrackPair*>::iterator iter_track;
    std::vector<MAUS::recon::global::TRefTrackPair*> *track_pair
        = pchain->get_track_parent_pairs();
    MAUS::GlobalTrack* track = NULL;
    for(iter_track = track_pair->begin();
        iter_track != track_pair->end(); ++iter_track){
      track = (*iter_track)->GetTrack();
      add_track_recursive(track);
    }
  }
}

bool GlobalEvent::add_track_check(MAUS::GlobalTrack* track) {
  // Check if the provided track matches an existing entry (this is
  // to save repeated entries and wasted disk space).
  MAUS::GlobalTrackPArray::iterator track_iter =
      std::find(_tracks->begin(), _tracks->end(), track);
  
  bool exists = (track_iter != _tracks->end());
  if(!exists)
    add_track(track);

  return exists;
}

void GlobalEvent::add_track_recursive(MAUS::GlobalTrack* track) {
  // Add the Track, checking if it already exists in the GlobalEvent.
  bool already_added = add_track_check(track);

  // If the chain had been added, then we will loop over the
  // constituent tracks and trackpoints, adding them too.
  if(!already_added) {
    // Constituent Tracks
    MAUS::GlobalTrack* ct = NULL;
    for(int i = 0; i < track->get_constituent_tracks()->GetLast() + 1; ++i) {
      ct = (MAUS::GlobalTrack*) track->get_constituent_tracks()->At(i);
      if(!ct) continue;
      add_track_recursive(ct);
    }

    // TrackPoints
    MAUS::GlobalTrackPoint* tp = NULL;
    for(int i = 0; i < track->get_trackpoints()->GetLast() + 1; ++i) {
      tp = (MAUS::GlobalTrackPoint*) track->get_trackpoints()->At(i);
      if(!tp) continue;
      add_trackpoint_recursive(tp);
    }
  }
}

bool GlobalEvent::add_trackpoint_check(MAUS::GlobalTrackPoint* trackpoint) {
  // Check if the provided trackpoint matches an existing entry (this is
  // to save repeated entries and wasted disk space).
  MAUS::GlobalTrackPointPArray::iterator trackpoint_iter =
      std::find(_trackpoints->begin(), _trackpoints->end(), trackpoint);
  
  bool exists = (trackpoint_iter != _trackpoints->end());
  if(!exists)
    add_trackpoint(trackpoint);

  return exists;
}

void GlobalEvent::add_trackpoint_recursive(MAUS::GlobalTrackPoint* trackpoint) {
  // Add the TrackPoint, checking if it already exists in the GlobalEvent.
  bool already_added = add_trackpoint_check(trackpoint);

  // If the chain had been added, then we will add the constituent
  // spacepoint.
  if(!already_added) {
    // SpacePoints
    add_spacepoint_check(trackpoint->get_spacepoint());
  }
}

bool GlobalEvent::add_spacepoint_check(MAUS::GlobalSpacePoint* spacepoint) {
  // Check if the provided spacepoint matches an existing entry (this is
  // to save repeated entries and wasted disk space).
  bool exists = false;

  for(unsigned int i = 0; i < _spacepoints->size(); ++i) {
    MAUS::GlobalSpacePoint* test = _spacepoints->at(i);
    if(test == spacepoint){
      exists = true;
      break;
    }
  }
  
  if(!exists)
    add_spacepoint(spacepoint);

  return exists;
}

}

