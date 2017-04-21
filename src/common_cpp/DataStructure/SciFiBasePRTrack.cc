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

// MAUS headers
#include "src/common_cpp/DataStructure/SciFiBasePRTrack.hh"

namespace MAUS {

SciFiBasePRTrack::SciFiBasePRTrack()
  : _spoints(NULL),
    _covariance(0),
    _position(0.0, 0.0, 0.0),
    _momentum(0.0, 0.0, 0.0),
    _seed_position(0.0, 0.0, 0.0),
    _seed_momentum(0.0, 0.0, 0.0),
    _chi_sq(-1.0),
    _ndf(-1),
    _charge(0),
    _n_fit_points(-1) {
  _spoints = new TRefArray();
}

SciFiBasePRTrack::SciFiBasePRTrack(DoubleArray cov)
  : _spoints(NULL),
    _covariance(cov),
    _position(0.0, 0.0, 0.0),
    _momentum(0.0, 0.0, 0.0),
    _seed_position(0.0, 0.0, 0.0),
    _seed_momentum(0.0, 0.0, 0.0),
    _chi_sq(-1.0),
    _ndf(-1),
    _charge(0),
    _n_fit_points(-1) {
  _spoints = new TRefArray();
}

SciFiBasePRTrack::SciFiBasePRTrack(int charge, DoubleArray cov, SciFiSpacePointPArray spoints)
  : _spoints(NULL),
    _covariance(cov),
    _position(0.0, 0.0, 0.0),
    _momentum(0.0, 0.0, 0.0),
    _seed_position(0.0, 0.0, 0.0),
    _seed_momentum(0.0, 0.0, 0.0),
    _chi_sq(-1.0),
    _ndf(-1),
    _charge(charge),
    _n_fit_points(-1) {
  _spoints = new TRefArray();
  for (std::vector<SciFiSpacePoint*>::iterator sp = spoints.begin(); sp != spoints.end(); ++sp) {
    _spoints->Add(*sp);
  }
}

SciFiBasePRTrack::SciFiBasePRTrack(const SciFiBasePRTrack& track)
  : _spoints(NULL),
    _covariance(track._covariance),
    _position(track._position),
    _momentum(track._momentum),
    _seed_position(track._seed_position),
    _seed_momentum(track._seed_momentum),
    _chi_sq(track._chi_sq),
    _ndf(track._ndf),
    _charge(track._charge),
    _n_fit_points(track._n_fit_points) {
  _spoints = new TRefArray(*track._spoints);
}

SciFiBasePRTrack::~SciFiBasePRTrack() {
  delete _spoints;
}

SciFiBasePRTrack& SciFiBasePRTrack::operator=(const SciFiBasePRTrack& track) {
  if (&track == this) return *this;

  if (_spoints) delete _spoints;
  _spoints = new TRefArray(*track._spoints);
  _covariance = track._covariance;
  _position = track._position;
  _momentum = track._momentum;
  _seed_position = track._seed_position;
  _seed_momentum = track._seed_momentum;
  _chi_sq = track._chi_sq;
  _ndf = track._ndf;
  _charge = track._charge;
  _n_fit_points = track._n_fit_points;

  return *this;
}


SciFiSpacePointPArray SciFiBasePRTrack::get_spacepoints_pointers() const {
  SciFiSpacePointPArray sp_pointers;

  // Check the _spoints container is not initialised
  if (!_spoints) {
    std::cerr << "Spacepoint TRefArray not initialised" << std::endl;
    return sp_pointers;
  }

  for (int i = 0; i < (_spoints->GetLast()+1); ++i) {
    sp_pointers.push_back(static_cast<SciFiSpacePoint*>(_spoints->At(i)));
  }
  return sp_pointers;
}

void SciFiBasePRTrack::add_spacepoint_pointer(SciFiSpacePoint* sp) {
  _spoints->Add(sp);
}

void SciFiBasePRTrack::set_spacepoints(TRefArray* spoints) {
  if (_spoints) delete _spoints;
  _spoints = spoints;
}

void SciFiBasePRTrack::set_spacepoints_pointers(const SciFiSpacePointPArray &spoints) {
  if (_spoints) delete _spoints;
  _spoints = new TRefArray();
  for (
    std::vector<SciFiSpacePoint*>::const_iterator sp = spoints.begin(); sp != spoints.end(); ++sp) {
    _spoints->Add(*sp);
  }
}

int SciFiBasePRTrack::get_num_triplets() const {
  int triplet_count = 0;
  for (int i = 0; i < (_spoints->GetLast()+1); ++i) {
    SciFiSpacePoint* sp = static_cast<SciFiSpacePoint*>(_spoints->At(i));
    if ((sp->get_channels()->GetLast() + 1) == 3) {
        triplet_count += 1;
    }
  }
  return triplet_count;
}

} // ~namespace MAUS
