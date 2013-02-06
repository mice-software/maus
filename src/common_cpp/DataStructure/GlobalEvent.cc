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
  _primarychains =
      new std::vector<MAUS::DataStructure::Global::PrimaryChain*>();
  _tracks        = new std::vector<MAUS::DataStructure::Global::Track*>();
  _trackpoints   = new std::vector<MAUS::DataStructure::Global::TrackPoint*>();
  _spacepoints   = new std::vector<MAUS::DataStructure::Global::SpacePoint*>();
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
    _primarychains =
        new std::vector<MAUS::DataStructure::Global::PrimaryChain*>();
    std::vector<MAUS::DataStructure::Global::PrimaryChain*>* old_primarychain =
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
    _tracks = new std::vector<MAUS::DataStructure::Global::Track*>();
    std::vector<MAUS::DataStructure::Global::Track*>* old_tracks =
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
    _trackpoints = new std::vector<MAUS::DataStructure::Global::TrackPoint*>();
    std::vector<MAUS::DataStructure::Global::TrackPoint*>* old_trackpoints =
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
    _spacepoints = new std::vector<MAUS::DataStructure::Global::SpacePoint*>();
    std::vector<MAUS::DataStructure::Global::SpacePoint*>* old_spacepoints =
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

bool GlobalEvent::add_primarychain_check(
    MAUS::DataStructure::Global::PrimaryChain* pchain) {
  // Check if the provided pchain matches an existing entry (this is
  // to save repeated entries and wasted disk space).
  std::vector<MAUS::DataStructure::Global::PrimaryChain*>::iterator pc_iter =
      std::find(_primarychains->begin(), _primarychains->end(), pchain);
  
  bool exists = (pc_iter != _primarychains->end());
  if(!exists)
    add_primarychain(pchain);

  return exists;
}

void GlobalEvent::add_primarychain_recursive(
    MAUS::DataStructure::Global::PrimaryChain* pchain) {
  // Add the primary chain, checking if it already exists in chain.
  bool already_added = add_primarychain_check(pchain);

  // If the chain had been added, then we will loop over the tracks
  // and add them too.
  if(!already_added) {
    std::vector<MAUS::DataStructure::Global::TRefTrackPair*>::iterator
        iter_track;
    std::vector<MAUS::DataStructure::Global::TRefTrackPair*> *track_pair
        = pchain->get_track_parent_pairs();
    MAUS::DataStructure::Global::Track* track = NULL;
    for(iter_track = track_pair->begin();
        iter_track != track_pair->end(); ++iter_track){
      track = (*iter_track)->GetTrack();
      add_track_recursive(track);
    }
  }
}

bool GlobalEvent::add_track_check(MAUS::DataStructure::Global::Track* track) {
  // Check if the provided track matches an existing entry (this is
  // to save repeated entries and wasted disk space).
  std::vector<MAUS::DataStructure::Global::Track*>::iterator track_iter =
      std::find(_tracks->begin(), _tracks->end(), track);
  
  bool exists = (track_iter != _tracks->end());
  if(!exists)
    add_track(track);

  return exists;
}

void GlobalEvent::add_track_recursive(
    MAUS::DataStructure::Global::Track* track) {
  // Add the Track, checking if it already exists in the GlobalEvent.
  bool already_added = add_track_check(track);

  // If the chain had been added, then we will loop over the
  // constituent tracks and trackpoints, adding them too.
  if(!already_added) {
    // Constituent Tracks
    MAUS::DataStructure::Global::Track* ct = NULL;
    for(int i = 0; i < track->get_constituent_tracks()->GetLast() + 1; ++i) {
      ct = (MAUS::DataStructure::Global::Track*)
          track->get_constituent_tracks()->At(i);
      if(!ct) continue;
      add_track_recursive(ct);
    }

    // TrackPoints
    MAUS::DataStructure::Global::TrackPoint* tp = NULL;
    for(int i = 0; i < track->get_trackpoints()->GetLast() + 1; ++i) {
      tp = (MAUS::DataStructure::Global::TrackPoint*)
          track->get_trackpoints()->At(i);
      if(!tp) continue;
      add_trackpoint_recursive(tp);
    }
  }
}

bool GlobalEvent::add_trackpoint_check(
    MAUS::DataStructure::Global::TrackPoint* trackpoint) {
  // Check if the provided trackpoint matches an existing entry (this is
  // to save repeated entries and wasted disk space).
  std::vector<MAUS::DataStructure::Global::TrackPoint*>::iterator
      trackpoint_iter = std::find(_trackpoints->begin(),
                                  _trackpoints->end(),
                                  trackpoint);
  
  bool exists = (trackpoint_iter != _trackpoints->end());
  if(!exists)
    add_trackpoint(trackpoint);

  return exists;
}

void GlobalEvent::add_trackpoint_recursive(
    MAUS::DataStructure::Global::TrackPoint* trackpoint) {
  // Add the TrackPoint, checking if it already exists in the GlobalEvent.
  bool already_added = add_trackpoint_check(trackpoint);

  // If the chain had been added, then we will add the constituent
  // spacepoint.
  if(!already_added) {
    // SpacePoints
    add_spacepoint_check(trackpoint->get_spacepoint());
  }
}

bool GlobalEvent::add_spacepoint_check(
    MAUS::DataStructure::Global::SpacePoint* spacepoint) {
  // Check if the provided spacepoint matches an existing entry (this is
  // to save repeated entries and wasted disk space).
  bool exists = false;

  for(unsigned int i = 0; i < _spacepoints->size(); ++i) {
    MAUS::DataStructure::Global::SpacePoint* test = _spacepoints->at(i);
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

