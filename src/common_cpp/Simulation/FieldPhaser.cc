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

#include <vector>
#include <set>

#include "src/common_cpp/Simulation/FieldPhaser.hh"

#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"
#include "src/legacy/Simulation/MICEPhysicsList.hh"
#include "src/legacy/Interface/VirtualHit.hh"

namespace MAUS {

FieldPhaser::FieldPhaser() : _phaserVirtualPlanes(),
                             _g4managerVirtualPlanes(NULL) {
}

FieldPhaser::~FieldPhaser() {
}

void FieldPhaser::SetUp() {
    MiceModule mod;
    _phaserVirtualPlanes.ConstructVirtualPlanes
                           (MICERun::getInstance()->btFieldConstructor, &mod);
    // set up
    std::vector<BTPhaser::FieldForPhasing*> cavities =
                           BTPhaser::GetInstance()->GetFieldsForPhasing();
    for (size_t i = 0; i < cavities.size(); ++i) {
        MakeVirtualPlanes(cavities[i]);
    }
    _g4managerVirtualPlanes =
                           MAUSGeant4Manager::GetInstance()->GetVirtualPlanes();
    MAUSGeant4Manager::GetInstance()->SetVirtualPlanes(&_phaserVirtualPlanes);
    MAUSGeant4Manager::GetInstance()->
                           GetPhysicsList()->BeginOfReferenceParticleAction();
}

void FieldPhaser::MakeVirtualPlanes(BTPhaser::FieldForPhasing* cavity) {
    VirtualPlane plane = VirtualPlane::BuildVirtualPlane(
        cavity->rotation, cavity->plane_position, cavity->radius, true, 0.,
        BTTracker::u, VirtualPlane::ignore, true
    );
    VirtualPlane* plane_ptr = new VirtualPlane(plane);
    // NOTE: VirtualPlaneManager now owns this memory
    _phaserVirtualPlanes.AddPlane(plane_ptr, NULL);
}

void FieldPhaser::SetPhases() {
    MAUSGeant4Manager* mgm = MAUSGeant4Manager::GetInstance();
    MAUSPrimaryGeneratorAction::PGParticle ref = mgm->GetReferenceParticle();
    SetUp();
    int n_attempts = 0;
    try {
        Squeak::mout(Squeak::info) << "Setting the phase " << std::flush;
        while (_phaserVirtualPlanes.GetPlanes().size() > 0 &&
               n_attempts < BTPhaser::GetInstance()->NumberOfCavities()*5) {
            ++n_attempts;
            Squeak::mout(Squeak::info) << "." << std::flush;
            Json::Value tracks = mgm->RunParticle(ref);
            Json::Value v_hits = JsonWrapper::GetProperty
                              (tracks, "virtual_hits", JsonWrapper::arrayValue);
            if (v_hits.size() == 0) break;
            std::set<int> phased_stations;
            for (int j = 0; j < v_hits.size(); ++j) {
                VirtualHit hit = _phaserVirtualPlanes.ReadHit(v_hits[j]);
                if (BTPhaser::GetInstance()->
                                     SetThePhase(hit.GetPos(), hit.GetTime())) {
                    ref = MAUSPrimaryGeneratorAction::PGParticle(hit);
                    phased_stations.insert(hit.GetStationNumber());
                }
            }
            _phaserVirtualPlanes.RemovePlanes(phased_stations);
        }
    }
    catch(...) {}
    Squeak::mout(Squeak::info) << "\nMade " << n_attempts << " attempts to phase "
          << BTPhaser::GetInstance()->NumberOfCavities() << " cavities with "
          << _phaserVirtualPlanes.GetPlanes().size() << " remaining" << std::endl;
    if (_phaserVirtualPlanes.GetPlanes().size() > 0) {
        TearDown();
        throw(Squeal(Squeal::recoverable, "Failed to phase cavities",
              "FieldPhaser::PhaseCavities"));
    }
    TearDown();
}

void FieldPhaser::TearDown() {
    MAUSGeant4Manager::GetInstance()->GetPhysicsList()->BeginOfRunAction();
    MAUSGeant4Manager::GetInstance()->SetVirtualPlanes(_g4managerVirtualPlanes);
}
}

