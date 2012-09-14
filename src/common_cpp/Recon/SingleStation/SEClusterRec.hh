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

/** @class SEClusterRec
 *  Digitize events by running Tracker electronics simulation.
 *
 */

#ifndef SECLUSTERREC_HH
#define SECLUSTERREC_HH
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
#include "src/common_cpp/Recon/SingleStation/SEEvent.hh"
#include "src/common_cpp/Recon/SingleStation/SEDigit.hh"

// namespace MAUS {

class SEClusterRec {
 public:
  SEClusterRec(); // Default constructor

  SEClusterRec(int cluster_exception, double min_npe);

  ~SEClusterRec();

  /** @brief Clustering main worker.
   * @arg evt a SEEvent to be filled with SEClusters
   * @arg modules the SE MICE modules
   */
  void process(SEEvent *evt, std::vector<const MiceModule*> modules);

  /** @brief Finds the position and direction of the clusters.
   * @arg clust a SECluster
   * @arg modules the MICE modules
   */
  void construct(SECluster *clust, std::vector<const MiceModule*> modules);

  bool are_neighbours(SEDigit *seed_i, SEDigit *seed_j);

 private:
  int _size_exception;

  double _min_npe;
};  // Don't forget this trailing colon!!!!
// } // ~namespace MAUS

#endif
