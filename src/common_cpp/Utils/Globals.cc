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

#include <sstream>

#include "Utils/Exception.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Utils/VersionNumber.hh"
#include "src/legacy/Interface/STLUtils.hh"
#include "Geant4/G4PVPlacement.hh"

namespace MAUS {

Globals* Globals::_process = NULL;

Globals* Globals::GetInstance() {
    if (_process != NULL) {
        return _process;
    } else {
        // watch out here, error handler not initialised(!)
        throw(Exceptions::Exception(Exceptions::recoverable,
                     std::string("Attempt to get Globals ")+
                          "instance when it has not been initialised",
                          "Globals::GetInstance()"));
    }
    return NULL; // appease gcc
}

Globals::~Globals() {
}

bool Globals::HasInstance() {
    return _process != NULL;
}

Globals::Globals()
  : _configuration_cards(NULL), _legacy_mice_run(NULL), _error_handler(NULL),
    _legacy_cards(NULL), _run_action_manager(NULL), _mc_mods(NULL),
    _recon_mods(NULL), _mc_field_constructor(NULL),
    _recon_field_constructor(NULL), _maus_geant4_manager(NULL),
    _scifi_geometry_helper(NULL) {
}

// in all the below I call GetInstance() to check that _process is initialised
RunActionManager* Globals::GetRunActionManager() {
    return GetInstance()->_run_action_manager;
}

CppErrorHandler* Globals::GetErrorHandler() {
    return GetInstance()->_error_handler;
}

Json::Value* Globals::GetConfigurationCards() {
    return GetInstance()->_configuration_cards;
}

dataCards* Globals::GetLegacyCards() {
    return GetInstance()->_legacy_cards;
}

BTFieldConstructor* Globals::GetMCFieldConstructor() {
    return GetInstance()->_mc_field_constructor;
}

BTFieldConstructor* Globals::GetReconFieldConstructor() {
    return GetInstance()->_recon_field_constructor;
}

MAUSGeant4Manager* Globals::GetGeant4Manager() {
    return GetInstance()->_maus_geant4_manager;
}

MiceModule* Globals::GetMonteCarloMiceModules() {
    return GetInstance()->_mc_mods;
}

MiceModule* Globals::GetReconstructionMiceModules() {
    return GetInstance()->_recon_mods;
}

GeometryNavigator* Globals::GetMCGeometryNavigator() {
    return GetInstance()->_mc_geometry_navigator;
}

SciFiGeometryHelper* Globals::GetSciFiGeometryHelper() {
    return GetInstance()->_scifi_geometry_helper;
}

std::string Globals::GetVersionNumberString() {
    std::string version = STLUtils::ToString(MAUS_VERSION_NUMBER_X)+"."+
                          STLUtils::ToString(MAUS_VERSION_NUMBER_Y)+"."+
                          STLUtils::ToString(MAUS_VERSION_NUMBER_Z);
    return version;
}

}  // namespace MAUS

