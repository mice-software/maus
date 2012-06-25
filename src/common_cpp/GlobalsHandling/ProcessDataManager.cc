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

#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/GlobalsHandling/PerRunDataManager.hh"
#include "src/common_cpp/GlobalsHandling/ProcessDataManager.hh"

namespace MAUS {

PerProcessDataManager* PerProcessDataManager::_process = NULL;

// initialisation order is important here!
// first we need datacards
// then legacy mice_run (what this replaces, but some stuff still calls that)
// then logging (uses datacards on legacy_mice_run to set output levels)
// then error handler (uses logging) 
// then legacy cards and run data
void PerProcessDataManager::Initialise(std::string json_datacards) {
    if (_process != NULL) {
        throw (Squeal(Squeal::recoverable,
      "Attempt to initialise PerProcessManager when it was already initialised",
                      "PerProcessDataManager::Initialise"));
    }
    _process = new PerProcessDataManager();    
    try {
        Json::Value config = JsonWrapper::StringToJson(json_datacards);
        _process->_configuration_cards = new Json::Value(config);
        _process->_legacy_mice_run = MICERun::getInstance();
        // used by logging
        _process->_legacy_mice_run->jsonConfiguration = 
                                                 _process->_configuration_cards;
        int verbose_level = JsonWrapper::GetProperty
                       (config, "verbose_level", JsonWrapper::intValue).asInt();
        // we set up logging but for now leave singleton-like access
        // meaning that we can't reinitialise the logging
        Logging::setStandardOutputs(verbose_level);
        // we set up CppErrorHandler but for now leave singleton-like access
        // meaning that we can't reinitialise the error handler
        _process->_error_handler = CppErrorHandler::getInstance();        
        _process->_legacy_cards = new dataCards();
        _process->_run_action_manager = new DataRunActionManager();
    } catch(Squeal squee) {
        delete _process;
        _process = NULL;
        throw squee;
    }
}

PerProcessDataManager* PerProcessDataManager::GetInstance() {
    if (_process != NULL) {
        return _process;
    } else {
        throw(Squeal(Squeal::recoverable,
                     std::string("Attempt to get PerProcessManager ")+
                          "instance when it has not been initialised",
                          "PerProcessDataManager::GetInstance()"));
    }
    return NULL; // appease gcc
}

PerProcessDataManager::~PerProcessDataManager() {
    // we don't delete the MICERun as this isn't really meant to be deleted
    // (it's legacy anyway)
    if (_run_action_manager != NULL) {
        delete _run_action_manager;
    }
    if (_legacy_cards != NULL) {
        delete _legacy_cards;
    }
    if (_error_handler != NULL) {
        delete _error_handler;
    }
    if (_configuration_cards != NULL) {
        delete _configuration_cards;
    }
    // somehow, this should always == _process
    if (this == _process) {
        _process = NULL;
    } else {
      // ack - exception in destructor is bad - but should never happen
      // use nonRecoverable flag to say "something nasty happened, please exit"
      throw(Squeal(Squeal::nonRecoverable,
                   "Multiple PerProcessDataManager instances",
                   "~PerProcessDataManager"));
    }
}

bool PerProcessDataManager::HasInstance() {
    return _process != NULL;
}

PerProcessDataManager::PerProcessDataManager()
    : _configuration_cards(NULL), _legacy_mice_run(NULL), _error_handler(NULL),
      _legacy_cards(NULL), _run_action_manager(NULL) {
}

// in all the below I call GetInstance() to check that _process is initialised
DataRunActionManager* PerProcessDataManager::GetRunActionManager() {
    return GetInstance()->_run_action_manager;
}

CppErrorHandler* PerProcessDataManager::GetErrorHandler() {
    return GetInstance()->_error_handler;
}

Json::Value* PerProcessDataManager::GetConfigurationCards() {
    return GetInstance()->_configuration_cards;
}

dataCards* PerProcessDataManager::GetLegacyCards() {
    return GetInstance()->_legacy_cards;
}
}

