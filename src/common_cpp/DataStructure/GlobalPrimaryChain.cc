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

#include "Interface/Squeak.hh"
// #include "DataStructure/GlobalTrackPoint.hh"

namespace MAUS {
namespace recon {
namespace global {

// Default constructor
PrimaryChain::PrimaryChain()
    : _mapper_name(""),
      _goodness_of_fit(0.) {
  _tracks = new std::vector<MAUS::GlobalTrack*>();
}

// Copy contructor
PrimaryChain::PrimaryChain(const PrimaryChain &primary_chain)
    : _mapper_name(""),
      _tracks(primary_chain.get_tracks()),
      _parents(primary_chain.get_parents()),
      _goodness_of_fit(primary_chain.get_goodness_of_fit()) {}

// Constructor setting #_mapper_name
PrimaryChain::PrimaryChain(std::string mapper_name)
    : _mapper_name(mapper_name),
      _goodness_of_fit(0.) {
  _tracks = new std::vector<MAUS::GlobalTrack*>();
}

// Destructor
PrimaryChain::~PrimaryChain() {}

// Assignment operator
PrimaryChain& PrimaryChain::operator=(const PrimaryChain &primary_chain) {
  if (this == &primary_chain) {
    return *this;
  }
  _mapper_name        = primary_chain.get_mapper_name();
  _tracks             = primary_chain.get_tracks();
  _parents            = primary_chain.get_parents();
  _goodness_of_fit    = primary_chain.get_goodness_of_fit();
  
  return *this;
}

// Create a new PrimaryChain, identical to the original, but separate.  All
// PrimaryChainPoints are also cloned.
PrimaryChain* PrimaryChain::Clone() const {
  MAUS::GlobalPrimaryChain* primaryChainNew =
      new MAUS::GlobalPrimaryChain(_mapper_name);

  for(unsigned int i = 0; i < _tracks->size(); ++i) {
    primaryChainNew->AddTrack(_tracks->at(i)->Clone(), _parents[i]);
  }

  primaryChainNew->set_goodness_of_fit(_goodness_of_fit);

  return primaryChainNew;
}

// Constituent Tracks methods
int PrimaryChain::AddTrack(MAUS::GlobalTrack* track, int parent) {
  // Check track is valid
  if(!track) {
    Squeak::mout(Squeak::error) << "Attempting to add NULL Track"
                                << std::endl;
    return -2;
  }

  // Check parent signifies either a primary track being added (-1) or
  // a track already stored in the chain.
  if(parent != -1) {
    size_t index = parent;
    if(index < 0 || index >= _tracks->size()) {
      Squeak::mout(Squeak::error)
          << "Proposed Parent not in Primary Chain, rejecting Track"
          << std::endl;
      return -2;
    }
  }

  // Add track and parent simultaneously
  _tracks->push_back(track);
  _parents.push_back(parent);

  // Return index of new entry
  return (_tracks->size() - 1);
}

bool PrimaryChain::RemoveTrack(MAUS::GlobalTrack* track) {
  // Find track in vector
  std::vector<MAUS::GlobalTrack*>::iterator target;
  target = std::find(_tracks->begin(), _tracks->end(), track);

  // If target not found, warning and return.
  if(target == _tracks->end()) {
    Squeak::mout(Squeak::warning)
        << "Track not in PrimaryChain, can't remove"
        << std::endl;
    return false;
  }

  // If target is parent of other track, prevent removal
  size_t index = target - _tracks->begin();
  std::vector<int>::iterator storedparent;
  storedparent = std::find(_parents.begin(), _parents.end(), index);
  if (storedparent != _parents.end()) {
    Squeak::mout(Squeak::warning)
        << "Attempting to remove track with daughter in PrimaryChain, forbidden"
        << std::endl;
    return false;
  }

  // Can't actually remove the entry, as indices will shift!
  _tracks->at(index) = NULL;
  _parents.at(index) = -2;

  return true;
}

bool PrimaryChain::HasTrack(MAUS::GlobalTrack* track) {
  // Check incoming track pointer is valid, otherwise might match a
  // NULL entry in vector.
  if(track == NULL) {
    Squeak::mout(Squeak::warning)
        << "PrimaryChain::HasTrack was passed a NULL Track pointer..."
        << std::endl;
    return false;
  }

  // Find track in vector
  std::vector<MAUS::GlobalTrack*>::iterator target;
  target = std::find(_tracks->begin(), _tracks->end(), track);

  if(target == _tracks->end()) return false;
  else return true;
}

bool PrimaryChain::HasTrackFromInt(int trackid) {
  if(trackid == -1) {
    Squeak::mout(Squeak::warning)
        << "Search for Parent ID == -1.  This signifies\n"
        << "initial track was a primary particle, with no parent."
        << std::endl;
    return false;
  }

  size_t index = trackid;
  if(index < 0 || index >= _tracks->size()) return false;

  // Check the track hasn't been removed via the RemoveTrack method.
  if(_tracks->at(trackid) == NULL) return false;
  
  return true;
}

int PrimaryChain::GetTrackId(MAUS::GlobalTrack* track) {
  // Check incoming track pointer is valid, otherwise might match a
  // NULL entry in vector.
  if(track == NULL) {
    Squeak::mout(Squeak::warning)
        << "PrimaryChain::GetTrackId was passed a NULL Track pointer..."
        << std::endl;
    return -2;
  }

  // Find track in vector
  std::vector<MAUS::GlobalTrack*>::iterator target;
  target = std::find(_tracks->begin(), _tracks->end(), track);

  if(target == _tracks->end()) return -2;
  else return (target - _tracks->begin());
}

int PrimaryChain::GetTrackParentId(MAUS::GlobalTrack* track) {
  // Find track in vector
  std::vector<MAUS::GlobalTrack*>::iterator target;
  target = std::find(_tracks->begin(), _tracks->end(), track);

  // Track not member of PrimaryChain, return -2
  if(target == _tracks->end()) return -2;

  size_t index = target - _tracks->begin();
  return _parents[index];
}

int PrimaryChain::GetTrackParentIdFromInt(int index) {
  if(index < 0) return -2;
  size_t index_s = index;
  if(index_s >= _parents.size()) return -2;
  else return _parents[index_s];
}

std::vector<int> PrimaryChain::GetTrackDaughterIds(MAUS::GlobalTrack* track) {
  // Find track in vector
  std::vector<MAUS::GlobalTrack*>::iterator target;
  target = std::find(_tracks->begin(), _tracks->end(), track);

  // Track not member of PrimaryChain, return empty vector
  if(target == _tracks->end()) return std::vector<int>();

  // Store indices for tracks with parent == index
  int index = target - _tracks->begin();
  return GetTrackDaughterIdsFromInt(index);
}

std::vector<int> PrimaryChain::GetTrackDaughterIdsFromInt(int index) {
  std::vector<int> result;
  unsigned int i;
  for(i = 0; i < _parents.size(); ++i) {
    if(_parents[i] == index) result.push_back(i);
  }
  return result;
}

const MAUS::GlobalTrack* PrimaryChain::GetTrackFromInt(int index) {
  if(index < 0) return NULL;
  size_t index_s = index;
  if(index_s >= _tracks->size()) return NULL;
  else return _tracks->at(index);
  
}

bool PrimaryChain::CleanUpTracks() {
  // Identify NULL Track pointers and matching indices
  std::vector<unsigned int> dead_indices;
  unsigned int i;
  for(i = 0; i < _tracks->size(); ++i) {
    if(_tracks->at(i) == NULL) dead_indices.push_back(i);
  }

  // Check no existing tracks point to a NULL parent
  std::vector<int>::iterator result;
  for(i = 0; i < dead_indices.size(); ++i) {
    result = std::find(_parents.begin(), _parents.end(), dead_indices[i]);
    if(result != _parents.end()) {
      Squeak::mout(Squeak::error)
          << "Parent ID matches a NULL track.  CleanUpTracks failed."
          << std::endl;
      return false;
    }
  }

  // Determine change in indices
  std::vector<int> index_map;
  int index_count = 0;
  for(i = 0; i < _parents.size(); ++i) {
    bool remove = (std::find(dead_indices.begin(), dead_indices.end(), i) !=
                   dead_indices.end());
    if(remove) {
      index_map.push_back(-2);
    } else {
      index_map.push_back(index_count++);
    }
  }
  
  // Remove NULL Tracks and change parents vector
  std::vector<MAUS::GlobalTrack*> * replacement_tracks =
      new std::vector<MAUS::GlobalTrack*>();
  std::vector<int> replacement_parents;
  for(i = 0; i < _tracks->size(); ++i) {
    // bool remove = (std::find(dead_indices.begin(), dead_indices.end(), i) !=
    //                dead_indices.end());
    bool remove = (index_map[i] == -2);
    if(!remove) {
      replacement_tracks->push_back(_tracks->at(i));
      // Maintain primary status
      if(_parents[i] == -1) {
        replacement_parents.push_back(-1);
      } else {
        replacement_parents.push_back(index_map[_parents[i]]);
      }
    }
  }
  set_tracks(replacement_tracks);
  set_parents(replacement_parents);
  
  return true;
}

void PrimaryChain::ClearTracks() {
  _tracks->clear();
  _parents.clear();
}

} // ~namespace global
} // ~namespace recon
} // ~namespace MAUS
