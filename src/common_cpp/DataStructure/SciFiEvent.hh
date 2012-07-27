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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_SCIFIEVENT_
#define _SRC_COMMON_CPP_DATASTRUCTURE_SCIFIEVENT_

// C++ headers
#include <vector>

// ROOT headers
#include "Rtypes.h"

// MAUS headers
#include "src/common_cpp/DataStructure/Hit.hh"
#include "src/common_cpp/DataStructure/SciFiDigit.hh"
#include "src/common_cpp/DataStructure/SciFiCluster.hh"

namespace MAUS {

typedef std::vector<SciFiHit*> SciFiHitPArray;
typedef std::vector<SciFiDigit*> SciFiDigitPArray;
typedef std::vector<SciFiCluster*> SciFiClusterPArray;

/** @class SciFiEvent A container to hold other SciFi containers, representing a particle event
 *
 */

class SciFiEvent {
  public:
    /** Default constructor - initialises to 0/NULL */
    SciFiEvent();

    /** Copy constructor - any pointers are deep copied */
    SciFiEvent(const SciFiEvent& _scifievent);

    /** Equality operator - any pointers are deep copied */
    SciFiEvent& operator=(const SciFiEvent& _scifievent);

    /** Destructor - any member pointers are deleted */
    virtual ~SciFiEvent();

    /** Hits */
    void add_hit(SciFiHit* hit) { _scifihits.push_back(hit); }
    SciFiHitPArray hits() const { return _scifihits; }

    /** Digits */
    void add_digit(SciFiDigit* digit) { _scifidigits.push_back(digit); }
    SciFiDigitPArray digits() const { return _scifidigits; }

    /** Clusters */
    void add_cluster(SciFiCluster* cluster) { _scificlusters.push_back(cluster); }
    SciFiClusterPArray clusters() const { return _scificlusters; }

  private:

    /** Hits in an event */
    SciFiHitPArray _scifihits;

    /** Digits in an event */
    SciFiDigitPArray _scifidigits;

    /** Clusters in an event */
    SciFiClusterPArray _scificlusters;

    ClassDef(SciFiEvent, 1)
};

} // ~namespace MAUS

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_SCIFIEVENT_

