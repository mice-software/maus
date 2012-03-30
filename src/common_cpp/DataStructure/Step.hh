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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_STEP_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_STEP_HH_

#include <vector>

#include "src/common_cpp/DataStructure/ThreeVector.hh"

namespace MAUS {

/** @class Step holds data pertaining to a single step point in GEANT4
 *
 *  Each time a step is made, we can record the position and momentum of the
 *  step point, total energy, time, energy deposited, path length, proper
 *  time of the step.
 */
class Step {
  public:
    /** Initialise to 0
     */
    Step();

    /** Copy constructor */
    Step(const Step& step);

    /** Equality operator */
    Step& operator=(const Step& step);

    /** Destructor - does nothing */
    ~Step();

    /** Returns the position of the step */
    ThreeVector GetPosition() const;
    /** Sets the position of the step */
    void SetPosition(ThreeVector pos);

    /** Returns the momentum of the step */
    ThreeVector GetMomentum() const;

    /** Sets the momentum of the step */
    void SetMomentum(ThreeVector mom);

    /** Returns the proper time of the step */
    double GetProperTime() const;

    /** Sets the proper time of the step */
    void SetProperTime(double tau);

    /** Returns the path length so far of the track */
    double GetPathLength() const;

    /** Sets the path length so far of the track */
    void SetPathLength(double length);

    /** Returns the time at which the step was made */
    double GetTime() const;

    /** Sets the time at which the step was made */
    void SetTime(double time);

    /** Returns the energy of the track making the step */
    double GetEnergy() const;

    /** Sets the energy of the track making the step */
    void SetEnergy(double energy);

    /** Returns the energy deposited during the step */
    double GetEnergyDeposited() const;

    /** Sets the energy deposited during the step */
    void SetEnergyDeposited(double edep);

  private:
    ThreeVector _position;
    ThreeVector _momentum;
    double _proper_time;
    double _path_length;
    double _time;
    double _energy;
    double _energy_deposited;
};

typedef std::vector<Step> StepArray;
}

#endif


