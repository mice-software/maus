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
#include "src/common_cpp/GlobalsHandling/GlobalsManager.hh"

namespace MAUS {

GlobalsManager* GlobalsManager::_process = NULL;

GlobalsManager* GlobalsManager::GetInstance() {
    if (_process != NULL) {
        return _process;
    } else {
        // watch out here, error handler not initialised(!)
        throw(Squeal(Squeal::recoverable,
                     std::string("Attempt to get GlobalsManager ")+
                          "instance when it has not been initialised",
                          "GlobalsManager::GetInstance()"));
    }
    return NULL; // appease gcc
}

GlobalsManager::~GlobalsManager() {
}

bool GlobalsManager::HasInstance() {
    return _process != NULL;
}

GlobalsManager::GlobalsManager()
  : _configuration_cards(NULL), _legacy_mice_run(NULL), _error_handler(NULL),
    _legacy_cards(NULL), _run_action_manager(NULL), _mc_mods(NULL),
    _recon_mods(NULL), _maus_geant4_manager(NULL), _mice_materials(NULL) {
}

// in all the below I call GetInstance() to check that _process is initialised
RunActionManager* GlobalsManager::GetRunActionManager() {
    return GetInstance()->_run_action_manager;
}

CppErrorHandler* GlobalsManager::GetErrorHandler() {
    return GetInstance()->_error_handler;
}

Json::Value* GlobalsManager::GetConfigurationCards() {
    return GetInstance()->_configuration_cards;
}

dataCards* GlobalsManager::GetLegacyCards() {
    return GetInstance()->_legacy_cards;
}

BTFieldConstructor* GlobalsManager::GetBTFieldConstructor() {
    return GetInstance()->_field_constructor;
}

MAUSGeant4Manager* GlobalsManager::GetGeant4Manager() {
    return GetInstance()->_maus_geant4_manager;
}

MiceModule* GlobalsManager::GetMonteCarloMiceModules() {
    return GetInstance()->_mc_mods;
}

MiceModule* GlobalsManager::GetReconstructionMiceModules() {
    return GetInstance()->_recon_mods;
}
}  // namespace MAUS

