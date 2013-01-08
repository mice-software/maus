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
#include <iostream>

#include "src/common_cpp/DataStructure/GlobalEvent.hh"


namespace MAUS {

GlobalEvent::GlobalEvent() :
    _primarychains(NULL), _tracks(NULL),
    _trackpoints(NULL), _spacepoints(NULL) {
  _primarychains = new std::vector<MAUS::recon::global::PrimaryChain*>();
  _tracks        = new std::vector<MAUS::recon::global::Track*>();
  _trackpoints   = new std::vector<MAUS::recon::global::TrackPoint*>();
  _spacepoints   = new std::vector<MAUS::recon::global::SpacePoint*>();
}

GlobalEvent::GlobalEvent(const GlobalEvent& globalevent) {
  *this = globalevent;
}

GlobalEvent& GlobalEvent::operator=(const GlobalEvent& globalevent) {
  if (this == &globalevent) {
    return *this;
  }

  if (_primarychains != NULL) {
    for(size_t i = 0; i < _primarychains->size(); ++i)
      delete _primarychains->at(i);
    delete _primarychains;
  }
  if (globalevent._primarychains == NULL) {
    _primarychains = NULL;
  } else {
    _primarychains = new std::vector<MAUS::recon::global::PrimaryChain*>
        (*globalevent._primarychains);
  }

  if (_tracks != NULL) {
    for(size_t i = 0; i < _tracks->size(); ++i)
      delete _tracks->at(i);
    delete _tracks;
  }
  if (globalevent._tracks == NULL) {
    _tracks = NULL;
  } else {
    _tracks = new std::vector<MAUS::recon::global::Track*>
        (*globalevent._tracks);
  }

  if (_trackpoints != NULL) {
    for(size_t i = 0; i < _trackpoints->size(); ++i)
      delete _trackpoints->at(i);
    delete _trackpoints;
  }
  if (globalevent._trackpoints == NULL) {
    _trackpoints = NULL;
  } else {
    _trackpoints = new std::vector<MAUS::recon::global::TrackPoint*>
        (*globalevent._trackpoints);
  }

  if (_spacepoints != NULL) {
    for(size_t i = 0; i < _spacepoints->size(); ++i)
      delete _spacepoints->at(i);
    delete _spacepoints;
  }
  if (globalevent._spacepoints == NULL) {
    _spacepoints = NULL;
  } else {
    _spacepoints = new std::vector<MAUS::recon::global::SpacePoint*>
        (*globalevent._spacepoints);
  }

  return *this;
}

GlobalEvent::~GlobalEvent() {
  if (_primarychains != NULL) {
    for(size_t i = 0; i < _primarychains->size(); ++i)
      delete _primarychains->at(i);
    delete _primarychains;
  }

  if (_tracks != NULL) {
    for(size_t i = 0; i < _tracks->size(); ++i)
      delete _tracks->at(i);
    delete _tracks;
  }

  if (_trackpoints != NULL) {
    for(size_t i = 0; i < _trackpoints->size(); ++i)
      delete _trackpoints->at(i);
    delete _trackpoints;
  }

  if (_spacepoints != NULL) {
    for(size_t i = 0; i < _spacepoints->size(); ++i)
      delete _spacepoints->at(i);
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
    _primarychains->push_back(pchain);

  return exists;
}

void GlobalEvent::add_primarychain_recursive(MAUS::GlobalPrimaryChain* pchain) {
  // Add the primary chain, checking if it already exists in chain.
  bool already_added = add_primarychain_check(pchain);

  // If the chain had been added, then we will loop over the tracks
  // and add them too.
  if(!already_added) {
    MAUS::GlobalTrackPArray::iterator iter_track;
    MAUS::GlobalTrackPArray *ctracks = pchain->get_tracks();
    for(iter_track = ctracks->begin();
        iter_track != ctracks->end(); ++iter_track){
      add_track_recursive(*iter_track);
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
    _tracks->push_back(track);

  return exists;
}

void GlobalEvent::add_track_recursive(MAUS::GlobalTrack* track) {
  // Add the Track, checking if it already exists in the GlobalEvent.
  bool already_added = add_track_check(track);

  // If the chain had been added, then we will loop over the
  // constituent tracks and trackpoints, adding them too.
  if(!already_added) {
    // Constituent Tracks
    MAUS::GlobalTrackPArray::iterator iter_track;
    MAUS::GlobalTrackPArray *ctracks = track->get_constituent_tracks();
    for(iter_track = ctracks->begin();
        iter_track != ctracks->end(); ++iter_track){
      add_track_recursive(*iter_track);
    }

    // TrackPoints
    MAUS::GlobalTrackPointPArray::const_iterator iter_tp;
    MAUS::GlobalTrackPointPArray *trackpoints = track->get_trackpoints();
    for(iter_tp = trackpoints->begin();
        iter_tp != trackpoints->end(); ++iter_tp){
      add_trackpoint_recursive(*iter_tp);
    }
  }
}

bool GlobalEvent::add_trackpoint_check(MAUS::GlobalTrackPoint* trackpoint) {
  // Check if the provided trackpoint matches an existing entry (this is
  // to save repeated entries and wasted disk space).
  MAUS::GlobalTrackPointPArray::iterator trackpoint_iter =
      std::find(_trackpoints->begin(), _trackpoints->end(), trackpoint);
  
  bool exists = (trackpoint_iter != _trackpoints->end());
  std::cout << "Adding TrackPoint? " << !exists << std::endl;
  if(!exists)
    _trackpoints->push_back(trackpoint);

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
  std::cout << "Adding SpacePoint:\t" << spacepoint << std::endl;

  bool exists = false;

  for(unsigned int i = 0; i < _spacepoints->size(); ++i) {
    MAUS::GlobalSpacePoint* test = _spacepoints->at(i);
    std::cout << "Checking\t" << i << "\t" << test << std::endl;
    if(test == spacepoint){
      exists = true;
      break;
    }
  }
  
  std::cout << "Already added?\t" << exists << std::endl;
  
  if(!exists)
    _spacepoints->push_back(spacepoint);

  std::cout << "Done" << std::endl;
  
  return exists;
}

}

