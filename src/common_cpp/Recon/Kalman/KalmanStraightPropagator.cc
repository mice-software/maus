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
#include "src/common_cpp/Recon/Kalman/KalmanStraightPropagator.hh"

namespace MAUS {

KalmanStraightPropagator::KalmanStraightPropagator() : KalmanPropagator() {
  _n_parameters = 4;
  // Propagator matrix.
  _F.ResizeTo(_n_parameters, _n_parameters);
  _F.Zero();
  // MCS error.
  _Q.ResizeTo(_n_parameters, _n_parameters);
  _Q.Zero();
  // Backpropagation matrix.
  _A.ResizeTo(_n_parameters, _n_parameters);
  _A.Zero();
}

KalmanStraightPropagator::~KalmanStraightPropagator() {}

void KalmanStraightPropagator::UpdatePropagator(const KalmanSite *old_site,
                                                const KalmanSite *new_site) {
  // Find dz between sites.
  double new_z = new_site->z();
  double old_z = old_site->z();
  double deltaZ = new_z-old_z;

  // Build _F.
  _F.UnitMatrix();
  _F(0, 1) = deltaZ;
  _F(2, 3) = deltaZ;
}

void KalmanStraightPropagator::CalculatePredictedState(const KalmanSite *old_site,
                                                       KalmanSite *new_site) {
  UpdatePropagator(old_site, new_site);

  TMatrixD a = old_site->a(KalmanSite::Filtered);

  TMatrixD a_projected = _F*a;

  new_site->set_a(a_projected, KalmanSite::Projected);
}

} // ~namespace MAUS
