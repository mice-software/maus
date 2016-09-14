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

#include <vector>
#include <algorithm>
#include <utility>

#include "Utils/Exception.hh"

namespace MAUS {
namespace DataStructure {
namespace Global {

// Default constructor
PrimaryChain::PrimaryChain()
    : _mapper_name(""),
      _goodness_of_fit(0.) {
  _tracks = new std::vector<MAUS::DataStructure::Global::TRefTrackPair*>();
  _parent_primary_chains = new TRefArray();
}

// Copy contructor
PrimaryChain::PrimaryChain(const PrimaryChain &primary_chain)
    : _mapper_name(""),
      _goodness_of_fit(primary_chain._goodness_of_fit),
      _comments(primary_chain._comments) {
  _tracks = new std::vector<MAUS::DataStructure::Global::TRefTrackPair*>();
  std::vector<MAUS::DataStructure::Global::TRefTrackPair*>::iterator iter;
  for (iter = primary_chain._tracks->begin();
      iter < primary_chain._tracks->end(); iter++) {
    _tracks->push_back(new MAUS::DataStructure::Global::TRefTrackPair(**iter));
  }
  _parent_primary_chains =
      new TRefArray(*primary_chain.get_parent_primary_chains());
}

// Constructor setting #_mapper_name
PrimaryChain::PrimaryChain(std::string mapper_name)
    : _mapper_name(mapper_name),
      _goodness_of_fit(0.) {
  _tracks = new std::vector<MAUS::DataStructure::Global::TRefTrackPair*>();
  _parent_primary_chains = new TRefArray();
}

// Destructor
PrimaryChain::~PrimaryChain() {
  std::vector<MAUS::DataStructure::Global::TRefTrackPair*>::iterator iter;
  for (iter = _tracks->begin(); iter < _tracks->end(); iter++)
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
  for (iter = primary_chain._tracks->begin();
      iter < primary_chain._tracks->end(); iter++) {
    _tracks->push_back(new MAUS::DataStructure::Global::TRefTrackPair(**iter));
  }
  _goodness_of_fit    = primary_chain._goodness_of_fit;
  _parent_primary_chains =
      new TRefArray(*primary_chain._parent_primary_chains);
  _comments = primary_chain._comments;
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
  for (size_t i = 0; i < _tracks->size(); i++) {
    track = _tracks->at(i)->GetTrack();
    tempMap[track] = track->Clone();
  }

  // Second, we loop through again.  Clones are added to the new
  // PrimaryChain, either as Primaries or with the appropriate
  // Clone...
  for (size_t i = 0; i < _tracks->size(); i++) {
    track = _tracks->at(i)->GetTrack();
    if (!_tracks->at(i)->second.IsValid()) {
      primaryChainNew->AddPrimaryTrack(tempMap[track]);
    } else {
      parent = _tracks->at(i)->GetParent();
      primaryChainNew->AddTrack(tempMap[track],
                                tempMap[parent]);
    }
  }

  primaryChainNew->set_goodness_of_fit(_goodness_of_fit);

  primaryChainNew->set_parent_primary_chains(
      new TRefArray(*_parent_primary_chains));

  primaryChainNew->set_comments(_comments);

  return primaryChainNew;
}

bool PrimaryChain::AddTrack(MAUS::DataStructure::Global::Track* track,
                            MAUS::DataStructure::Global::Track* parent) {
  // Check track is valid
  if (!track) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Attempting to add a NULL Track",
                 "DataStructure::Global::PrimaryChain::AddTrack()"));
  }

  // Check parent is valid.  If not, then add the track as a primary
  if (!parent)
    return AddPrimaryTrack(track);

  // Check parent is already in the _track vector.
  bool found = false;
  for (size_t i = 0; i < _tracks->size(); i++) {
    if (_tracks->at(i)->GetTrack() == parent) {
      found = true;
      break;
    }
  }

  if (!found) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Proposed Parent not in Primary Chain",
                 "DataStructure::Global::PrimaryChain::AddTrack()"));
  }

  // Add the track with parent
  MAUS::DataStructure::Global::TRefTrackPair* newPair =
      new MAUS::DataStructure::Global::TRefTrackPair(track, parent);
  _tracks->push_back(newPair);
  return true;
}

bool PrimaryChain::AddPrimaryTrack(MAUS::DataStructure::Global::Track* track) {
  // Check track is valid
  if (!track) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Attempting to add a NULL Track",
                 "DataStructure::Global::PrimaryChain::AddPrimaryTrack()"));
  }

  // Add the track with a NULL parent
  MAUS::DataStructure::Global::TRefTrackPair* newPair =
      new MAUS::DataStructure::Global::TRefTrackPair(track, NULL);
  _tracks->push_back(newPair);
  return true;
}

bool PrimaryChain::HasTrack(MAUS::DataStructure::Global::Track* track) {
  // Find track in vector
  std::vector<MAUS::DataStructure::Global::TRefTrackPair*>::iterator iter;
  for (iter = _tracks->begin(); iter < _tracks->end(); iter++) {
    if ((*iter)->GetTrack() == track) {
      return true;
    }
  }
  return false;
}

bool PrimaryChain::HasTrackAsParent(
    MAUS::DataStructure::Global::Track* parent) {
  // Find track as parent vector
  std::vector<MAUS::DataStructure::Global::TRefTrackPair*>::iterator iter;
  for (iter = _tracks->begin(); iter < _tracks->end(); iter++) {
    if ((*iter)->GetParent() == parent) {
      return true;
    }
  }
  return false;
}

bool PrimaryChain::IsPrimaryTrack(MAUS::DataStructure::Global::Track* track) {
  // Find track in vector
  std::vector<MAUS::DataStructure::Global::TRefTrackPair*>::iterator iter;
  for (iter = _tracks->begin(); iter < _tracks->end(); iter++) {
    if ((*iter)->GetTrack() == track) {
      return !((*iter)->second.IsValid());
    }
  }
  return false;
}

std::vector<MAUS::DataStructure::Global::Track*> PrimaryChain::GetTracks() {
  std::vector<MAUS::DataStructure::Global::Track*> result;

  // Get a pointer for each of the tracks stored.
  std::vector<MAUS::DataStructure::Global::TRefTrackPair*>::iterator iter;
  for (iter = _tracks->begin(); iter < _tracks->end(); iter++) {
    result.push_back((*iter)->GetTrack());
  }

  return result;
}

std::vector<MAUS::DataStructure::Global::Track*>
PrimaryChain::GetPrimaryTracks() {
  std::vector<MAUS::DataStructure::Global::Track*> result;

  // Get a pointer for each of the tracks that is primary
  std::vector<MAUS::DataStructure::Global::TRefTrackPair*>::iterator iter;
  for (iter = _tracks->begin(); iter < _tracks->end(); iter++) {
    if ((*iter)->second.IsValid() == false)
      result.push_back((*iter)->GetTrack());
  }

  return result;
}

MAUS::DataStructure::Global::Track* PrimaryChain::GetTrackParent(
    MAUS::DataStructure::Global::Track* track) {
  // Find track, and return parent
  std::vector<MAUS::DataStructure::Global::TRefTrackPair*>::iterator iter;
  for (iter = _tracks->begin(); iter < _tracks->end(); iter++) {
    if ((*iter)->GetTrack() == track) {
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
  for (iter = _tracks->begin(); iter < _tracks->end(); iter++) {
    if ((*iter)->GetParent() == track) {
      result.push_back((*iter)->GetTrack());
    }
  }

  return result;
}

void PrimaryChain::ClearTracks() {
  std::vector<MAUS::DataStructure::Global::TRefTrackPair*>::iterator iter;
  for (iter = _tracks->begin(); iter < _tracks->end(); iter++)
    delete *iter;
  _tracks->clear();
}

void PrimaryChain::AddParentChain(
    MAUS::DataStructure::Global::PrimaryChain* chain) {
  // Check primary chain is valid
  if (!chain) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Attempting to add a NULL PrimaryChain",
                 "DataStructure::Global::PrimaryChain::AddParentChain()"));
  }

  _parent_primary_chains->Add(chain);
}

std::vector<MAUS::DataStructure::Global::PrimaryChain*>
PrimaryChain::GetParentChains() {
  std::vector<MAUS::DataStructure::Global::PrimaryChain*> result;
  // Not a fast function, ROOT recommends avoiding unnecessary repeats
  int n = _parent_primary_chains->GetEntries();
  for (int i = 0; i < n; ++i) {
    // Cast the TObject* into a PrimaryChain*, and add to result
    result.push_back((MAUS::DataStructure::Global::PrimaryChain*)
                     _parent_primary_chains->At(i));
  }
  return result;
}

void PrimaryChain::set_track_parent_pairs(
    std::vector<MAUS::DataStructure::Global::TRefTrackPair*>* tracks) {
  if (_tracks != NULL) {
    // These TRefTrackPair's are unique to this PrimaryChain, and
    // owned by it.  Delete them before we replace the vector.
    for (size_t i = 0; i < _tracks->size(); ++i)
      delete _tracks->at(i);
    delete _tracks;
  }
  _tracks = tracks;
}

std::vector<MAUS::DataStructure::Global::TRefTrackPair*>*
PrimaryChain::get_track_parent_pairs() const {
  return _tracks;
}

void PrimaryChain::set_goodness_of_fit(double goodness_of_fit) {
  _goodness_of_fit = goodness_of_fit;
}

double PrimaryChain::get_goodness_of_fit() const {
  return _goodness_of_fit;
}


void PrimaryChain::set_parent_primary_chains(TRefArray* parent_primary_chains) {
  if (_parent_primary_chains != NULL) {
    delete _parent_primary_chains;
  }
  _parent_primary_chains = parent_primary_chains;
}

TRefArray* PrimaryChain::get_parent_primary_chains() const {
  return _parent_primary_chains;
}

void PrimaryChain::AddComment(std::string key, std::string comment) {
  _comments[key] = comment;
}

void PrimaryChain::ClearComments() {
  _comments.clear();
}

void PrimaryChain::RemoveComment(std::string key) {
  _comments.erase(key);
}

void PrimaryChain::set_comments(
    std::map<std::string, std::string> comments) {
  _comments = comments;
}

std::map<std::string, std::string> PrimaryChain::get_comments() const {
  return _comments;
}
} // ~namespace Global
} // ~namespace DataStructure
} // ~namespace MAUS
