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

/** @class MAUS::DataStructure::Global::PrimaryChain
 *  @ingroup globalrecon
 *  @brief The reconstructed chain for a single primary particle.
 *
 *  A hypothesised reconstructed chain for a single primary particle.
 *  This consists of a series of tracks, with one primary and the
 *  possibility of multiple daughters.
 */

#include "src/common_cpp/DataStructure/Global/TRefTrackPair.hh"

namespace MAUS {
namespace DataStructure {
namespace Global {

TRefTrackPair::TRefTrackPair() {
  this->first = TRef(NULL); 
  this->second = TRef(NULL); 
};

TRefTrackPair::TRefTrackPair(TRef f, TRef s) {
  this->first = f; 
  this->second = s; 
};

TRefTrackPair::TRefTrackPair(MAUS::DataStructure::Global::Track* track,
                             MAUS::DataStructure::Global::Track* parent) {
  this->first = track; 
  this->second = parent; 
};

// Destructor
TRefTrackPair::~TRefTrackPair() {};

// Set the Track
void TRefTrackPair::SetTrack(MAUS::DataStructure::Global::Track* track) {
  this->first = track; 
}    

// Get a const pointer to the track
MAUS::DataStructure::Global::Track* TRefTrackPair::GetTrack() const {
  return (MAUS::DataStructure::Global::Track*) this->first.GetObject();
}    

// Set the Parent
void TRefTrackPair::SetParent(MAUS::DataStructure::Global::Track* parent) {
  this->second = parent; 
}    

// Get a const pointer to the parent
MAUS::DataStructure::Global::Track* TRefTrackPair::GetParent() const {
  return (MAUS::DataStructure::Global::Track*) this->second.GetObject();
}    

// Setter for first element, for TRefTrackPairProcessor
void TRefTrackPair::set_first(TRef first) {
  this->first = first;
}

// Getter for first element, for TRefTrackPairProcessor
TRef TRefTrackPair::get_first() const {
  return this->first;
}

// Setter for second element, for TRefTrackPairProcessor
void TRefTrackPair::set_second(TRef second) {
  this->second = second;
}

// Getter for second element, for TRefTrackPairProcessor
TRef TRefTrackPair::get_second() const {
  return this->second;
}
} // ~namespace Global
} // ~namespace DataStructure
} // ~namespace MAUS
