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

#ifndef _SRC_COMMON_CPP_RECON_EMR_TRACKMOMENTUM_HH
#define _SRC_COMMON_CPP_RECON_EMR_TRACKMOMENTUM_HH

// C++ headers
#include <vector>
#include <string>
#include <cmath>

// ROOT headers
#include "TF1.h"
#include "TCanvas.h"

// MAUS headers
#include "Utils/Exception.hh"
#include "Interface/Squeak.hh"

namespace TrackMomentum {

  /** @brief Reconstructs the particle momentum in the CSDA
    *
    *  Unfolds the momentum of a particle from its range in the EMR.
    *  Requires a particle ID hypothesis.
    *
    *  @param range	- Input range of the particle in the EMR
    *  @param particle	- Input particle ID hypothesis: pion, muon, electron
    */
  double csda_momentum(double range,
		       std::string particle);

  /** @brief Reconstructs the error on the particle momentum in the CSDA
    *
    *  Unfolds the error on the CSDA momentum.
    *  Requires a particle ID hypothesis.
    *
    *  @param momentum	- Input CSDA momentum of the particle
    *  @param erange	- Input error on the range of the particle in the EMR
    *  @param particle	- Input particle ID hypothesis: pion, muon, electron
    */
  double csda_momentum_error(double momentum,
			     double erange,
			     std::string particle);

  /** @brief Initializes all the variables used by the momentum unfolder
    *
    *  @param particle	- Input particle ID hypothesis: pion, muon, electron
    */
  bool initialize(std::string particle);

  /** @brief Maximum kinetic energy transferred to an electron in a single collision
    *
    *  @param beta	- Input relativistic beta: v/c
    *  @param gamma	- Input relativistic gamma: 1/sqrt(1-beta^2)
    *  @param M		- Input mass of the impinging particle
    */
  double tmax(double beta, double gamma, double M);

  /** @brief Relativistic energy loss (function of beta*gamma) of an electron
    *
    *  @param beta	- Input relativistic beta: v/c
    *  @param tau	- Input relativistic tau: gamma-1
    */
  double tauFel(double beta, double tau);

  /** @brief Relativistic energy loss (function of beta*gamma) of an positron
    *
    *  @param beta	- Input relativistic beta: v/c
    *  @param tau	- Input relativistic tau: gamma-1
    */
  double tauFpos(double beta, double tau);

  /** @brief Density effect on the ionization energy loss in PS
    *
    *  @param alpha	- Input relativistic alpha: beta*gamma
    */
  double deltaF(double alpha);

  /** @brief Bethe-Bloch formula for moderately relativistic heavy ions in PS
    *
    *  @param x[0]	- Input relativistic alpha: beta*gamma
    *  @param par[0]	- Input mass of the ion (M)
    */
  double bethe(double* x, double* par);

  /** @brief Inverse of the Bethe-Bloch formula
    *
    *  @param x[0]	- Input relativistic alpha: beta*gamma
    *  @param par[0]	- Input mass of the ion (M)
    */
  double invbethe(double* x, double* par);

  /** @brief Inverse of the Bethe-Bloch formula times the Jacobian of variable change
    *
    *  @param x[0]	- Input relativistic alpha: beta*gamma
    *  @param par[0]	- Input mass of the ion (M)
    */
  double invbethe_jac(double* x, double* par);

  /** @brief Range function as the integral of the inverse of Bethe-Bloch(CSDA)
    *
    *  @param x[0]	- Input momentum of the impinging particle
    *  @param par[0]	- Input mass of the ion (M)
    */
  double rangefunction(double* x, double* par);
} // namespace TrackMomentum

#endif // #define _SRC_COMMON_CPP_RECON_EMR_TRACKMOMENTUM_HH
