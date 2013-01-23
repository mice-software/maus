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

/** @class MAUS::recon::global::PrimaryChain
 *  @ingroup globalrecon
 *  @brief The reconstructed chain for a single primary particle.
 *
 *  A hypothesised reconstructed chain for a single primary particle.
 *  This consists of a series of tracks, with one primary and the
 *  possibility of multiple daughters.
 */

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_TREFTRACKPAIR_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_TREFTRACKPAIR_HH_

// C++ headers
#include <utility>

// ROOT headers
#include <TRef.h>

#include "src/common_cpp/DataStructure/GlobalTrack.hh"

namespace MAUS {
namespace recon {
namespace global {

class TRefTrackPair : public std::pair<TRef, TRef> {
 public:
  // Standard Constructor
  TRefTrackPair() {
    this->first = TRef(NULL); 
    this->second = TRef(NULL); 
  };

  TRefTrackPair(TRef f, TRef s) {
    this->first = f; 
    this->second = s; 
  };

  TRefTrackPair(MAUS::recon::global::Track* track,
                MAUS::recon::global::Track* parent) {
    this->first = track; 
    this->second = parent; 
  };

  // Destructor
  ~TRefTrackPair() {};

  // Set the Track
  void SetTrack(MAUS::recon::global::Track* track) {
    this->first = track; 
  }    

  // Get a const pointer to the track
  MAUS::recon::global::Track* GetTrack() const {
    return (MAUS::recon::global::Track*) this->first.GetObject();
  }    

  // Set the Parent
  void SetParent(MAUS::recon::global::Track* parent) {
    this->second = parent; 
  }    

  // Get a const pointer to the parent
  MAUS::recon::global::Track* GetParent() const {
    return (MAUS::recon::global::Track*) this->second.GetObject();
  }    

  // Setter for first element, for TRefTrackPairProcessor
  void set_first(TRef first) {
    this->first = first;
  }

  // Getter for first element, for TRefTrackPairProcessor
  TRef get_first() const {
    return this->first;
  }

  // Setter for second element, for TRefTrackPairProcessor
  void set_second(TRef second) {
    this->second = second;
  }

  // Getter for second element, for TRefTrackPairProcessor
  TRef get_second() const {
    return this->second;
  }
};
} // ~namespace global
} // ~namespace recon
} // ~namespace MAUS

#endif
