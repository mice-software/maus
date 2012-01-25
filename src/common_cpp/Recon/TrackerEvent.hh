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

/** @class TrackerEvent
 *  D...
 *
 */

#ifndef TRACKEREVENT_HH
#define TRACKEREVENT_HH
// C headers

// C++ headers
#include <vector>
#include <string>

#include "src/common_cpp/Recon/SciFiHit.hh"
#include "src/common_cpp/Recon/SciFiDigit.hh"
#include "src/common_cpp/Recon/SciFiCluster.hh"
#include "src/common_cpp/Recon/SciFiSpacePoint.hh"
#include "src/common_cpp/Recon/SciFiStraightPRTrack.hh"

class TrackerEvent {
 public:
  TrackerEvent();
  ~TrackerEvent();

  // std::string get_geometry_filename() const { return _filename; }

  // void set_geometry_filename(std::string file) { _filename = file; }

  std::vector<SciFiHit*>   scifihits;
  std::vector<SciFiDigit*>   scifidigits;
  std::vector<SciFiCluster*> scificlusters;
  std::vector<SciFiSpacePoint*> scifispacepoints;
  std::vector<SciFiStraightPRTrack*> scifistraightprtracks;

  // private:
  // std::string _filename;
};  // Don't forget this trailing colon!!!!

#endif
