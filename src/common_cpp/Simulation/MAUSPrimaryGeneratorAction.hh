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

#ifndef _SRC_CPP_CORE_SIMULATION_MAUSPRIMARYGENERATORACTION_HH_
#define _SRC_CPP_CORE_SIMULATION_MAUSPRIMARYGENERATORACTION_HH_

#include <string>
#include <queue>

#include "Geant4/G4ParticleGun.hh"
#include "Geant4/G4ParticleTable.hh"
#include "Geant4/G4VUserPrimaryGeneratorAction.hh"  // inherit from

#include "src/legacy/Interface/Squeak.hh"
#include "src/legacy/Interface/VirtualHit.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/DataStructure/Primary.hh"

// TODO (Rogers): should use src/common_cpp/DataStructure/Primary.hh rather than
//                PGParticle

namespace MAUS {

/** @class  MAUSPrimaryGeneratorAction
 *  Geant4 calls this class to determine the events in the spill
 *
 *  This is just a FIFO (First In First Out) std::queue of hits. Hits can be
 *  loaded using Push(...) and unloaded using Pop(). GeneratePrimaries(...)
 *  will fire hits until the queue is empty.
 */
class MAUSPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
 public:
  /** @class PGParticle
   *
   *  Simple subclass to store collection of data associated with a 
   *  MAUSPrimaryGeneratorAction event. 
   *  (x,y,z) is initial position in Cartesian coordinates
   *  (px,py,pz) is initial momentum direction in Cartesian coordinates
   *  energy is initial total energy
   *  time is initial global time
   *  pid is the pdg number of the particle
   *  seed is the random seed
   */
  class PGParticle {
   public:
    /// Initialise to 0
    PGParticle();
    /// Initialise from Virtual Hit
    explicit PGParticle(VirtualHit hit);
    /// Re-initialise from Json
    void ReadJson(Json::Value pg_particle);
    /// Re-initialise from Json
    void ReadCpp(MAUS::Primary primary);
    /// Change momentum to set the mass shell condition (E^2 = p^2+m^2)
    void MassShellCondition();
    /// Represent as a Json object
    Json::Value WriteJson();
    /// Represent as a "primary" particle object
    Primary GetPrimary();
    double x, y, z, time, px, py, pz, sx, sy, sz, energy;
    int pid;
    unsigned int seed;
  };

  /** @brief Construct the MAUSPrimaryGeneratorAction
   */
  MAUSPrimaryGeneratorAction();

  /** @brief Destruct the MAUSPrimaryGeneratorAction
   */
  ~MAUSPrimaryGeneratorAction();

  /** @brief Pop the particle
   *  
   *  Take the first event loaded using Push() and put the value into argEvent.
   *  Remove that event from the queue. Throw an exception if I run out of
   *  events, the PID is not recognised or the energy is non-physical.
   *
   *  @params argEvent Load the particle into this event
   */
  void GeneratePrimaries(G4Event * argEvent);

  /** @brief Push a particle onto the back of the queue
   */
  void Push(PGParticle particle) {_part_q.push(particle);}

  /** @brief Remove the particle from the front of the queue
   *
   *  @returns the value of the particle on the front of the queue
   */
  PGParticle Pop();

  /** @brief Return true if (x, y, z) are in the world volume
   *
   *  Checks the dimensions of the root MiceModule to find if x, y, z are
   *  inside. Returns true if they are. Always returns true if the root
   *  MiceModule is NULL.
   */
  bool isInWorldVolume(double x, double y, double z);

 protected:
  G4ParticleGun*          gun;
  static MAUSPrimaryGeneratorAction* self;

 private:
  std::queue<PGParticle> _part_q;
};

}  // ends MAUS namespace

#endif  // _SRC_CPP_CORE_SIMULATION_MAUSPRIMARYGENERATORACTION_HH_
