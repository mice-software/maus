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

#include "Rtypes.h" // ROOT

#include "src/common_cpp/DataStructure/ThreeVector.hh"
#include "src/common_cpp/DataStructure/Step.hh"

namespace MAUS {

typedef std::vector<Step> StepArray;

/** @class Track stores data pertaining to a single particle in the geometry
 *
 *  Track stores: the start and end position/momentum of the trackl the track id
 *  (unique id assigned by GEANT4), track id of the parent track - i.e. if the
 *  track is a secondary, the id of the track that spawned it; and optionally a
 *  list of all the step points that make up the track (note that this can be
 *  switched off in data cards as it can lead to large files)
 */
class Track {
  public:
    /** Initialise to 0/NULL */
    Track();

    /** Copy constructor - deep copies steps */
    Track(const Track& track);

    /** Equality operator - deep copies steps */
    Track& operator=(const Track& track);

    /** Destructor - frees memory allocated to steps */
    virtual ~Track();

    /** Get the position of the track where it was spawned */
    ThreeVector GetInitialPosition() const;

    /** Set the position of the track where it was spawned */
    void SetInitialPosition(ThreeVector pos);

    /** Get the momentum of the track when it was spawned */
    ThreeVector GetInitialMomentum() const;

    /** Set the momentum of the track when it was spawned */
    void SetInitialMomentum(ThreeVector mom);

    /** Get the position of the track where it was stopped */
    ThreeVector GetFinalPosition() const;

    /** Set the position of the track where it was stopped */
    void SetFinalPosition(ThreeVector pos);

    /** Get the momentum of the track when it was stopped */
    ThreeVector GetFinalMomentum() const;

    /** Set the momentum of the track when it was stopped */
    void SetFinalMomentum(ThreeVector pos);

    /** Get the PDG particle type */
    int GetParticleId() const;

    /** Set the PDG particle type */
    void SetParticleId(int id);

    /** Get the GEANT4 assigned id of the track */
    int GetTrackId() const;

    /** Set the GEANT4 assigned id of the track */
    void SetTrackId(int id);

    /** Get the GEANT4 assigned id of the track that spawned this one */
    int GetParentTrackId() const;

    /** Set the GEANT4 assigned id of the track that spawned this one */
    void SetParentTrackId(int id);

    /** Get the vector of step points associated with this track */
    StepArray* GetSteps() const;

    /** Set the vector of step points associated with this track (deleting
     *  existing steps
     */
    void SetSteps(StepArray* steps);

  private:
    StepArray* _steps;

    ThreeVector _initial_position;
    ThreeVector _final_position;
    ThreeVector _initial_momentum;
    ThreeVector _final_momentum;

    int _particle_id;
    int _track_id;
    int _parent_track_id;

    ClassDef(Track, 1)
};
}

#endif
