/* This file is part of MAUS: http://  micewww.pp.rl.ac.uk:8080/projects/maus
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
 * along with MAUS.  If not, see <http://  www.gnu.org/licenses/>.
 */

// Author: Peter Lane

#ifndef SRC_COMMON_CPP_OPTICS_LINEAR_APPROXIMATION_MODEL_HH
#define SRC_COMMON_CPP_OPTICS_LINEAR_APPROXIMATION_MODEL_HH

#include <vector>

#include "src/common_cpp/Optics/TransferMap.hh"

namespace MAUS {


/** @brief Calculates TransferMaps using any of several least squares fit
 *  functions.
 *  
 *  Here we find a polynomial of arbitrary dimension and order from a set of
 *  points by the method of least squares. The particular version of least
 *  squares used is specified in the constructor.
 *
 *  Note that the problem must be overspecified, so the number of points must be
 *  >= number of polynomial coefficients.
 *  A few interesting variations on this theme... have a look!\n
 * \n
 */

class LinearApproximationOpticsModel : public OpticsModel {
 public:
  // *************************
  //  Constructors
  // *************************

  LinearApproximationOpticsModel();

  ~LinearApproximationOpticsModel() { }
 private:
};

class LinearApproximationTransferMap : public TransferMap {
 public:
  LinearApproximationTransferMap(double z1, double z2, double mass)
    : z1_(z1), z2_(z2), mass_(mass) { }
  LinearApproximationTransferMap(const LinearApproximationTransferMap& map);
    : z1_(map.z1_), z2_(map.z2_), mass_(map.mass_) { }
  CovarianceMatrix Transport(CovarianceMatrix const * const covariances);
  PhaseSpaceVector Transport(PhaseSpaceVector const * const vector);
 private:
  double z1_;
  double z2_;
  double mass_;
};

} // namespace MAUS

#endif




