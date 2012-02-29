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

/** @class Primary holds data for the primary particle that initiated the event
 *
 *  Primary holds the dynamical variables (position, momentum direction, energy,
 *  time) as well as Particle ID (PDG value) and random seed fed to GEANT4.
 */
class Primary {
 public:
  /** Initialise everything to 0
   */
  Primary();

  /** Copy constructor
   */
  Primary(const Primary& md);
       
  /** Equality operator
   */
  Primary& operator=(const Primary& md);
  
  /** Destructor (does nothing)
   */
  virtual ~Primary();

  /** Get the PDG particle ID
   */
  int GetParticleId() const;

  /** Set the PDG particle ID
   */
  void SetParticleId(int id);
  
  /** Get the random seed (used by GEANT4)
   */
  int GetRandomSeed() const;

  /** Set the random seed (used by GEANT4)
   */
  void SetRandomSeed(int seed);

  /** Get the particle total energy
   */
  double GetEnergy() const;

  /** Set the particle total energy
   */
  void SetEnergy(double energy);

  /** Get the particle time (relative to 0, some arbitrary reference time)
   */
  double GetTime() const;

  /** Set the particle time (relative to 0, some arbitrary reference time)
   */
  void SetTime(double time);

  /** Get the particle position
   */
  ThreeVector GetPosition() const;

  /** Set the particle position
   */
  void SetPosition(ThreeVector pos);

  /** Get the particle momentum
   */
  ThreeVector GetMomentum() const;

  /** Set the particle momentum
   */
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

