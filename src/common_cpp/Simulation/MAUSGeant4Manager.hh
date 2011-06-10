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

/** @class MAUSGeant4Manager
 *
 *  Manage Geant4 setup - singleton class that controls the interface with
 *  geant4. This has to be a singleton class so that we can't accidentally set
 *  up geant4 twice.
 *
 *  So some comments about the Geant4 setup. At the moment, we make one event
 *  per primary. This is actually incorrect - really we should be making several
 *  tracks on each Geant4 event - i.e. the spill should be the Geant4 event.
 */

#ifndef _SRC_CPP_CORE_SIMULATION_MAUSGEANT4MANAGER_HH_
#define _SRC_CPP_CORE_SIMULATION_MAUSGEANT4MANAGER_HH_

#include <G4RunManager.hh>
#include <G4SDManager.hh>

// should all be forward declarations?
#include "src/legacy/Simulation/MICERunAction.hh"
#include "src/legacy/Simulation/FillMaterials.hh"
#include "src/legacy/Simulation/MICEDetectorConstruction.hh"
#include "src/common_cpp/Simulation/MAUSPrimaryGeneratorAction.hh"
#include "src/common_cpp/Simulation/MAUSSteppingAction.hh"
#include "src/common_cpp/Simulation/MAUSTrackingAction.hh"

class MICEPhysicsList;

namespace MAUS {

class MAUSGeant4Manager {
  public:
    /** @brief Get the singleton MAUSGeant4Manager
     *
     *  Get the instance of the MAUSGeant4Manager. This will construct the
     *  MAUSGeant4Manager if required.
     */
    static MAUSGeant4Manager* GetInstance();

    /** @brief Get the G4RunManager
     */
    G4RunManager* GetRunManager() const {return _runManager;}

    /** @brief Get the MAUSSteppingAction
     */
    MAUSSteppingAction* GetStepping() const {return _stepAct;}

    /** @brief Get the MAUSSteppingAction
     */
    MAUSTrackingAction* GetTracking() const {return _trackAct;}

    /** @brief Get the MAUSSteppingAction
     */
    MAUSPrimaryGeneratorAction* GetPrimaryGenerator() const {return _primary;}

    /** @brief Get the Geometry
     */
    MICEDetectorConstruction* GetGeometry() const {return _detector;}

    /** @brief Control whether step data is stored for each track
     *
     *  @params willStoreTracks Set to true to store every step for each track
     *          in the Json track. Set to false to only store initial and final
     *          position and momentum
     */
    void SetStoreTracks(bool willStoreTracks) {_storeTracks = willStoreTracks;}

    /** @brief Get the flag controlling whether step data is stored
     */
    bool GetStoreTracks() {return _storeTracks;}

    /** @brief Phased fields in the geometry (e.g. RF cavities)
     *
     *  If there are unphased fields in the geometry, SetPhases will attempt to
     *  phase them using FieldPhaser. Throws an exception if the phasing fails.
     */
    void SetPhases();

  private:
    MAUSGeant4Manager();
    ~MAUSGeant4Manager();

    G4RunManager* _runManager;
    MICEPhysicsList* _physList;
    MAUSPrimaryGeneratorAction* _primary;
    MAUSSteppingAction*          _stepAct;
    MAUSTrackingAction*          _trackAct;
    MICEDetectorConstruction*    _detector;
    bool                         _storeTracks;
};

}  // namespace MAUS

#endif  // _SRC_CPP_CORE_SIMULATION_MAUSGEANT4MANAGER_HH_

