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
#include <map>

// C++ headers
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>

// other headers
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/SciFiDigit.hh"
#include "src/common_cpp/Recon/SciFi/SciFiGeometryHelper.hh"

namespace MAUS {

class SciFiClusterRec {
 public:
  SciFiClusterRec();

  ~SciFiClusterRec();

  /** @brief
   * @arg
   */
  SciFiClusterRec(int cluster_exception,
                  double min_npe,
                  const SciFiTrackerMap& geometry_map);

  /** @brief Clustering main worker.
   * @arg evt a SciFiEvent to be filled with SciFiClusters
   * @arg modules the SciFi MICE modules
   */
  void process(SciFiEvent &evt) const;

  /** @brief Finds the position and direction of the clusters.
   * @arg clust a SciFiCluster
   */
  void process_cluster(SciFiCluster *clust) const;

  /** @brief Evaluates if two digits belong to neighbouring channels.
   * @arg Digits for comparison.
   */
  bool are_neighbours(SciFiDigit *seed_i, SciFiDigit *seed_j) const;

  /** @brief Loops over digits and accepts those above npe cut as seeds.
   * @arg The particle event.
   */
  std::vector<SciFiDigit*> get_seeds(SciFiEvent &evt) const;

  /** @brief
   * @arg
   */
  void make_clusters(SciFiEvent &evt, std::vector<SciFiDigit*> &seeds) const;

 private:
  int _size_exception;

  double _min_npe;

  SciFiTrackerMap _geometry_map;
};  // Don't forget this trailing colon!!!!

} // ~namespace MAUS

#endif
