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

class Step {
  public:
    Step();

    Step(const Step& step);    

    Step& operator=(const Step& step);

    ~Step();

    ThreeVector GetPosition() const;
    void SetPosition(ThreeVector pos);

    ThreeVector GetMomentum() const;
    void SetMomentum(ThreeVector mom);

    double GetProperTime() const;
    void SetProperTime(double tau);

    double GetPathLength() const;
    void SetPathLength(double length);

    double GetTime() const;
    void SetTime(double time);    

    double GetEnergy() const;
    void SetEnergy(double energy);

    double GetEnergyDeposited() const;
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


