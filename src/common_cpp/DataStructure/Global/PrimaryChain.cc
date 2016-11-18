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

#include "Utils/Squeak.hh"

namespace MAUS {
namespace DataStructure {
namespace Global {

// Default constructor
PrimaryChain::PrimaryChain()
    : _mapper_name(""), _type(MAUS::DataStructure::Global::kNoChainType),
      _us_daughter(NULL), _ds_daughter(NULL) {
  _tracks = new TRefArray();
}

// Copy constructor
PrimaryChain::PrimaryChain(const PrimaryChain &primary_chain)
    : _mapper_name(primary_chain._mapper_name), _type(primary_chain._type),
      _us_daughter(primary_chain._us_daughter), _ds_daughter(primary_chain._ds_daughter) {
  _tracks = new TRefArray(*primary_chain._tracks);
}

// Constructor setting mapper name
PrimaryChain::PrimaryChain(std::string mapper_name)
    : _mapper_name(mapper_name), _type(MAUS::DataStructure::Global::kNoChainType),
      _us_daughter(NULL), _ds_daughter(NULL) {
  _tracks = new TRefArray();
}

// Destructor
PrimaryChain::~PrimaryChain() {
  delete _tracks;
}

// Assignment Operator
PrimaryChain& PrimaryChain::operator=(const PrimaryChain &primary_chain) {
  if (this == &primary_chain) {
    return *this;
  }
  _mapper_name = primary_chain._mapper_name;
  _type = primary_chain._type;
  _tracks = new TRefArray(*primary_chain._tracks);
  _us_daughter = primary_chain._us_daughter;
  _ds_daughter = primary_chain._ds_daughter;
  return *this;
}

MAUS::DataStructure::Global::PrimaryChain* PrimaryChain::GetUSDaughter() const {
  MAUS::DataStructure::Global::PrimaryChain* us_daughter =
    (MAUS::DataStructure::Global::PrimaryChain*) _us_daughter.GetObject();
  return us_daughter;
}

void PrimaryChain::SetUSDaughter(MAUS::DataStructure::Global::PrimaryChain* us_daughter) {
  if (_type == MAUS::DataStructure::Global::kThrough) {
    _us_daughter = us_daughter;
  } else {
    Squeak::mout(Squeak::error)
        << "Attempting to add an upstream daughter chain to a non-through primary chain";
  }
}

MAUS::DataStructure::Global::PrimaryChain* PrimaryChain::GetDSDaughter() const {
  MAUS::DataStructure::Global::PrimaryChain* ds_daughter_chain =
    (MAUS::DataStructure::Global::PrimaryChain*) _ds_daughter.GetObject();
  return ds_daughter_chain;
}

void PrimaryChain::SetDSDaughter(MAUS::DataStructure::Global::PrimaryChain* ds_daughter) {
  if (_type == MAUS::DataStructure::Global::kThrough) {
    _ds_daughter = ds_daughter;
  } else {
    Squeak::mout(Squeak::error)
        << "Attempting to add a downstream daughter chain to a non-through primary chain";
  }
}

MAUS::DataStructure::Global::Track*
    PrimaryChain::GetMatchedTrack(MAUS::DataStructure::Global::PID pid) const {
  MAUS::DataStructure::Global::Track* track = NULL;
  int n = _tracks->GetEntries();
  for (int i = 0; i < n; i++) {
    track = (MAUS::DataStructure::Global::Track*) _tracks->At(i);
    if (track and track->get_mapper_name() == "MapCppGlobalTrackMatching"
        and track->get_pid() == pid) {
      return track;
    }
  }
  return 0;
}

std::vector<MAUS::DataStructure::Global::Track*> PrimaryChain::GetMatchedTracks() const {
  std::vector<MAUS::DataStructure::Global::Track*> tracks;
  MAUS::DataStructure::Global::Track* track = NULL;
  int n = _tracks->GetEntries();
  for (int i = 0; i < n; i++) {
    track = (MAUS::DataStructure::Global::Track*) _tracks->At(i);
    if (track and track->get_mapper_name() == "MapCppGlobalTrackMatching") {
      tracks.push_back(track);
    }
  }
  return tracks;
}

void PrimaryChain::AddMatchedTrack(MAUS::DataStructure::Global::Track* track) {
  track->set_mapper_name("MapCppGlobalTrackMatching");
  
}

MAUS::DataStructure::Global::Track* PrimaryChain::GetPIDTrack() const {
  // TODO: When implementing this into PID
  return NULL;
}

void PrimaryChain::SetPIDTrack(MAUS::DataStructure::Global::Track* track) {
  // TODO: When implementing this into PID
}

MAUS::DataStructure::Global::Track* PrimaryChain::GetFittedTrack() const {
  // TODO: When implementing this into Fitting
  return NULL;
}

void PrimaryChain::SetFittedTrack(MAUS::DataStructure::Global::Track* track) {
  // TODO: When implementing this into Fitting
}

//~ void PrimaryChain::AddLRSpacePoint(MAUS::DataStructure::Global::SpacePoint* spacepoint) {
  //~ _lr_spacepoints->Add(spacepoint);
//~ }

//~ void PrimaryChain::AddLRTrack(MAUS::DataStructure::Global::Track* track) {
  //~ _lr_tracks->Add(track);
//~ }

//~ void PrimaryChain::AddTrack(MAUS::DataStructure::Global::Track* track) {
  //~ _tracks->Add(track);
//~ }

//~ std::vector<MAUS::DataStructure::Global::SpacePoint*> PrimaryChain::GetLRSpacePoints() const {
  //~ std::vector<MAUS::DataStructure::Global::SpacePoint*> spacepoints;
  //~ MAUS::DataStructure::Global::SpacePoint* sp = NULL;
  //~ int n = _lr_spacepoints->GetEntries();
  //~ for (int i = 0; i < n; i++) {
    //~ sp = (MAUS::DataStructure::Global::SpacePoint*) _lr_spacepoints->At(i);
    //~ if (!sp) continue;
    //~ spacepoints.push_back(sp);
  //~ }
  //~ return spacepoints;
//~ }

//~ std::vector<MAUS::DataStructure::Global::Track*> PrimaryChain::GetLRTracks() const {
  //~ std::vector<MAUS::DataStructure::Global::Track*> tracks;
  //~ MAUS::DataStructure::Global::Track* track = NULL;
  //~ int n = _lr_tracks->GetEntries();
  //~ for (int i = 0; i < n; i++) {
    //~ track = (MAUS::DataStructure::Global::Track*) _lr_tracks->At(i);
    //~ if (!track) continue;
    //~ tracks.push_back(track);
  //~ }
  //~ return tracks;
//~ }

//~ std::vector<MAUS::DataStructure::Global::Track*> PrimaryChain::GetTracks() const {
  //~ std::vector<MAUS::DataStructure::Global::Track*> tracks;
  //~ MAUS::DataStructure::Global::Track* track = NULL;
  //~ int n = _tracks->GetEntries();
  //~ for (int i = 0; i < n; i++) {
    //~ track = (MAUS::DataStructure::Global::Track*) _tracks->At(i);
    //~ if (!track) continue;
    //~ tracks.push_back(track);
  //~ }
  //~ return tracks;
//~ }

//~ std::vector<MAUS::DataStructure::Global::Track*>
    //~ PrimaryChain::GetTracks(std::string mapper_name) const {
  //~ std::vector<MAUS::DataStructure::Global::Track*> tracks;
  //~ MAUS::DataStructure::Global::Track* track = NULL;
  //~ int n = _tracks->GetEntries();
  //~ for (int i = 0; i < n; i++) {
    //~ track = (MAUS::DataStructure::Global::Track*) _tracks->At(i);
    //~ if (!track) continue;
    //~ if (track->get_mapper_name() == mapper_name) {
      //~ tracks.push_back(track);
    //~ }
  //~ }
  //~ return tracks;
//~ }


} // ~namespace Global
} // ~namespace DataStructure
} // ~namespace MAUS
