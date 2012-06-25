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

#ifndef _SRC_COMMON_CPP_GLOBALSHANDLING_PROCESSDATAMANAGER_HH_
#define _SRC_COMMON_CPP_GLOBALSHANDLING_PROCESSDATAMANAGER_HH_

#include <string>

#include "json/json.h"

class Squeak;
class dataCards;
class MICERun;

namespace MAUS {

typedef Squeak Logging;
class DataRunActionManager;
class CppErrorHandler;

class MAUSGeant4Manager;
class BTFieldConstructor;
class MiceModule;

// I decided not to make this a user interfaceable class. I can change that if
// we find we need lots of things set up at the beginning of the job...

/** @class ProcessDataManager - Handler for global objects.
 *
 *  The MAUSProcess holds data associated with a particular execution of the
 *  MAUS code. The MAUSProcess is a singleton class. Note that in distributed
 *  computing mode there will be one MAUSProcess per processing node.
 *
 *  This is quite a low level object, so it just hands pointers around. For 
 *  example don't want to require GEANT4 or anything to interact with this class
 *  and don't want to risk circular dependencies.
 */
class PerProcessDataManager {
  public:
    /** Initialise the singleton instance.
     *
     *  This initialises the singleton instance or throws an exception if it is
     *  already initialised (as we can initialise with the wrong configuration).
     *
     *  Nb: this should be initialised in Go (somewhere early in the MAUS
     *  setup)
     */
    static void Initialise(std::string json_datacards);

    /** Return a pointer to the singleton instance
     *
     *  This returns a pointer to the singleton instance or throws an exception
     *  if it is not initialised yet.
     */
    static PerProcessDataManager* GetInstance();

    /** Return true if the singleton has been initialised, else false
     */
    static bool HasInstance(); 

    /** Deallocate memory and set singleton instance to NULL
     *
     *  It is possible to delete the PerProcessManager and then reinitialise
     *  with different datacards - but devs should never need to do this (please
     *  don't unless you know what you are doing).
     */
    ~PerProcessDataManager();

    /** Get PerRunDataManager. Data updated run-by-run.
     */
    static DataRunActionManager* GetRunActionManager();

    /** Get CppErrorHandler. Controls what happens to exceptions.
     */
    static CppErrorHandler* GetErrorHandler();

    /** Get Configuration datacards. Global run controls.
     */ 
    static Json::Value* GetConfigurationCards();

    /** Get legacy datacards. Old global run controls (please dont use)
     */ 
    static dataCards* GetLegacyCards();

    /** Get the BTFieldConstructor field map class
     */
    static BTFieldConstructor* GetBTFieldConstructor();

    /** Get the Geant4Manager (controls access to G4 objects)
     */
    static MAUSGeant4Manager* GetGeant4Manager();

    /** Get the MiceModules (owns geometry information)
     */
    static MiceModule* GetMiceModules();

    /** JobHeader */
    /** JobFooter */
    /** RunHeader */
    /** RunFooter */

  private:
    PerProcessDataManager();
    PerProcessDataManager(const PerProcessDataManager& process);
    PerProcessDataManager& operator=(const PerProcessDataManager& process);

    Json::Value* _configuration_cards;
    MICERun* _legacy_mice_run;
    CppErrorHandler* _error_handler;
    dataCards* _legacy_cards;
    DataRunActionManager* _run_action_manager;
    static PerProcessDataManager* _process;
};
}

#endif
