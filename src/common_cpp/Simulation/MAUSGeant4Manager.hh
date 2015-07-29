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

#ifndef _SRC_COMMON_CPP_SIMULATION_MAUSGEANT4MANAGER_HH_
#define _SRC_COMMON_CPP_SIMULATION_MAUSGEANT4MANAGER_HH_


#include <string>
#include <vector>

#include "Geant4/G4RunManager.hh"
#include "Geant4/G4SDManager.hh"
#include "Geant4/G4GDMLParser.hh"

// should all be forward declarations? yes - but be careful about namespace
#include "src/legacy/Simulation/FillMaterials.hh"
#include "src/common_cpp/Simulation/DetectorConstruction.hh"
#include "src/common_cpp/Simulation/MAUSPrimaryGeneratorAction.hh"
#include "src/common_cpp/Simulation/MAUSSteppingAction.hh"
#include "src/common_cpp/Simulation/MAUSTrackingAction.hh"
#include "src/common_cpp/Simulation/MAUSEventAction.hh"
#include "src/common_cpp/Simulation/VirtualPlanes.hh"

class BTFieldConstructor;

namespace MAUS {

class MAUSVisManager;
class MAUSPhysicsList;
class MCEvent;

namespace Simulation {
class DetectorConstruction;
}

/** @class MAUSGeant4Manager
 *
 *  @brief Manage Geant4 setup - singleton class that controls the interface with
 *  geant4.
 *
 *  This has to be a singleton class so that we can't accidentally set
 *  up geant4 twice.
 *
 *  Geant4 setup:
 *  We have a few different classes 
 */
class MAUSGeant4Manager {
  public:
    /** @brief Get the singleton MAUSGeant4Manager
     *
     *  Get the instance of the MAUSGeant4Manager. This will construct the
     *  MAUSGeant4Manager if required.
     *
     *  Note: Construction requires only MC MiceModules and Configuration cards
     *  to be defined on Globals (this is part of the general globals set up so
     *  we like to know dependencies).
     */
    static MAUSGeant4Manager* GetInstance();

    /** @brief Return true if the MAUSGeant4Manager is already initialised
     */
    static bool HasInstance();

    /** @brief Get the G4RunManager
     */
    G4RunManager* GetRunManager() const {return _runManager;}

    /** @brief Get the MAUSSteppingAction
     */
    MAUSSteppingAction* GetStepping() const {return _stepAct;}

    /** @brief Get the MAUSSteppingAction
     */
    MAUSTrackingAction* GetTracking() const {return _trackAct;}

    /** @brief Get the MAUSEventAction
     */
    MAUSEventAction* GetEventAction() const {return _eventAct;}

    /** @brief Get the MAUSSteppingAction
     */
    MAUSPrimaryGeneratorAction* GetPrimaryGenerator() const {return _primary;}

    /** @brief Get the MAUSPhysicsList
     */
    MAUSPhysicsList* GetPhysicsList() const {return _physList;}

    /** @brief Get the Geometry
     */
    Simulation::DetectorConstruction* GetGeometry() const {return _detector;}

    /** @brief Get the VirtualPlanes
     */
    VirtualPlaneManager* GetVirtualPlanes() const {return _virtPlanes;}

    /** @brief Set the VirtualPlanes
     *
     *  MAUSGeant4Manager takes ownership of memory allocated to the planes
     */
    inline void SetVirtualPlanes(VirtualPlaneManager* virt);

    /** @brief Phased fields in the geometry (e.g. RF cavities)
     *
     *  If there are unphased fields in the geometry, SetPhases will attempt to
     *  phase them using FieldPhaser. Throws an exception if the phasing fails.
     */
    void SetPhases();

    /** @brief Get the reference particle from json configuration
     */
    MAUSPrimaryGeneratorAction::PGParticle GetReferenceParticle();

    /** @brief Run a particle through the simulation
     *
     *  @returns a json object with tracking, virtual hits and real hits
     */
    MCEvent* RunParticle(MAUSPrimaryGeneratorAction::PGParticle p);

    /** @brief Run a particle through the simulation
     *
     *  @returns copy of particle with any tracking output appended
     *           to the particle. Following branches will be overwritten with
     *           tracking output from this event:\n
     *             "tracks", "virtual_hits", "hits"
     */
    MCEvent* RunParticle(MAUS::Primary particle);

    /** @brief Run an array of particles through the simulation
     *
     *  @param particle_array should conform to spill schema for spill->mc
     *         branch (so array of Json values containing objects, each of which
     *         has a primary branch containing the primary particle data)
     *
     *  @returns an array of particles with any new hits, virtual_hits or tracks
     *  appended
     */
    Json::Value RunManyParticles(Json::Value particle_array);

    /** @brief Run a vector of particles through the simulation
     *
     *  @param mc_events to run; MAUSGeant4Manager takes ownership of the memory
     *                   assigned to mc_events
     *
     *  @returns a vector of particles with any new hits, virtual_hits or tracks
     *  appended; caller owns the memory
     */
    std::vector<MCEvent*>* RunManyParticles(std::vector<MCEvent*>* mc_events);

    /** @brief Get the visualisation manager or return NULL if vis is inactive
     *
     *  Visualisation requires use_visualisation configuration variable set
     */
    MAUSVisManager* GetVisManager() {return _visManager;}

    /** @brief Get the MAUS field manager object
     */
    BTFieldConstructor* GetField();

    /** Delete the manager and close any existing simulation
     *
     *  Note that it is a feature of Geant4 that the geometry cannot be reopened
     */
    ~MAUSGeant4Manager();

    /** Set the auxiliary information for the GDML objects 
     *  
     * Right now this is almost exclusively sensitive detector information
     * but it may be expanded to include visualization information if 
     * necessary.
     */
    void SetAuxInformation(MiceModule& module);

    /** Set the sensitive detector information 
     *
     *  Add the sensitive detector designation to a volume
     */
    void DefineSensitiveDetector(MiceModule& module, G4LogicalVolume* myvol,
				 std::string sensdetname);
    /** Set the sensitive detector information for daughter volumes 
     *
     *  Recursively examine logical volumes for daughters and add
     *  the sensitive detector designation 
     */
    void SetDaughterSensitiveDetectors(G4LogicalVolume* logic);

    /** Set the user limits on step wise processes
     *
     * Recursively examine logical volumes for daughers and add
     * the user limit information
     */
    void SetDaughterUserLimits(G4LogicalVolume* logic);

    /** Reset the simulation with a new geometry set
     *
     *  @param module root module of the new geometry set. Field definitions and
     *         Geant4 geometry definitions will be taken from this module.
     *
     *  Resets the phases if the field on Globals has been set (this is a
     *  dependency for restting the phase).
     */
    void SetMiceModules(MiceModule& module);

  private:
    MAUSGeant4Manager();
    MAUSGeant4Manager(const MAUSGeant4Manager& copy);
    MAUSGeant4Manager& operator=(const MAUSGeant4Manager& copy);

    G4RunManager* _runManager;
    MAUSPhysicsList* _physList;
    MAUSPrimaryGeneratorAction* _primary;
    MAUSSteppingAction*         _stepAct;
    MAUSTrackingAction*         _trackAct;
    MAUSEventAction*            _eventAct;
    Simulation::DetectorConstruction*  _detector;
    G4GDMLParser                _parser;
    VirtualPlaneManager*        _virtPlanes;
    MAUSVisManager*             _visManager;

    void SetVisManager();
    void BeamOn(int number_of_particles);

    MCEvent* Tracking(MAUSPrimaryGeneratorAction::PGParticle p);

    // std::vector<G4UserLimits*> _userLims;


    static MAUSGeant4Manager* _instance;
    static bool _isClosed;
    G4double _keThreshold;
    G4double _trackMax;
    G4double _timeMax;
    G4double _stepMax;
};

inline void MAUSGeant4Manager::SetVirtualPlanes(VirtualPlaneManager* virt) {
    if (_virtPlanes != NULL)
        delete _virtPlanes;
    _virtPlanes = virt;
}
}  // namespace MAUS

#endif  // _SRC_CPP_CORE_SIMULATION_MAUSGEANT4MANAGER_HH_

