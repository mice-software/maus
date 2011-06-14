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

#include "src/common_cpp/Simulation/FieldPhaser.hh"

#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"

#include "src/legacy/Interface/VirtualHit.hh"

namespace MAUS {

FieldPhaser::FieldPhaser() {
}

FieldPhaser::~FieldPhaser() {
}

void FieldPhaser::SetUp() {
  // clear the virtual planes and reinitialise without virtual planes
  // should be in MAUSG4Manager?
  MiceModule mod;
  delete VirtualPlaneManager::GetInstance();
  VirtualPlaneManager::ConstructVirtualPlanes(MICERun::getInstance()->btFieldConstructor, &mod);

  // set up 
  std::vector<BTPhaser::FieldForPhasing*> cavities = 
                                  BTPhaser::GetInstance()->GetFieldsForPhasing();
  for (size_t i = 0; i < cavities.size(); ++i) {
    MakeVirtualPlanes(cavities[i]);
  }
}

void FieldPhaser::MakeVirtualPlanes(BTPhaser::FieldForPhasing* cavity) {
    VirtualPlane plane = VirtualPlane::BuildVirtualPlane(
        cavity->rotation, cavity->plane_position, cavity->radius, true, 0., 
        BTTracker::u, VirtualPlane::ignore, true
    );
    VirtualPlane* plane_ptr = new VirtualPlane(plane);
  
    // NOTE: VirtualPlaneManager now owns this memory
    VirtualPlaneManager::AddPlane(plane_ptr, NULL);
}

void FieldPhaser::SetPhases() {
    SetUp();

    MAUSGeant4Manager* mgm = MAUSGeant4Manager::GetInstance();
    MAUSPrimaryGeneratorAction::PGParticle p;
    p.pz = 1.;
    p.energy = 226.;
    p.pid = -13;
    int n_cavities = BTPhaser::GetInstance()->NumberOfCavities();
    int n_attempts = 0;
    try {
      Squeak::mout(Squeak::info) << "Setting the phase " << std::flush;
      while (n_cavities > 0 &&
             n_attempts < BTPhaser::GetInstance()->NumberOfCavities()*5) {
          Squeak::mout(Squeak::info) << "." << std::flush;
          Json::Value v_hits = JsonWrapper::GetProperty(RunParticle(p), "virtual_hits", JsonWrapper::arrayValue);
          for (int j = 0; j < v_hits.size(); ++j) {
              VirtualHit hit = VirtualPlaneManager::GetInstance()->ReadHit(v_hits[j]);
              if (BTPhaser::GetInstance()->SetThePhase(hit.GetPos(), hit.GetTime()))
                  --n_cavities;
          }
          ++n_attempts;
      }
    }
    catch(...) {}
    Squeak::mout(Squeak::info) << "\nMade " << n_attempts << " attempts and phased "
            << BTPhaser::GetInstance()->NumberOfCavities()-n_cavities << " of " << BTPhaser::GetInstance()->NumberOfCavities() << " cavities" << std::endl;
    if (!BTPhaser::GetInstance()->IsPhaseSet())
        throw(Squeal(Squeal::recoverable,
              "Failed to phase cavities",
              "FieldPhaser::PhaseCavities"));

    TearDown();
}

// better in MAUSGeant4Manager
Json::Value FieldPhaser::RunParticle(MAUSPrimaryGeneratorAction::PGParticle p) {
    MAUSGeant4Manager::GetInstance()->GetPrimaryGenerator()->Push(p);
    MAUSGeant4Manager::GetInstance()->GetRunManager()->BeamOn(1);
    Json::Value tracks = MAUSGeant4Manager::GetInstance()->GetStepping()->GetTracks();
    MAUSGeant4Manager::GetInstance()->GetStepping()->SetTracks(Json::Value(Json::objectValue));
    VirtualPlaneManager::StartOfEvent();  // Should be in EventAction
    return tracks;
}

void FieldPhaser::TearDown() {
  delete VirtualPlaneManager::GetInstance();
  VirtualPlaneManager::ConstructVirtualPlanes(MICERun::getInstance()->btFieldConstructor, MICERun::getInstance()->miceModule);
}

}

