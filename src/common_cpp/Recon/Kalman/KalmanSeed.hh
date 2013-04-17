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

#include <algorithm>
#include <vector>

#include "TMatrixD.h"
#include "TMath.h"

#include "src/common_cpp/DataStructure/SciFiHelicalPRTrack.hh"
#include "src/common_cpp/DataStructure/SciFiStraightPRTrack.hh"
#include "src/common_cpp/DataStructure/SciFiCluster.hh"

namespace MAUS {

class KalmanSeed {
 public:
  KalmanSeed();

  ~KalmanSeed();

  void Build(const SciFiStraightPRTrack* pr_track);

  void Build(const SciFiHelicalPRTrack* pr_track);

  void ProcessMeasurements(const SciFiStraightPRTrack* pr_track);

  void ProcessMeasurements(const SciFiHelicalPRTrack* pr_track);

  TMatrixD ComputeInitialStateVector(const SciFiHelicalPRTrack* seed);

  TMatrixD ComputeInitialStateVector(const SciFiStraightPRTrack* seed);

  void RetrieveClusters(std::vector<SciFiSpacePoint*> &spacepoints, double &seed_pz);

  bool is_helical()  const { return _helical; }

  bool is_straight() const { return _straight; }

  std::vector<SciFiCluster*> clusters() const { return _clusters; }

  TMatrixD initial_state_vector() const { return _a0; }

  size_t is_usable() const { return _clusters.size(); }

  double momentum() const { return _momentum; }

  std::vector<SciFiSpacePoint*> spacepoints() { return _spacepoints; }

  int n_parameters() const { return _n_parameters; }

 private:
  std::vector<SciFiCluster*> _clusters;

  std::vector<SciFiSpacePoint*> _spacepoints;

  TMatrixD _a0;

  double _momentum;

  bool _straight;

  bool _helical;

  int _n_parameters;
};

} // ~namespace MAUS

#endif
