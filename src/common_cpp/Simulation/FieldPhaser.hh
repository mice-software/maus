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
 *
 */

#ifndef _SRC_COMMON_CPP_SIMULATION_CAVITYPHASER_HH_
#define _SRC_COMMON_CPP_SIMULATION_CAVITYPHASER_HH_

#include "src/common_cpp/Simulation/MAUSPrimaryGeneratorAction.hh"
#include "src/common_cpp/Simulation/VirtualPlanes.hh"
#include "src/common_cpp/DataStructure/VirtualHit.hh"
#include "src/legacy/BeamTools/BTPhaser.hh"

// Exceptionally, this class is tested at integration test level only. Really
// no point in testing at unit level.

namespace MAUS {

/** @class FieldPhaser
 *
 *  There are two classes that contribute to setting of the RF phase. BTPhaser
 *  class handles set up from the field maps. FieldPhaser handles execution of
 *  phasing routine using Simulation tools.
 */

class FieldPhaser {
 public:
  /** @brief Constructor initialises to defaults
   */
  FieldPhaser();

  /** @brief Destructor - no memory alloc'd
   */
  ~FieldPhaser();

  /** @brief Automatic routine to set the phases on RF cavities
   *
   *  MAUS can set phases on RF cavities automatically. The idea here is to
   *  throw particles at the RF cavities and measure when they pass through
   *  the RF cavities. Measurement is used by making a set of VirtualPlanes one
   *  for each RF cavity. If the particle passes through an RF cavity with
   *  difference between cavity phase and particle phase less than some tolerance
   *  the cavity is considered phased.
   *
   *  Some notes:\n
   *    If the particle fails to phase an RF cavity we throw an exception
   *    We use methods in MAUSGeant4Manager to move virtual planes during the
   *      execution of the phaser; we move them back at the end
   *    We use methods in MAUSPhysicsList to turn off stochastic processes
   *      during the execution of the phaser; we turn them on at the end
   */
  void SetPhases();

 private:
  void SetUp();

  void TearDown();

  void MakeVirtualPlanes(BTPhaser::FieldForPhasing* cavity);

  MAUSPrimaryGeneratorAction::PGParticle TryToPhase(std::vector<MAUS::VirtualHit>* v_hits);

  MAUSPrimaryGeneratorAction::PGParticle VirtualHitToPGParticle(MAUS::VirtualHit hit);

  VirtualPlaneManager* _phaserVirtualPlanes;
  VirtualPlaneManager* _g4managerVirtualPlanes;
};
}  // namespace

#endif  // ifdef _...CAVITYPHASER_HH_

