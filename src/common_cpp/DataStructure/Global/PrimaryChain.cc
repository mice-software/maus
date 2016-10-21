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

namespace MAUS {
namespace DataStructure {
namespace Global {

// Default constructor
PrimaryChain::PrimaryChain()
    : _mapper_name("") {
  _lr_spacepoints = new TRefArray();
  _lr_tracks = new TRefArray();
  _tracks = new TRefArray();
}

// Copy constructor
PrimaryChain::PrimaryChain(const PrimaryChain &primary_chain)
    : _mapper_name(primary_chain._mapper_name) {
  _lr_spacepoints = new TRefArray(*primary_chain._lr_spacepoints);
  _lr_tracks = new TRefArray(*primary_chain._lr_tracks);
  _tracks = new TRefArray(*primary_chain._tracks);
}

// Constructor setting mapper name
PrimaryChain::PrimaryChain(std::string mapper_name)
    : _mapper_name(mapper_name) {
  _lr_spacepoints = new TRefArray();
  _lr_tracks = new TRefArray();
  _tracks = new TRefArray();
}

// Destructor
PrimaryChain::~PrimaryChain() {
  delete _lr_spacepoints;
  delete _lr_tracks;
  delete _tracks;
}

// Assignment Operator
PrimaryChain& PrimaryChain::operator=(const PrimaryChain &primary_chain) {
  if (this == &primary_chain) {
    return *this;
  }
  _mapper_name = primary_chain._mapper_name;
  _lr_spacepoints = new TRefArray(*primary_chain._lr_spacepoints);
  _lr_tracks = new TRefArray(*primary_chain._lr_tracks);
  _tracks = new TRefArray(*primary_chain._tracks);
  return *this;
}

void PrimaryChain::AddLRSpacePoint(MAUS::DataStructure::Global::SpacePoint* spacepoint) {
  _lr_spacepoints->Add(spacepoint);
}

void PrimaryChain::AddLRTrack(MAUS::DataStructure::Global::Track* track) {
  _lr_tracks->Add(track);
}

void PrimaryChain::AddTrack(MAUS::DataStructure::Global::Track* track) {
  _tracks->Add(track);
}

std::vector<MAUS::DataStructure::Global::SpacePoint*> PrimaryChain::GetLRSpacePoints() const {
  std::vector<MAUS::DataStructure::Global::SpacePoint*> spacepoints;
  MAUS::DataStructure::Global::SpacePoint* sp = NULL;
  int n = _lr_spacepoints->GetEntries();
  for (int i = 0; i < n; i++) {
    sp = (MAUS::DataStructure::Global::SpacePoint*) _lr_spacepoints->At(i);
    if (!sp) continue;
    spacepoints.push_back(sp);
  }
  return spacepoints;
}

std::vector<MAUS::DataStructure::Global::Track*> PrimaryChain::GetLRTracks() const {
  std::vector<MAUS::DataStructure::Global::Track*> tracks;
  MAUS::DataStructure::Global::Track* track = NULL;
  int n = _lr_tracks->GetEntries();
  for (int i = 0; i < n; i++) {
    track = (MAUS::DataStructure::Global::Track*) _lr_tracks->At(i);
    if (!track) continue;
    tracks.push_back(track);
  }
  return tracks;
}

std::vector<MAUS::DataStructure::Global::Track*> PrimaryChain::GetTracks() const {
  std::vector<MAUS::DataStructure::Global::Track*> tracks;
  MAUS::DataStructure::Global::Track* track = NULL;
  int n = _tracks->GetEntries();
  for (int i = 0; i < n; i++) {
    track = (MAUS::DataStructure::Global::Track*) _tracks->At(i);
    if (!track) continue;
    tracks.push_back(track);
  }
  return tracks;
}

std::vector<MAUS::DataStructure::Global::Track*>
    PrimaryChain::GetTracks(std::string mapper_name) const {
  std::vector<MAUS::DataStructure::Global::Track*> tracks;
  MAUS::DataStructure::Global::Track* track = NULL;
  int n = _tracks->GetEntries();
  for (int i = 0; i < n; i++) {
    track = (MAUS::DataStructure::Global::Track*) _tracks->At(i);
    if (!track) continue;
    if (track->get_mapper_name() == mapper_name) {
      tracks.push_back(track);
    }
  }
  return tracks;
}

void PrimaryChain::set_mapper_name(std::string mapper_name) {
  _mapper_name = mapper_name;
}

std::string PrimaryChain::get_mapper_name() const {
  return _mapper_name;
}
} // ~namespace Global
} // ~namespace DataStructure
} // ~namespace MAUS
