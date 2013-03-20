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
#include "src/common_cpp/Recon/Kalman/StraightTrack.hh"

namespace MAUS {

StraightTrack::StraightTrack(bool MCS, bool Eloss) : KalmanTrack(MCS, Eloss) {
  _n_parameters = 4;
}

StraightTrack::~StraightTrack() {}

void StraightTrack::UpdatePropagator(const KalmanSite *old_site, const KalmanSite *new_site) {
  // Reset.
  _F.Zero();

  // Find dz between sites.
  double new_z = new_site->z();
  double old_z = old_site->z();
  double deltaZ = new_z-old_z;

  // Build _F.
  for ( int i = 0; i < _n_parameters; i++ ) {
    _F(i, i) = 1.;
  }
  _F(0, 1) = deltaZ;
  _F(2, 3) = deltaZ;
}

void StraightTrack::CalculatePredictedState(const KalmanSite *old_site, KalmanSite *new_site) {
  UpdatePropagator(old_site, new_site);

  TMatrixD a = old_site->a(KalmanSite::Filtered);

  TMatrixD a_projected = TMatrixD(_F, TMatrixD::kMult, a);

  new_site->set_a(a_projected, KalmanSite::Projected);
}

} // ~namespace MAUS
