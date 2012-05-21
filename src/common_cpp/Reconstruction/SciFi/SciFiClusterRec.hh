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
#include "src/common_cpp/Reconstruction/SciFi/SciFiEvent.hh"
#include "src/common_cpp/Reconstruction/SciFi/SciFiDigit.hh"

// namespace MAUS {

class SciFiClusterRec {
 public:
  SciFiClusterRec(); // Default constructor

  SciFiClusterRec(int cluster_exception, double min_npe);

  ~SciFiClusterRec();

  /** @brief Clustering main worker.
   * @arg evt a SciFiEvent to be filled with SciFiClusters
   * @arg modules the SciFi MICE modules
   */
  void process(SciFiEvent &evt, std::vector<const MiceModule*> modules);

  /** @brief Finds the position and direction of the clusters.
   * @arg clust a SciFiCluster
   * @arg modules the MICE modules
   */
  void construct(SciFiCluster *clust, std::vector<const MiceModule*> modules);

  Hep3Vector get_reference_frame_pos(int tracker, std::vector<const MiceModule*> modules);

 private:

  int _size_exception;

  double _min_npe;
};  // Don't forget this trailing colon!!!!
// } // ~namespace MAUS

#endif
