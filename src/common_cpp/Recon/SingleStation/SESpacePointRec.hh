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

/** @class SESpacePoint
 *
 *
 */

#ifndef SESPACEPOINTREC_HH
#define SESPACEPOINTREC_HH
// C headers
#include <assert.h>
#include <json/json.h>

#include <CLHEP/Random/RandPoisson.h>
#include <CLHEP/Random/RandGauss.h>
#include <CLHEP/Random/RandExponential.h>
#include <CLHEP/Units/PhysicalConstants.h>

// C++ headers
#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "Config/MiceModule.hh"

#include "src/common_cpp/Recon/SingleStation/SECluster.hh"
#include "src/common_cpp/Recon/SingleStation/SESpacePoint.hh"
#include "src/common_cpp/Recon/SingleStation/SEEvent.hh"

// namespace MAUS {

class SESpacePointRec {
 public:
  SESpacePointRec(); // Default constructor

  ~SESpacePointRec();

  void process(SEEvent &evt);

  void build_duplet(SESpacePoint* duplet);

  void build_triplet(SESpacePoint* triplet);

  Hep3Vector crossing_pos(SECluster* c1, SECluster* c2);

  bool kuno_accepts(SECluster* cluster1,
                    SECluster* cluster2,
                    SECluster* cluster3);


  bool clusters_are_not_used(SECluster* candidate_A,
                             SECluster* candidate_B);

  bool clusters_are_not_used(SECluster* candidate_A,
                             SECluster* candidate_B,
                             SECluster* candidate_C);

  bool duplet_within_radius(SECluster* candidate_A,
                            SECluster* candidate_B);
 private:
  /// This is the acceptable radius for any duplet.
  static const int _acceptable_radius = 160; // mm
  static const int _kuno  = 318;
  static const int _kuno_toler = 2;
};  // Don't forget this trailing colon!!!!
// } // ~namespace MAUS

#endif
