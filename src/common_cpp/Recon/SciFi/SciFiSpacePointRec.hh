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

/** @class SciFiSpacePoint
 *
 *
 */

#ifndef SCIFISPACEPOINTREC_HH
#define SCIFISPACEPOINTREC_HH
// C headers
#include <assert.h>
#include <json/json.h>

#include <CLHEP/Random/RandPoisson.h>
#include <CLHEP/Random/RandGauss.h>
#include <CLHEP/Random/RandExponential.h>
#include <CLHEP/Units/PhysicalConstants.h>
#include <CLHEP/Matrix/Matrix.h>
#include <TMath.h>

// C++ headers
#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "Config/MiceModule.hh"

#include "src/common_cpp/DataStructure/SciFiCluster.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"

namespace MAUS {

class SciFiSpacePointRec {
 public:
  SciFiSpacePointRec();

  SciFiSpacePointRec(double aAcceptableRadius, double aKunoSumT1S5,
                     double aKunoSum, double aKunoTolerance);

  ~SciFiSpacePointRec();

  void process(SciFiEvent &evt) const;

  void make_cluster_container(SciFiEvent &evt, std::vector<SciFiCluster*>
                                                                       (&clusters)[2][6][3]) const;

  void look_for_triplets(SciFiEvent &evt, std::vector<SciFiCluster*> (&clusters)[2][6][3]) const;

  void look_for_duplets(SciFiEvent &evt, std::vector<SciFiCluster*> (&clusters)[2][6][3]) const;

  void build_duplet(SciFiSpacePoint* duplet) const;

  void build_triplet(SciFiSpacePoint* triplet) const;

  ThreeVector crossing_pos(SciFiCluster* c1, SciFiCluster* c2) const;

  bool kuno_accepts(SciFiCluster* cluster1,
                    SciFiCluster* cluster2,
                    SciFiCluster* cluster3) const;


  bool clusters_are_not_used(SciFiCluster* candidate_A,
                             SciFiCluster* candidate_B) const;

  bool clusters_are_not_used(SciFiCluster* candidate_A,
                             SciFiCluster* candidate_B,
                             SciFiCluster* candidate_C) const;

  bool duplet_within_radius(SciFiCluster* candidate_A,
                            SciFiCluster* candidate_B) const;

  /** Return the radius cut */
  double get_acceptable_radius() { return _acceptable_radius; }

  /** Return the Kuno conjecture sum for Tracker 1 Station 5 */
  double get_kuno_1_5() { return _kuno_1_5; }

  /** Return the Kuno conjecture sum for the other tracker stations */
  double get_kuno_else() { return _kuno_else; }

  /** Return the Kuno conjecture tolerance */
  double get_kuno_tolerance() { return _kuno_toler; }

 private:
  /// This is the acceptable radius for any duplet.
  double _acceptable_radius; // mm
  double _kuno_1_5;
  double _kuno_else;
  double _kuno_toler;
};  // Don't forget this trailing colon!!!!

} // ~namespace MAUS

#endif
