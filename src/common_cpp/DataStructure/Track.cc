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

#include "src/common_cpp/DataStructure/Track.hh"

namespace MAUS {

Track::Track() : _steps(NULL), _initial_position(0, 0, 0),
                 _final_position(0, 0, 0),
                 _initial_momentum(0, 0, 0), _final_momentum(0, 0, 0),
                 _particle_id(0), _track_id(0), _parent_track_id(0) {
}

Track::Track(const Track& track)
               : _steps(NULL), _initial_position(0, 0, 0),
                 _final_position(0, 0, 0),
                 _initial_momentum(0, 0, 0), _final_momentum(0, 0, 0),
                 _particle_id(0), _track_id(0), _parent_track_id(0)  {
    *this = track;
}

Track& Track::operator=(const Track& track) {
    if (this == &track) {
        return *this;
    }
    if (_steps != NULL) {
        delete _steps;
    }
    if (track._steps == NULL) {
        _steps = NULL;
    } else {
        _steps = new StepArray(*(track._steps));
    }
    _initial_position = track._initial_position;
    _final_position = track._final_position;
    _initial_momentum = track._initial_momentum;
    _final_momentum = track._final_momentum;
    _particle_id = track._particle_id;
    _track_id = track._track_id;
    _parent_track_id = track._parent_track_id;
    return *this;
}

Track::~Track() {
    if (_steps != NULL) {
        delete _steps;
        _steps = NULL;
    }
}

ThreeVector Track::GetInitialPosition() const {
    return _initial_position;
}

void Track::SetInitialPosition(ThreeVector pos) {
    _initial_position = pos;
}

ThreeVector Track::GetInitialMomentum() const {
    return _initial_momentum;
}

void Track::SetInitialMomentum(ThreeVector mom) {
    _initial_momentum = mom;
}

ThreeVector Track::GetFinalPosition() const {
    return _final_position;
}

void Track::SetFinalPosition(ThreeVector pos) {
    _final_position = pos;
}

ThreeVector Track::GetFinalMomentum() const {
    return _final_momentum;
}

void Track::SetFinalMomentum(ThreeVector mom) {
    _final_momentum = mom;
}

int Track::GetParticleId() const {
    return _particle_id;
}

void Track::SetParticleId(int id) {
    _particle_id = id;
}

int Track::GetTrackId() const {
    return _track_id;
}

void Track::SetTrackId(int id) {
    _track_id = id;
}

int Track::GetParentTrackId() const {
    return _parent_track_id;
}

void Track::SetParentTrackId(int id) {
    _parent_track_id = id;
}

StepArray* Track::GetSteps() const {
    return _steps;
}

void Track::SetSteps(StepArray* steps) {
    _steps = steps;
}
}

