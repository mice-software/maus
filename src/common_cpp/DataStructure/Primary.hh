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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_PRIMARY_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_PRIMARY_HH_

#include <vector>
#include "src/common_cpp/DataStructure/ThreeVector.hh"

#include "Rtypes.h" // ROOT

namespace MAUS {

class Primary {
 public:
  Primary();

  Primary(const Primary& md);
       
  Primary& operator=(const Primary& md);
  
  virtual ~Primary();

  int GetParticleId() const;
  void SetParticleId(int id);
  
  int GetRandomSeed() const;
  void SetRandomSeed(int seed);

  double GetEnergy() const;
  void SetEnergy(double energy);

  double GetTime() const;
  void SetTime(double time);

  ThreeVector GetPosition() const;
  void SetPosition(ThreeVector pos);

  ThreeVector GetMomentum() const;
  void SetMomentum(ThreeVector mom);

 private:

  int _seed;
  int _particle_id;
  double _energy;
  double _time;
  ThreeVector _position;
  ThreeVector _momentum;

  ClassDef(Primary, 1)
};

}

#endif

