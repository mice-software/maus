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

#ifndef _SRC_COMMON_CPP_UTILS_GLOBALS_HH_
#define _SRC_COMMON_CPP_UTILS_GLOBALS_HH_

#include <string>

#include "json/json.h"

#include "Utils/Squeak.hh"

// legacy classes outside the MAUS namespace
class dataCards;
class MICERun;
class MiceModule;
class BTFieldConstructor;

namespace MAUS {

class RunActionManager;
class CppErrorHandler;
class GeometryNavigator;
class SciFiGeometryHelper;

class MAUSGeant4Manager;

// I decided not to make this a user interfaceable class. I can change that if
// we find we need lots of things set up at the beginning of the job...

/** @class Globals - Handler for global objects.
 *
 *  The Globals holds data associated with a particular execution of the
 *  MAUS code. The Globals is a singleton class. Note that in distributed
 *  computing mode there will be one MAUSProcess per processing node.
 *
 *  This is quite a low level object, so it just hands pointers around. For 
 *  example don't want to require GEANT4 or anything to interact with this class
 *  and don't want to risk circular dependencies. For this reason instantiation
 *  is handled by GlobalsManager class, which is high level and does need
 *  to know about all that stuff. Also has destructor there. I would rather keep
 *  construction and destruction in the class, but I think it makes circular
 *  references which can be unpleasant.
 *
 *  Note also that one feature is that (except GEANT4, which won't fix) it would
 *  be nice if we take out all of the global/static variables and initialise
 *  things here instead. So that we can in principle do a delete and
 *  reinitialise and get a completely clean set up. This is not completely fixed
 *  yet - next thing is to remove src/legacy/MICERun.hh and fix everything that
 *  breaks
 */
class Globals {
  public:
    /** Return a pointer to the singleton instance.
     *
     *  This returns a pointer to the singleton instance or throws an exception
     *  if it is not initialised yet. Initialisation should have been done by
     *  GlobalsManager.
     *
     *  Globals retains ownership of this memory
     */
    static Globals* GetInstance();

    /** Return true if the singleton has been initialised, else false
     *
     *  Globals retains ownership of this memory
     */
    static bool HasInstance();

    /** Get RunActionManager. Data updated run-by-run.
     *
     *  Globals retains ownership of this memory
     */
    static RunActionManager* GetRunActionManager();

    /** Get CppErrorHandler. Controls what happens to exceptions.
     *
     *  Globals retains ownership of this memory
     */
    static CppErrorHandler* GetErrorHandler();

    /** Get Configuration datacards. Global run controls.
     *
     *  Globals retains ownership of this memory
     */ 
    static Json::Value* GetConfigurationCards();

    /** Get legacy datacards. Old global run controls (please dont use)
     *
     *  Globals retains ownership of this memory
     */
    static dataCards* GetLegacyCards();

    /** Get the Monte Carlo BTFieldConstructor (controls access to field maps)
     *
     *  Globals retains ownership of this memory
     */
    static BTFieldConstructor* GetMCFieldConstructor();

    /** Get the Reconstruction BTFieldConstructor (controls access to field maps)
     *
     *  If simulation_geometry_filename is the same as 
     *  reconstruction_geometry_filename, this really points at the same memory
     *  (optimisation to avoid having to build the same thing twice)
     *
     *  Globals retains ownership of this memory
     */
    static BTFieldConstructor* GetReconFieldConstructor();


    /** Get the Geant4Manager (controls access to G4 objects)
     *
     *  If simulation_geometry_filename is the same as 
     *  reconstruction_geometry_filename, this really points at the same memory
     *  (optimisation to avoid having to build the same thing twice)
     *
     *  Globals retains ownership of this memory
     */
    static MAUSGeant4Manager* GetGeant4Manager();

    /** Get the MC Geometry MiceModules
     *
     *  MiceModules is a tree of geometry information - this returns the root of
     *  the tree
     *
     *  Concept is to have reconstruction geometry and separate MC geometry so
     *  that we can do MC vs Recon studies
     *
     *  If simulation_geometry_filename is the same as 
     *  reconstruction_geometry_filename, this really points at the same memory
     *  (optimisation to avoid having to build the same thing twice)
     *
     *  Globals retains ownership of this memory
     */
    static MiceModule* GetMonteCarloMiceModules();

    /** Get the Reconstruction Geometry MiceModules
     *
     *  MiceModules is a tree of geometry information - this returns the root of
     *  the tree.
     *
     *  Concept is to have reconstruction geometry and separate MC geometry so
     *  that we can do MC vs Recon studies
     *
     *  If simulation_geometry_filename is the same as 
     *  reconstruction_geometry_filename, this really points at the same memory
     *  (optimisation to avoid having to build the same thing twice)
     *
     *  Globals retains ownership of this memory
     */
    static MiceModule* GetReconstructionMiceModules();

    /** Get the G4 World Volume
     *  
     *  Returns the pointer to the physical volume of the current MC geometry,
     *  as loaded into memory.
     *
     *  This pointer can be used with a G4Navigator to allow the user to inspect
     *  the current geometry.
     *
     *  Globals retains ownership of this memory
     */
    static GeometryNavigator* GetMCGeometryNavigator();

    /** Get the SciFi Geometry Helper
     *  
     *  Stores shortcut routines, helper functions and a map of the tracker
     *  construction for use during reconstruction routines.
     *
     *  Globals retains ownership of this memory
     */
    static SciFiGeometryHelper* GetSciFiGeometryHelper();

    /** Get the version number like x.y.z
     *
     *  Return the version number x.y.z
     */
    static std::string GetVersionNumberString();

  private:
    // construction, destruction etc disallowed
    Globals();
    Globals(const Globals& process);
    Globals& operator=(const Globals& process);
    ~Globals();

    Json::Value* _configuration_cards;
    MICERun* _legacy_mice_run;
    CppErrorHandler* _error_handler;
    dataCards* _legacy_cards;
    RunActionManager* _run_action_manager;
    MiceModule* _mc_mods;
    MiceModule* _recon_mods;
    BTFieldConstructor* _mc_field_constructor;
    BTFieldConstructor* _recon_field_constructor;
    MAUSGeant4Manager* _maus_geant4_manager;
    GeometryNavigator* _mc_geometry_navigator;
    SciFiGeometryHelper* _scifi_geometry_helper;
    static Globals* _process;
    // responsible for construction etc
    friend class GlobalsManager;
};
}  // namespace MAUS

#endif  // _SRC_COMMON_CPP_UTILS_GLOBALS_HH_
