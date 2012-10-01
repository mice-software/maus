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

/** @class SciFiClusterRec
 *  Digitize events by running Tracker electronics simulation.
 *
 */

#ifndef SCIFICLUSTERREC_HH
#define SCIFICLUSTERREC_HH

// C headers
#include <assert.h>
#include <json/json.h>
#include <CLHEP/Random/RandPoisson.h>
#include <CLHEP/Random/RandGauss.h>
#include <CLHEP/Random/RandExponential.h>

// C++ headers
#include <cmath>
#include <string>
#include <vector>

// other headers
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Vector/Rotation.h"

#include "src/legacy/Config/MiceModule.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/SciFiDigit.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"

namespace MAUS {

class SciFiClusterRec {
 public:
  SciFiClusterRec(); // Default constructor

  SciFiClusterRec(int cluster_exception, double min_npe, std::vector<const MiceModule*> modules);

  ~SciFiClusterRec();

  /** @brief Clustering main worker.
   * @arg evt a SciFiEvent to be filled with SciFiClusters
   * @arg modules the SciFi MICE modules
   */
  void process(SciFiEvent &evt);

  /** @brief Finds the position and direction of the clusters.
   * @arg clust a SciFiCluster
   */
  void process_cluster(SciFiCluster *clust);

  ThreeVector get_reference_frame_pos(int tracker);

  bool are_neighbours(SciFiDigit *seed_i, SciFiDigit *seed_j);

  std::vector<SciFiDigit*> get_seeds(SciFiEvent &evt);

  void make_clusters(SciFiEvent &evt, std::vector<SciFiDigit*> &seeds);

  const MiceModule* find_plane(int tracker, int station, int plane);

  // Set relative position & channel number for the Kalman Filter.
  // This is the position of the cluster relatively to station 1 of the tracker (0 or 1)
  // with the displacement of the station centre subtracted.
  void construct(SciFiCluster *clust, const MiceModule* this_plane,
                 ThreeVector &dir, ThreeVector &tracker_ref_frame_pos, double &alpha);

 private:

  int _size_exception;

  double _min_npe;

  std::vector<const MiceModule*> _modules;
};  // Don't forget this trailing colon!!!!

} // ~namespace MAUS

#endif
