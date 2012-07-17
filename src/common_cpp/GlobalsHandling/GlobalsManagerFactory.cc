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

#include "src/legacy/Interface/Squeal.hh"
#include "src/legacy/Interface/Squeak.hh"
#include "src/legacy/Interface/MICERun.hh"
#include "src/legacy/Interface/MiceMaterials.hh"
#include "src/legacy/Config/MiceModule.hh"


#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/GlobalsHandling/RunActionManager.hh"
#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"

#include "src/common_cpp/GlobalsHandling/GlobalsManager.hh"
#include "src/common_cpp/GlobalsHandling/GlobalsManagerFactory.hh"

namespace MAUS {

// initialisation order is important here!
// first we need datacards
// then legacy mice_run (what this replaces, but some stuff still calls that)
// then logging (uses datacards on legacy_mice_run to set output levels)
// then error handler (uses logging)
// then legacy cards and run data
void GlobalsManagerFactory::InitialiseGlobalsManager(std::string json_datacards) {
    if (GlobalsManager::_process != NULL) {
        throw (Squeal(Squeal::recoverable,
      "Attempt to initialise GlobalsManager when it was already initialised",
                      "GlobalsManagerFactory::InitialiseGlobalsManager"));
    }
    GlobalsManager* process = new GlobalsManager();    
    try {

        Json::Value config = JsonWrapper::StringToJson(json_datacards);
        process->_configuration_cards = new Json::Value(config);
        process->_legacy_mice_run = MICERun::getInstance();
        // used by logging
        process->_legacy_mice_run->jsonConfiguration = 
                                                 process->_configuration_cards;
        int verbose_level = JsonWrapper::GetProperty
                       (config, "verbose_level", JsonWrapper::intValue).asInt();
        // we set up logging but for now leave singleton-like access
        // meaning that we can't reinitialise the logging
        Logging::setStandardOutputs(verbose_level);
        // we set up CppErrorHandler but for now leave singleton-like access
        // meaning that we can't reinitialise the error handler
        process->_error_handler = CppErrorHandler::getInstance();   
        process->_legacy_cards = new dataCards(1);
        process->_legacy_mice_run->DataCards = process->_legacy_cards;
        process->_run_action_manager = new RunActionManager();
        std::string rec_file = JsonWrapper::GetProperty
           (*process->_configuration_cards, "reconstruction_geometry_filename",
            JsonWrapper::stringValue).asString();
        std::string sim_file = JsonWrapper::GetProperty
           (*process->_configuration_cards, "simulation_geometry_filename",
            JsonWrapper::stringValue).asString();
        process->_mc_mods = new MiceModule(sim_file);
        process->_legacy_mice_run->miceModule = process->_mc_mods;
        process->_mice_materials = new MiceMaterials();  // delete
        MICERun::getInstance()->miceMaterials = process->_mice_materials;
        fillMaterials(*MICERun::getInstance());
        process->_maus_geant4_manager = MAUSGeant4Manager::GetInstance();
        process->_field_constructor = MICERun::getInstance()->btFieldConstructor;
        process->_field_constructor->Print(Squeak::mout(Squeak::info));
    } catch(Squeal squee) {
        delete process;
        throw squee;
    }
    GlobalsManager::_process = process;
}

void GlobalsManagerFactory::DeleteGlobalsManager() {
    // we don't delete the MICERun as this isn't really meant to be deleted
    // (it's legacy anyway)
    if (GlobalsManager::_process == NULL) {
        throw (Squeal(Squeal::recoverable,
             "Attempt to delete GlobalsManager when it was already initialised",
                      "GlobalsManagerFactory::DeleteGlobalsManager"));
    }
    if (GlobalsManager::_process->_mice_materials != NULL) {
        delete GlobalsManager::_process->_mice_materials;
    }
    if (GlobalsManager::_process->_maus_geant4_manager != NULL) {
        // delete GlobalsManager::_process->_maus_geant4_manager; wont delete
    }  
    if (GlobalsManager::_process->_field_constructor != NULL) {
        // delete GlobalsManager::_process->_field_constructor;
    }
    if (GlobalsManager::_process->_recon_mods != NULL) {
        delete GlobalsManager::_process->_recon_mods;
    }
    if (GlobalsManager::_process->_mc_mods != NULL) {
        delete GlobalsManager::_process->_mc_mods;
    }
    if (GlobalsManager::_process->_run_action_manager != NULL) {
        delete GlobalsManager::_process->_run_action_manager;
    }
    if (GlobalsManager::_process->_legacy_cards != NULL) {
        delete GlobalsManager::_process->_legacy_cards;
    }
    if (GlobalsManager::_process->_error_handler != NULL) {
        // won't delete
        // delete GlobalsManager::_process->_error_handler;
    }
    if (GlobalsManager::_process->_legacy_mice_run != NULL) {
        // won't delete (messes up loads of stuff)
        // delete GlobalsManager::_process->_legacy_mice_run;
    }
    if (GlobalsManager::_process->_configuration_cards != NULL) {
        // won't delete (messes up Squeak)
        // delete GlobalsManager::_process->_configuration_cards;
    }
    delete GlobalsManager::_process;
    GlobalsManager::_process = NULL;
};
}

