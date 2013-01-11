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

#include "DataStructure/GlobalPrimaryChain.hh"

#include <algorithm>
#include <utility>

#include "Interface/Squeak.hh"
// #include "DataStructure/GlobalTrackPoint.hh"

namespace MAUS {
namespace recon {
namespace global {

// Default constructor
PrimaryChain::PrimaryChain()
    : _mapper_name(""),
      _tracks(),
      _goodness_of_fit(0.) {
}

// Copy contructor
PrimaryChain::PrimaryChain(const PrimaryChain &primary_chain)
    : _mapper_name(""),
      _tracks(primary_chain.get_tracks_tref()),
      _goodness_of_fit(primary_chain.get_goodness_of_fit()) {}

// Constructor setting #_mapper_name
PrimaryChain::PrimaryChain(std::string mapper_name)
    : _mapper_name(mapper_name),
      _tracks(),
      _goodness_of_fit(0.) {
}

// Destructor
PrimaryChain::~PrimaryChain() {}

// Assignment operator
PrimaryChain& PrimaryChain::operator=(const PrimaryChain &primary_chain) {
  if (this == &primary_chain) {
    return *this;
  }
  _mapper_name        = primary_chain.get_mapper_name();
  _tracks             = primary_chain.get_tracks_tref();
  _goodness_of_fit    = primary_chain.get_goodness_of_fit();
  
  return *this;
}

// Create a new PrimaryChain, identical to the original, but separate.  All
// PrimaryChainPoints are also cloned.
PrimaryChain* PrimaryChain::Clone() const {
  MAUS::GlobalPrimaryChain* primaryChainNew =
      new MAUS::GlobalPrimaryChain(_mapper_name);

  MAUS::recon::global::Track* track;
  MAUS::recon::global::Track* parent;

  // First, we loop through all of the tracks, cloning each, and
  // placing the clones in a temporary map...
  std::map<MAUS::recon::global::Track*,
           MAUS::recon::global::Track*> tempMap;
  for(size_t i = 0; i < _tracks.size(); i++) {
    track = (MAUS::recon::global::Track*) _tracks.at(i).first.GetObject();
    tempMap[track] = track->Clone();
  }

  // Second, we loop through again.  Clones are added to the new
  // PrimaryChain, either as Primaries or with the appropriate
  // Clone...
  for(size_t i = 0; i < _tracks.size(); i++) {
    track = (MAUS::recon::global::Track*) _tracks.at(i).first.GetObject();
    if(!_tracks.at(i).second.IsValid())
      primaryChainNew->AddPrimaryTrack(tempMap[track]);
    else {
      parent = (MAUS::recon::global::Track*) _tracks.at(i).second.GetObject();
      primaryChainNew->AddTrack(tempMap[track],
                                tempMap[parent]);
    }
  }

  primaryChainNew->set_goodness_of_fit(_goodness_of_fit);

  return primaryChainNew;
}

bool PrimaryChain::AddTrack(MAUS::GlobalTrack* track,
                            MAUS::GlobalTrack* parent) {
  // Check track is valid
  if(!track) {
    Squeak::mout(Squeak::error) << "Attempting to add NULL Track"
                                << std::endl;
    return false;
  }

  // Check parent is valid.  If not, then add the track as a primary
  if(!parent)
    return AddPrimaryTrack(track);
  
  // Check parent is already in the _track vector.
  bool found = false;
  for(size_t i = 0; i < _tracks.size(); i++) {
    if(_tracks.at(i).first.GetObject() == parent) {
      found = true;
      break;
    }
  }

  if(!found) {
    Squeak::mout(Squeak::error)
        << "Proposed Parent not in Primary Chain, rejecting Track"
        << std::endl;
    return false;
  }

  // Add the track with parent
  _tracks.push_back(std::make_pair(track, parent));
  return true;
}

bool PrimaryChain::AddPrimaryTrack(MAUS::GlobalTrack* track) {
  // Check track is valid
  if(!track) {
    Squeak::mout(Squeak::error) << "Attempting to add NULL Track"
                                << std::endl;
    return false;
  }
  
  // Add the track with a NULL parent
  _tracks.push_back(std::make_pair(track, TRef(NULL)));
  return true;
}

bool PrimaryChain::RemoveTrack(MAUS::GlobalTrack* track) {
  // Check track is in the container
  bool in_tracks = false;
  for(size_t i = 0; i < _tracks.size(); i++) {
    if(_tracks.at(i).first.GetObject() == track) {
      in_tracks = true;
      break;
    }
  }
  
  // If not in _tracks, warning and return.
  if(!in_tracks) {
    Squeak::mout(Squeak::warning)
        << "Track not in PrimaryChain, can't remove"
        << std::endl;
    return false;
  }

  // If target is parent of other track, prevent removal
  bool is_parent = false;
  for(size_t i = 0; i < _tracks.size(); i++) {
    if(_tracks.at(i).second.GetObject() == track) {
      is_parent = true;
      break;
    }
  }
  
  // If track is a parent, warn and return
  if (is_parent) {
    Squeak::mout(Squeak::warning)
        << "Attempting to remove track with daughter in PrimaryChain, forbidden"
        << std::endl;
    return false;
  }

  // Remove the pair
  std::vector<std::pair<TRef, TRef> >::iterator index;
  for(index = _tracks.begin(); index < _tracks.end(); index++) {
    if(index->first.GetObject() == track) {
      _tracks.erase(index);
      break;
    }
  }
  
  return true;
}

bool PrimaryChain::HasTrack(MAUS::GlobalTrack* track) {
  // Find track in vector
  std::vector<std::pair<TRef, TRef> >::iterator index;
  for(index = _tracks.begin(); index < _tracks.end(); index++) {
    if(index->first.GetObject() == track) {
      return true;
    }
  }
  return false;
}

bool PrimaryChain::HasTrackAsParent(MAUS::GlobalTrack* parent) {
  // Find track as parent vector
  std::vector<std::pair<TRef, TRef> >::iterator index;
  for(index = _tracks.begin(); index < _tracks.end(); index++) {
    if(index->second.GetObject() == parent) {
      return true;
    }
  }
  return false;
}

bool PrimaryChain::IsPrimaryTrack(MAUS::GlobalTrack* track) {
  // Find track in vector
  std::vector<std::pair<TRef, TRef> >::iterator index;
  for(index = _tracks.begin(); index < _tracks.end(); index++) {
    if(index->first.GetObject() == track) {
      return !(index->second.IsValid());
    }
  }
  return false;
}

MAUS::GlobalTrack* PrimaryChain::GetTrackParent(MAUS::GlobalTrack* track) {
  // Find track, and return parent
  std::vector<std::pair<TRef, TRef> >::iterator index;
  for(index = _tracks.begin(); index < _tracks.end(); index++) {
    if(index->first.GetObject() == track) {
      return (MAUS::GlobalTrack*) index->second.GetObject();
    }
  }
  return NULL;
}

std::vector<MAUS::GlobalTrack*>
PrimaryChain::GetTrackDaughters(MAUS::GlobalTrack* track) {
  std::vector<MAUS::GlobalTrack*> result;

  // Find all tracks with this track as a parent
  std::vector<std::pair<TRef, TRef> >::iterator index;
  for(index = _tracks.begin(); index < _tracks.end(); index++) {
    if(index->second.GetObject() == track) {
      result.push_back((MAUS::GlobalTrack*) index->first.GetObject());
    }
  }

  return result;
}
  
void PrimaryChain::ClearTracks() {
  _tracks.clear();
}

} // ~namespace global
} // ~namespace recon
} // ~namespace MAUS
