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

#include "Geant4/G4SDManager.hh"

#include "DetModel/MAUSSD.hh"
#include "Interface/STLUtils.hh"

namespace MAUS {
int MAUSSD::_uniqueID(0);
std::map<std::string, bool> MAUSSD::_hasVolumes;

MAUSSD::MAUSSD(MiceModule* mod)
     : G4VSensitiveDetector(namePrefix()+mod->fullName()) {
  _hasVolumes[namePrefix()] = true;
  _module = mod;
  G4SDManager::GetSDMpointer()->AddNewDetector(this);
}

std::string MAUSSD::namePrefix() {
  return "/"+STLUtils::ToString(_uniqueID)+"/";
}

void MAUSSD::ResetSDs() {
  if (_hasVolumes[namePrefix()])
      G4SDManager::GetSDMpointer()->Activate(namePrefix(), false);
  _uniqueID++;
  _hasVolumes[namePrefix()] = false;
}
}  // ends MAUS namespace
