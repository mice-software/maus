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

/** @class SciFiEvent
 *  D...
 *
 */

#ifndef SCIFIEVENT_HH
#define SCIFIEVENT_HH
// C headers

// C++ headers
#include <vector>
#include <string>
#include <iterator>

#include "src/common_cpp/Recon/SciFi/SciFiHit.hh"
#include "src/common_cpp/Recon/SciFi/SciFiDigit.hh"
#include "src/common_cpp/Recon/SciFi/SciFiCluster.hh"
#include "src/common_cpp/Recon/SciFi/SciFiSpacePoint.hh"
#include "src/common_cpp/Recon/SciFi/SciFiStraightPRTrack.hh"

// namespace MAUS {

class SciFiEvent {
 public:
  SciFiEvent();

  ~SciFiEvent();

  std::vector<SciFiHit*> hits() const { return _scifihits; }

  std::vector<SciFiDigit*> digits() const { return _scifidigits; }

  std::vector<SciFiCluster*> clusters() const { return _scificlusters; }

  std::vector<SciFiSpacePoint*> spacepoints() const { return _scifispacepoints; }

  std::vector<SciFiSpacePoint*> seeds() const { return _scifiseeds; }

  std::vector<SciFiStraightPRTrack> straightprtracks() const { return _scifistraightprtracks; }

 private:
  std::vector<SciFiHit*>            _scifihits;
  std::vector<SciFiDigit*>          _scifidigits;
  std::vector<SciFiCluster*>        _scificlusters;
  std::vector<SciFiSpacePoint*>     _scifispacepoints;
  std::vector<SciFiSpacePoint*>     _scifiseeds;
  std::vector<SciFiStraightPRTrack> _scifistraightprtracks;
};  // Don't forget this trailing colon!!!!
// } // ~namespace MAUS

#endif
