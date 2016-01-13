/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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
 */

#include "DataStructure/EMREventTrack.hh"

namespace MAUS {

EMREventTrack::EMREventTrack()
  : _emrplanehits(), _emrspacepoints(),
    _track(), _type(""), _id(-1), _time(-1.),
    _plane_density_ma(-1.), _plane_density_sa(-1.),
    _total_charge_ma(-1.), _total_charge_sa(-1.),
    _charge_ratio_ma(-1.), _charge_ratio_sa(-1.) {
}

EMREventTrack::EMREventTrack(const EMREventTrack& emret)
  : _emrplanehits(), _emrspacepoints(),
    _track(), _type(""), _id(-1), _time(-1.),
    _plane_density_ma(-1.), _plane_density_sa(-1.),
    _total_charge_ma(-1.), _total_charge_sa(-1.),
    _charge_ratio_ma(-1.), _charge_ratio_sa(-1.) {

  *this = emret;
}

EMREventTrack& EMREventTrack::operator=(const EMREventTrack& emret) {
  if (this == &emret)
        return *this;

  this->_emrplanehits = emret._emrplanehits;
  for (size_t i = 0; i < this->_emrplanehits.size(); i++)
    this->_emrplanehits[i] = new EMRPlaneHit(*(this->_emrplanehits[i]));

  this->_emrspacepoints = emret._emrspacepoints;
  for (size_t i = 0; i < this->_emrspacepoints.size(); i++)
    this->_emrspacepoints[i] = new EMRSpacePoint(*(this->_emrspacepoints[i]));

  this->SetEMRTrack(emret._track);
  this->SetType(emret._type);
  this->SetTrackId(emret._id);
  this->SetTime(emret._time);
  this->SetPlaneDensityMA(emret._plane_density_ma);
  this->SetPlaneDensitySA(emret._plane_density_sa);
  this->SetTotalChargeMA(emret._total_charge_ma);
  this->SetTotalChargeSA(emret._total_charge_sa);
  this->SetChargeRatioMA(emret._charge_ratio_ma);
  this->SetChargeRatioSA(emret._charge_ratio_sa);

  return *this;
}

EMREventTrack::~EMREventTrack() {

  for (size_t i = 0; i < _emrplanehits.size(); i++)
    delete _emrplanehits[i];

  _emrplanehits.resize(0);

  for (size_t i = 0; i < _emrspacepoints.size(); i++)
    delete _emrspacepoints[i];

  _emrspacepoints.resize(0);
}

void EMREventTrack::SetEMRPlaneHitArray(EMRPlaneHitArray emrplanehits) {

  for (size_t i = 0; i < _emrplanehits.size(); i++) {
    if (_emrplanehits[i] != NULL)
        delete _emrplanehits[i];
  }
  _emrplanehits = emrplanehits;
}

void EMREventTrack::SetEMRSpacePointArray(EMRSpacePointArray emrspacepoints) {

  for (size_t i = 0; i < _emrspacepoints.size(); i++) {
    if (_emrspacepoints[i] != NULL)
        delete _emrspacepoints[i];
  }
  _emrspacepoints = emrspacepoints;
}
} // namespace MAUS
