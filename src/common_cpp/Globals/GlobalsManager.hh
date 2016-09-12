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

#ifndef _SRC_COMMON_CPP_GLOBALS_GlobalsManager_HH_
#define _SRC_COMMON_CPP_GLOBALS_GlobalsManager_HH_

#include <string>

#include "Utils/Squeak.hh"

// legacy classes outside the MAUS namespace
class dataCards;
class MICERun;
class MiceModule;
class BTFieldConstructor;

namespace MAUS {

typedef Squeak Logging;
class RunActionManager;
class CppErrorHandler;

class MAUSGeant4Manager;

/** GlobalsManager initialises the Globals
 *
 *  I want to keep it so that the Globals is quite a low level object -
 *  just handing pointers around. But some objects, e.g. MAUSGeant4Manager, are
 *  quite high level objects. The only time I need to know about them is during
 *  instantiation, so I make a high level Factory class to instantiate them.
 *
 *  Could probably be a namespace, but I make it a class just in case I need in
 *  the future some member data (occasionally happens).
 *
 *  Friend of Globals.
 *
 *  Tested with Globals.
 */

class GlobalsManager {
  public:
    /** Initialise the Globals.
     *
     *  This initialises the singleton instance or throws an exception if it is
     *  already initialised (as we can initialise with the wrong configuration).
     *
     *  Nb: this should be initialised in Go (somewhere early in the MAUS
     *  setup)
     */
    static void InitialiseGlobals(std::string json_datacards);

    /** Deallocate memory and set singleton instance to NULL
     *
     *  It is possible to delete the Globals and then reinitialise
     *  with different datacards - but devs should never need to do this (please
     *  don't unless you know what you are doing).
     */
    static void DeleteGlobals();

    /** Set the Reconstruction Geometry MiceModules
     *
     *  Globals takes ownership of memory allocated to root_module
     */
    static void SetReconstructionMiceModules(MiceModule* root_module);

    /** Set the MC Geometry MiceModules
     *
     *  Globals takes ownership of memory allocated to root_module. If
     *  root_module is non-NULL, this triggers also an update of the mc fields
     *  and geant4 geometry. If root_module is NULL, then mc fields and geant4
     *  are left untouched.
     */
    static void SetMonteCarloMiceModules(MiceModule* root_module);

    /** Set legacy datacards. Old global run controls (please dont use)
     *
     *  Globals takes ownership of memory allocated to cards
     */
    static void SetLegacyCards(dataCards* cards);

    /** Set RunActionManager. Data updated run-by-run.
     *
     *  Globals takes ownership of memory allocated to manager
     */
    static void SetRunActionManager(RunActionManager* manager);

    /** Do any final memory clean-up before exiting the program.
     *
     *  Following execution of this function, MAUS will no longer be usable
     */
    static void Finally();

  private:
    // disallow copy and assign
    GlobalsManager();
    ~GlobalsManager();
    GlobalsManager(const GlobalsManager& glob);
    GlobalsManager& operator=(const GlobalsManager& glob);
};
}

#endif //  _SRC_COMMON_CPP_GLOBALSHANDLING_GlobalsManager_HH_


    // some stuff doesn't work yet and is commented
    // we will never be able to do Geant4, but the rest should be fixable given
    // time...

    /** Set the Geant4Manager (controls access to G4 objects)
     */
    // static void SetGeant4Manager(MAUSGeant4Manager* manager);

    /** Set the BTFieldConstructor (controls access to field maps)
     */
    // static void SetBTFieldConstructor(BTFieldConstructor* field);

    /** Set Configuration datacards. Global run controls.
     */ 
    // static void SetConfigurationCards(Json::Value* cards);

    /** Set CppErrorHandler. Controls what happens to exceptions.
     */
    // static void SetErrorHandler(CppErrorHandler* handler);


