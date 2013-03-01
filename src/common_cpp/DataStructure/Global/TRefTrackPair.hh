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

/** @class MAUS::DataStructure::Global::TRefTrackPair
 *  @ingroup globalrecon
 *  @brief A container class, for a pair of TRefs to Tracks.
 *  @author Ian Taylor, University of Warwick
 *  @date 2013/03/01
 *
 *  A container class, for a pair of TRef references to two
 *  MAUS::DataStructure::Global::TRefTrackPair objects. The main use
 *  case is for a track / parent pair, within the PrimaryChain object.
 *  The first reference must be valid, but the second can be NULL.
 */

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_TREFTRACKPAIR_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_TREFTRACKPAIR_HH_

// C++ headers
#include <utility>

// ROOT headers
#include <TRef.h>

#include "src/common_cpp/DataStructure/Global/Track.hh"

namespace MAUS {
namespace DataStructure {
namespace Global {

class TRefTrackPair : public std::pair<TRef, TRef> {
 public:
  /// Standard Constructor
  TRefTrackPair();

  /// Constructor from TRefs
  TRefTrackPair(TRef f, TRef s);

  /// Constructor from pointers
  TRefTrackPair(MAUS::DataStructure::Global::Track* track,
                MAUS::DataStructure::Global::Track* parent);

  /// Destructor
  ~TRefTrackPair();

  /// Set the Track
  void SetTrack(MAUS::DataStructure::Global::Track* track);

  /// Get a const pointer to the track
  MAUS::DataStructure::Global::Track* GetTrack() const;

  /// Set the Parent
  void SetParent(MAUS::DataStructure::Global::Track* parent);

  /// Get a const pointer to the parent
  MAUS::DataStructure::Global::Track* GetParent() const;

  /// Setter for first element, for TRefTrackPairProcessor
  void set_first(TRef first);

  /// Getter for first element, for TRefTrackPairProcessor
  TRef get_first() const;

  /// Setter for second element, for TRefTrackPairProcessor
  void set_second(TRef second);

  /// Getter for second element, for TRefTrackPairProcessor
  TRef get_second() const;
};
} // ~namespace Global
} // ~namespace DataStructure
} // ~namespace MAUS

#endif
