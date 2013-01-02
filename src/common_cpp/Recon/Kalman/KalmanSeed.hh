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

#ifndef KALMANSEED_HH
#define KALMANSEED_HH

#include "TMatrixD.h"

#include "src/common_cpp/DataStructure/SciFiHelicalPRTrack.hh"
#include "src/common_cpp/DataStructure/SciFiStraightPRTrack.hh"
#include "src/common_cpp/DataStructure/SciFiCluster.hh"

namespace MAUS {

class KalmanSeed {
 public:
  KalmanSeed();

  ~KalmanSeed();

  template <class PRTrack> void build(PRTrack* helical_track);

  template <class PRTrack> void process_measurements(PRTrack* pr_track);

  TMatrixD compute_initial_state_vector(SciFiHelicalPRTrack* seed);

  TMatrixD compute_initial_state_vector(SciFiStraightPRTrack* seed);

  void retrieve_clusters(std::vector<SciFiSpacePoint> &spacepoints, double &seed_pz);

  bool is_helical()  const { return _helical; }

  bool is_straight() const { return _straight; }

  std::vector<SciFiCluster*> get_clusters() const { return _clusters; }

  TMatrixD get_initial_state_vector() const { return _a0; }

  size_t is_usable() const { return _clusters.size(); }

 private:
  std::vector<SciFiCluster*> _clusters;

  std::vector<SciFiSpacePoint> _spacepoints;

  TMatrixD _a0;

  bool _straight;

  bool _helical;
};

} // ~namespace MAUS

#endif
