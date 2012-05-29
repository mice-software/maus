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

/** @class SEEvent
 *  D...
 *
 */

#ifndef SEEVENT_HH
#define SEEVENT_HH

// C headers

// C++ headers
#include <vector>
#include <string>
#include <iterator>

// Third party headers
#include "TH1D.h"

// MAUS headers
#include "src/common_cpp/Recon/SingleStation/SEDigit.hh"
#include "src/common_cpp/Recon/SingleStation/SECluster.hh"
#include "src/common_cpp/Recon/SingleStation/SESpacePoint.hh"
#include "src/common_cpp/Recon/SingleStation/TOFSpacePoint.hh"

class SEEvent {
 public:
  /// Constructor and Destructor
  SEEvent();

  SEEvent(const SEEvent &event);

  ~SEEvent();

  SEEvent& operator=(const SEEvent &event);

  /// Digits
  void add_digit(SEDigit* digit) { _SEdigits.push_back(digit); }
  std::vector<SEDigit*> digits() const { return _SEdigits; }

  /// Clusters
  void add_cluster(SECluster* cluster) { _SEclusters.push_back(cluster); }
  // void set_clusters(std::vector<SECluster*> clusters) { _SEclusters=clusters; }
  std::vector<SECluster*> clusters() const { return _SEclusters; }

  /// Spacepoints
  void add_spacepoint(SESpacePoint* spacepoint) { _SEspacepoints.push_back(spacepoint); }
  std::vector<SESpacePoint*> spacepoints() const { return _SEspacepoints; }

  void add_TOF0spacepoint(TOFSpacePoint* spacepoint) { _TOF0spacepoints.push_back(spacepoint); }
  std::vector<TOFSpacePoint*> TOF0spacepoints() const { return _TOF0spacepoints; }

  void add_TOF1spacepoint(TOFSpacePoint* spacepoint) { _TOF1spacepoints.push_back(spacepoint); }
  std::vector<TOFSpacePoint*> TOF1spacepoints() const { return _TOF1spacepoints; }

  ///
  void set_phys_event_no(int phys_event_no) { _phys_event_no = phys_event_no; }
  int get_phys_event_no()    const { return _phys_event_no; }

  void set_part_event_no(int part_event_no) { _part_event_no = part_event_no; }
  int get_part_event_no()    const { return _part_event_no; }

 private:
  int _phys_event_no;

  int _part_event_no;

  /// Digits in an event.
  std::vector<SEDigit*>          _SEdigits;

  /// Clusters in an event.
  std::vector<SECluster*>        _SEclusters;

  /// Spacepoints in an event.
  std::vector<SESpacePoint*>     _SEspacepoints;

  std::vector<TOFSpacePoint*>     _TOF0spacepoints;

  std::vector<TOFSpacePoint*>     _TOF1spacepoints;
};  // Don't forget this trailing colon!!!!

#endif
