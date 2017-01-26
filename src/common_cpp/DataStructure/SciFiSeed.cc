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

#include "src/common_cpp/DataStructure/SciFiSeed.hh"

namespace MAUS {

// Default constructor
SciFiSeed::SciFiSeed()
  : _tracker(-1),
    _state_vector(),
    _covariance(),
    _algorithm(-1),
    _pr_track(new TRef()){
}

// Copy contructor
SciFiSeed::SciFiSeed(const SciFiSeed &sd)
  : _tracker(sd._tracker),
    _state_vector(sd._state_vector),
    _covariance(sd._covariance),
    _algorithm(sd._algorithm),
    _pr_track(new TRef(*sd._pr_track)) {
}


SciFiSeed::~SciFiSeed() {
  delete _pr_track;
}


SciFiSeed& SciFiSeed::operator=(const SciFiSeed &sd) {
  if (this == &sd) {
    return *this;
  }
  _tracker = sd._tracker;
  _state_vector = sd._state_vector;
  _covariance = sd._covariance;
  _algorithm = sd._algorithm;

  if (this->_pr_track) delete _pr_track;
  _pr_track = new TRef(sd._pr_track);

  return *this;
}

void SciFiSeed::setStateVector(TMatrixD vector) {
  _state_vector.clear();
  for (int i = 0; i < vector.GetNrows(); ++i) {
    _state_vector.push_back(vector(i, 0));
  }
}

TMatrixD SciFiSeed::getStateVector() const {
  TMatrixD newVec(_state_vector.size(), 1);
  for (int i = 0; i < _state_vector.size(); ++i) {
    newVec(i,0) = _state_vector[i];
  }
  return newVec;
}


void SciFiSeed::setCovariance(TMatrixD covariance) {
  _covariance.clear();
  for (int i = 0; i < covariance.GetNrows(); ++i) {
    for (int j = 0; j < covariance.GetNcols(); ++j) {
      _covariance.push_back(covariance(i, j));
    }
  }
}

TMatrixD SciFiSeed::getCovariance() const {
  int length = _state_vector.size();
  TMatrixD newCov(length, length);

  for (int i = 0; i < length; ++i) {
    for (int j = 0; j < length; ++j) {
      newCov(i,j) = _covariance[i*length + j];
    }
  }
  return newCov;
}

} // ~namespace MAUS
