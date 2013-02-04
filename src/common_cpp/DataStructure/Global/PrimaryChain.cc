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

#include "DataStructure/Global/PrimaryChain.hh"

#include <algorithm>
#include <utility>

#include "Interface/Squeak.hh"
// #include "DataStructure/Global/TrackPoint.hh"

namespace MAUS {
namespace DataStructure {
namespace Global {

// Default constructor
PrimaryChain::PrimaryChain()
    : _mapper_name(""),
      _tracks(new std::vector<MAUS::DataStructure::Global::TRefTrackPair*>()),
      _goodness_of_fit(0.) {
  _parent_primary_chains = new TRefArray();
}

// Copy contructor
PrimaryChain::PrimaryChain(const PrimaryChain &primary_chain)
    : _mapper_name(""),
      _goodness_of_fit(primary_chain._goodness_of_fit) {
  _tracks = new std::vector<MAUS::DataStructure::Global::TRefTrackPair*>();
  std::vector<MAUS::DataStructure::Global::TRefTrackPair*>::iterator iter;
  for(iter = primary_chain._tracks->begin();
      iter < primary_chain._tracks->end(); iter++){
    _tracks->push_back(new MAUS::DataStructure::Global::TRefTrackPair(**iter));
  }
  _parent_primary_chains =
      new TRefArray(*primary_chain.get_parent_primary_chains());
}

// Constructor setting #_mapper_name
PrimaryChain::PrimaryChain(std::string mapper_name)
    : _mapper_name(mapper_name),
      _tracks(new std::vector<MAUS::DataStructure::Global::TRefTrackPair*>()),
      _goodness_of_fit(0.) {
  _parent_primary_chains = new TRefArray();
}

// Destructor
PrimaryChain::~PrimaryChain() {
  std::vector<MAUS::DataStructure::Global::TRefTrackPair*>::iterator iter;
  for(iter = _tracks->begin(); iter < _tracks->end(); iter++)
    delete *iter;
  delete _parent_primary_chains;
}

// Assignment operator
PrimaryChain& PrimaryChain::operator=(const PrimaryChain &primary_chain) {
  if (this == &primary_chain) {
    return *this;
  }
  _mapper_name        = primary_chain._mapper_name;
  std::vector<MAUS::DataStructure::Global::TRefTrackPair*>::iterator iter;
  for(iter = primary_chain._tracks->begin();
      iter < primary_chain._tracks->end(); iter++){
    _tracks->push_back(new MAUS::DataStructure::Global::TRefTrackPair(**iter));
  }
  _goodness_of_fit    = primary_chain._goodness_of_fit;
  _parent_primary_chains =
      new TRefArray(*primary_chain._parent_primary_chains);
  return *this;
}

// Create a new PrimaryChain, identical to the original, but separate.  All
// constituent tracks are also cloned.
PrimaryChain* PrimaryChain::Clone() const {
  MAUS::DataStructure::Global::PrimaryChain* primaryChainNew =
      new MAUS::DataStructure::Global::PrimaryChain(_mapper_name);

  MAUS::DataStructure::Global::Track *track;
  MAUS::DataStructure::Global::Track *parent;
  // MAUS::DataStructure::Global::Track const *track;
  // MAUS::DataStructure::Global::Track const *parent;

  // First, we loop through all of the tracks, cloning each, and
  // placing the clones in a temporary map...
  std::map<const MAUS::DataStructure::Global::Track*,
           MAUS::DataStructure::Global::Track*> tempMap;
  for(size_t i = 0; i < _tracks->size(); i++) {
    track = _tracks->at(i)->GetTrack();
    tempMap[track] = track->Clone();
  }

  // Second, we loop through again.  Clones are added to the new
  // PrimaryChain, either as Primaries or with the appropriate
  // Clone...
  for(size_t i = 0; i < _tracks->size(); i++) {
    track = _tracks->at(i)->GetTrack();
    if(!_tracks->at(i)->second.IsValid())
      primaryChainNew->AddPrimaryTrack(tempMap[track]);
    else {
      parent = _tracks->at(i)->GetParent();
      primaryChainNew->AddTrack(tempMap[track],
                                tempMap[parent]);
    }
  }

  primaryChainNew->set_goodness_of_fit(_goodness_of_fit);

  primaryChainNew->set_parent_primary_chains(
      new TRefArray(*_parent_primary_chains));
  return primaryChainNew;
}

bool PrimaryChain::AddTrack(MAUS::DataStructure::Global::Track* track,
                            MAUS::DataStructure::Global::Track* parent) {
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
  for(size_t i = 0; i < _tracks->size(); i++) {
    if(_tracks->at(i)->GetTrack() == parent) {
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
  MAUS::DataStructure::Global::TRefTrackPair* newPair =
      new MAUS::DataStructure::Global::TRefTrackPair(track, parent);
  _tracks->push_back(newPair);
  return true;
}

bool PrimaryChain::AddPrimaryTrack(MAUS::DataStructure::Global::Track* track) {
  // Check track is valid
  if(!track) {
    Squeak::mout(Squeak::error) << "Attempting to add NULL Track"
                                << std::endl;
    return false;
  }
  
  // Add the track with a NULL parent
  MAUS::DataStructure::Global::TRefTrackPair* newPair =
      new MAUS::DataStructure::Global::TRefTrackPair(track, NULL);
  _tracks->push_back(newPair);
  return true;
}

bool PrimaryChain::RemoveTrack(MAUS::DataStructure::Global::Track* track) {
  // Check track is in the container
  bool in_tracks = false;
  for(size_t i = 0; i < _tracks->size(); i++) {
    if(_tracks->at(i)->GetTrack() == track) {
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
  for(size_t i = 0; i < _tracks->size(); i++) {
    if(_tracks->at(i)->GetParent() == track) {
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
  std::vector<MAUS::DataStructure::Global::TRefTrackPair*>::iterator iter;
  for(iter = _tracks->begin(); iter < _tracks->end(); iter++) {
    if((*iter)->GetTrack() == track) {
      _tracks->erase(iter);
      break;
    }
  }
  
  return true;
}

bool PrimaryChain::HasTrack(MAUS::DataStructure::Global::Track* track) {
  // Find track in vector
  std::vector<MAUS::DataStructure::Global::TRefTrackPair*>::iterator iter;
  for(iter = _tracks->begin(); iter < _tracks->end(); iter++) {
    if((*iter)->GetTrack() == track) {
      return true;
    }
  }
  return false;
}

bool PrimaryChain::HasTrackAsParent(
    MAUS::DataStructure::Global::Track* parent) {
  // Find track as parent vector
  std::vector<MAUS::DataStructure::Global::TRefTrackPair*>::iterator iter;
  for(iter = _tracks->begin(); iter < _tracks->end(); iter++) {
    if((*iter)->GetParent() == parent) {
      return true;
    }
  }
  return false;
}

bool PrimaryChain::IsPrimaryTrack(MAUS::DataStructure::Global::Track* track) {
  // Find track in vector
  std::vector<MAUS::DataStructure::Global::TRefTrackPair*>::iterator iter;
  for(iter = _tracks->begin(); iter < _tracks->end(); iter++) {
    if((*iter)->GetTrack() == track) {
      return !((*iter)->second.IsValid());
    }
  }
  return false;
}

MAUS::DataStructure::Global::Track* PrimaryChain::GetTrackParent(
    MAUS::DataStructure::Global::Track* track) {
  // Find track, and return parent
  std::vector<MAUS::DataStructure::Global::TRefTrackPair*>::iterator iter;
  for(iter = _tracks->begin(); iter < _tracks->end(); iter++) {
    if((*iter)->GetTrack() == track) {
      return (*iter)->GetParent();
    }
  }
  return NULL;
}

std::vector<MAUS::DataStructure::Global::Track*>
PrimaryChain::GetTrackDaughters(MAUS::DataStructure::Global::Track* track) {
  std::vector<MAUS::DataStructure::Global::Track*> result;

  // Find all tracks with this track as a parent
  std::vector<MAUS::DataStructure::Global::TRefTrackPair*>::iterator iter;
  for(iter = _tracks->begin(); iter < _tracks->end(); iter++) {
    if((*iter)->GetParent() == track) {
      result.push_back((*iter)->GetTrack());
    }
  }

  return result;
}
  
void PrimaryChain::ClearTracks() {
  std::vector<MAUS::DataStructure::Global::TRefTrackPair*>::iterator iter;
  for(iter = _tracks->begin(); iter < _tracks->end(); iter++)
    delete *iter;
  _tracks->clear();
}

} // ~namespace Global
} // ~namespace DataStructure
} // ~namespace MAUS
