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

#include "DataStructure/EMREvent.hh"
#include "DataStructure/EMRPlaneHit.hh"

namespace MAUS {

EMREvent::EMREvent()
  : _emrplanehitarray(), _initial_trigger(false),
    _has_primary(false), _range_primary(0.0),
    _has_secondary(false), _range_secondary(0.0),
    _secondary_to_primary_track_distance(0),
    _total_charge_MA(0.0), _charge_ratio_MA(0.0),
    _total_charge_SA(0.0), _charge_ratio_SA(0.0),
    _plane_density(0.0), _chi2(0.0) {
}

EMREvent::EMREvent(const EMREvent& _emrevent)
  : _emrplanehitarray(), _initial_trigger(false),
    _has_primary(false), _range_primary(0.0),
    _has_secondary(false), _range_secondary(0.0),
    _secondary_to_primary_track_distance(0),
    _total_charge_MA(0.0), _charge_ratio_MA(0.0),
    _total_charge_SA(0.0), _charge_ratio_SA(0.0),
    _plane_density(0.0), _chi2(0.0) {
  *this = _emrevent;
}

EMREvent& EMREvent::operator=(const EMREvent& _emrevent) {
  if (this == &_emrevent) {
        return *this;
  }

  this->_emrplanehitarray = _emrevent._emrplanehitarray;
  for (size_t i = 0; i < this->_emrplanehitarray.size(); i++)
    this->_emrplanehitarray[i] = new EMRPlaneHit(*(this->_emrplanehitarray[i]));

  SetInitialTrigger(_emrevent._initial_trigger);
  SetHasPrimary(_emrevent._has_primary);
  SetRangePrimary(_emrevent._range_primary);
  SetHasSecondary(_emrevent._has_secondary);
  SetRangeSecondary(_emrevent._range_secondary);
  SetSecondaryToPrimaryTrackDistance(_emrevent._secondary_to_primary_track_distance);
  SetTotalChargeMA(_emrevent._total_charge_MA);
  SetChargeRatioMA(_emrevent._charge_ratio_MA);
  SetTotalChargeSA(_emrevent._total_charge_SA);
  SetChargeRatioSA(_emrevent._charge_ratio_SA);
  SetPlaneDensity(_emrevent._plane_density);
  SetChi2(_emrevent._chi2);
  return *this;
}

EMREvent::~EMREvent() {
  int nph = _emrplanehitarray.size();
  for (int i = 0; i < nph; i++)
    delete _emrplanehitarray[i];

  _emrplanehitarray.resize(0);
}

EMRPlaneHitArray EMREvent::GetEMRPlaneHitArray() const {
  return _emrplanehitarray;
}

void EMREvent::SetEMRPlaneHitArray(EMRPlaneHitArray emrplanehitarray) {
  int nplhits = _emrplanehitarray.size();
  for (int i = 0; i < nplhits; i++) {
    if (_emrplanehitarray[i] != NULL)
        delete _emrplanehitarray[i];
  }
  _emrplanehitarray = emrplanehitarray;
}

bool EMREvent::GetInitialTrigger() const {
  return _initial_trigger;
}

void EMREvent::SetInitialTrigger(bool initial_trigger) {
  _initial_trigger = initial_trigger;
}

bool EMREvent::GetHasPrimary() const {
  return _has_primary;
}

void EMREvent::SetHasPrimary(bool has_primary) {
  _has_primary = has_primary;
}

double EMREvent::GetRangePrimary() const {
  return _range_primary;
}

void EMREvent::SetRangePrimary(double range_primary) {
  _range_primary = range_primary;
}

bool EMREvent::GetHasSecondary() const {
  return _has_secondary;
}

void EMREvent::SetHasSecondary(bool has_secondary) {
  _has_secondary = has_secondary;
}

double EMREvent::GetRangeSecondary() const {
  return _range_secondary;
}

void EMREvent::SetRangeSecondary(double range_secondary) {
  _range_secondary = range_secondary;
}

double EMREvent::GetSecondaryToPrimaryTrackDistance() const {
  return _secondary_to_primary_track_distance;
}

void EMREvent::SetSecondaryToPrimaryTrackDistance(double secondary_to_primary_track_distance) {
  _secondary_to_primary_track_distance = secondary_to_primary_track_distance;
}

double EMREvent::GetTotalChargeMA() const {
  return _total_charge_MA;
}

void EMREvent::SetTotalChargeMA(double total_charge_MA) {
  _total_charge_MA = total_charge_MA;
}

double EMREvent::GetChargeRatioMA() const {
  return _charge_ratio_MA;
}

void EMREvent::SetChargeRatioMA(double charge_ratio_MA) {
  _charge_ratio_MA = charge_ratio_MA;
}

double EMREvent::GetTotalChargeSA() const {
  return _total_charge_SA;
}

void EMREvent::SetTotalChargeSA(double total_charge_SA) {
  _total_charge_SA = total_charge_SA;
}

double EMREvent::GetChargeRatioSA() const {
  return _charge_ratio_SA;
}

void EMREvent::SetChargeRatioSA(double charge_ratio_SA) {
  _charge_ratio_SA = charge_ratio_SA;
}

double EMREvent::GetPlaneDensity() const {
  return _plane_density;
}

void EMREvent::SetPlaneDensity(double plane_density) {
  _plane_density = plane_density;
}

double EMREvent::GetChi2() const {
  return _chi2;
}

void EMREvent::SetChi2(double chi2) {
  _chi2 = chi2;
}
}

