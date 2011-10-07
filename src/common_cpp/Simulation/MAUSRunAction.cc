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

#include "G4Run.hh"
#include "G4VVisManager.hh"
#include "G4UImanager.hh"

#include "src/common_cpp/Simulation/MAUSRunAction.hh"

namespace MAUS {

void MAUSRunAction::BeginOfRunAction(const G4Run* aRun)
{
  //Visualization
  if (G4VVisManager::GetConcreteInstance()) 
  {
     G4UImanager::GetUIpointer()->ApplyCommand("/vis/scene/notifyHandlers");    
  } 
}

}

