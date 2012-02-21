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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_TRACK_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_TRACK_HH_

#include <vector>
#include "src/common_cpp/DataStructure/ThreeVector.hh"
#include "src/common_cpp/DataStructure/Step.hh"

#include "Rtypes.h" // ROOT

namespace MAUS {

class Track {
  public:
    Track();
    Track(const Track& track);
    Track& operator=(const Track& track);
    ~Track();

    ThreeVector GetInitialPosition() const;
    void SetInitialPosition(ThreeVector pos);

    ThreeVector GetInitialMomentum() const;
    void SetInitialMomentum(ThreeVector mom);

    ThreeVector GetFinalPosition() const;
    void SetFinalPosition(ThreeVector pos);

    ThreeVector GetFinalMomentum() const;
    void SetFinalMomentum(ThreeVector pos);

    int GetParticleId() const;
    void SetParticleId(int id);

    int GetTrackId() const;
    void SetTrackId(int id);

    int GetParentTrackId() const;
    void SetParentTrackId(int id);

    std::vector<Step>* GetSteps() const;
    void SetSteps(std::vector<Step>* steps);

  private:
    std::vector<Step>* _steps;

    ThreeVector _initial_position;
    ThreeVector _final_position;
    ThreeVector _initial_momentum;
    ThreeVector _final_momentum;

    int _particle_id;
    int _track_id;
    int _parent_track_id;
};

}

#endif
