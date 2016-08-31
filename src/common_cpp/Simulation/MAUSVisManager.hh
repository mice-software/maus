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

#ifndef SRC_COMMON_CPP_SIMULATION_MAUSVISMANAGER_HH
#define SRC_COMMON_CPP_SIMULATION_MAUSVISMANAGER_HH

#include <string>

#include "Geant4/G4VisManager.hh"
#include "Geant4/G4UImanager.hh"

#include "Utils/Squeak.hh"

namespace MAUS {

class MAUSVisManager: public G4VisManager {
 public:
  /** Constructor does nothing
   */
  MAUSVisManager();

  /** Destructor does nothing
   */
  ~MAUSVisManager();

  /** @brief Set up visualisation for a particular beamOn command
   *
   *  Resets the viewer; sets the zoom, camera angle etc based on configuration
   *  commands
   */
  void SetupRun();

  /** @brief Clear up anything that needs clearing from the run setup
   *
   *  Flushes the viewer buffer (force to write a graphics file).
   */
  void TearDownRun();

 private:
  void RegisterGraphicsSystems();
  void ApplyCommand(std::string cmd) {
    Squeak::mout(Squeak::debug) << "g4ui> " << cmd << std::endl;
    G4UImanager::GetUIpointer()->ApplyCommand(cmd);
  }
};
}

#endif
