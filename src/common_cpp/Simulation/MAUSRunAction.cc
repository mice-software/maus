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

#include "Geant4/G4Run.hh"
#include "Geant4/G4Event.hh"
#include "Geant4/G4PrimaryVertex.hh"
#include "Geant4/G4PrimaryParticle.hh"
#include "Geant4/G4VVisManager.hh"
#include "Geant4/G4UImanager.hh"

#include "src/common_cpp/Simulation/MAUSRunAction.hh"

namespace MAUS {

void MAUSRunAction::BeginOfRunAction(const G4Run* aRun) {
  
  const std::vector<const G4Event*, std::allocator<const G4Event*> >* e = aRun->GetEventVector();
  std::cerr<<"event length"<<(*e).size()<<std::endl;
  for(size_t i=0; i<(*e).size(); i++){
      G4PrimaryVertex* pv = (*e)[i]->GetPrimaryVertex();
      G4PrimaryParticle* p = pv->GetPrimary(0);
      std::cerr<<"pol"<<p->GetPolarization()<<std::endl;
      
  }
  //->GetPrimaryVertex()->GetPrimary()->GetPolarization();
  //G4Event->GetPrimaryVertex();
  //G4PrimaryVertex->GetPrimary();
  //G4PrimaryParticle  polarization= ...  ->GetPolarization();
  //std::cerr<<polarization<<" "<<std::endl;
  
  // Visualization
  if (G4VVisManager::GetConcreteInstance()) {
     G4UImanager::GetUIpointer()->ApplyCommand("/vis/scene/notifyHandlers");
     
  }
}
}

