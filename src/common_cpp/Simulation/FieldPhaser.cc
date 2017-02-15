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
#include <limits>
#include <utility>

#include "src/common_cpp/Simulation/FieldPhaser.hh"
#include "src/common_cpp/Utils/Globals.hh"

#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"
#include "src/common_cpp/Simulation/MAUSPhysicsList.hh"

#include "src/common_cpp/DataStructure/VirtualHit.hh"
#include "src/common_cpp/DataStructure/MCEvent.hh"

namespace MAUS {

FieldPhaser::FieldPhaser() : _phaserVirtualPlanes(NULL),
                             _g4managerVirtualPlanes(NULL) {
}

FieldPhaser::~FieldPhaser() {
    delete _phaserVirtualPlanes;
}

void FieldPhaser::SetUp() {
    MiceModule mod;
    // take a copy of existing virtual planes
    _g4managerVirtualPlanes = new VirtualPlaneManager(
                         *MAUSGeant4Manager::GetInstance()->GetVirtualPlanes());
    if (_phaserVirtualPlanes == NULL)  // should always be NULL by now!
        _phaserVirtualPlanes = new VirtualPlaneManager();
    // MAUSGeant4Manager now owns this memory
    MAUSGeant4Manager::GetInstance()->SetVirtualPlanes(_phaserVirtualPlanes);
    // Set up the virtual planes
    _phaserVirtualPlanes->ConstructVirtualPlanes(&mod);
    // set up
    std::vector<BTPhaser::FieldForPhasing*> cavities =
                           BTPhaser::GetInstance()->GetFieldsForPhasing();
    for (size_t i = 0; i < cavities.size(); ++i) {
        MakeVirtualPlanes(cavities[i]);
    }
    MAUSGeant4Manager::GetInstance()->
                           GetPhysicsList()->BeginOfReferenceParticleAction();
}

void FieldPhaser::MakeVirtualPlanes(BTPhaser::FieldForPhasing* cavity) {
    VirtualPlane plane = VirtualPlane::BuildVirtualPlane(
        cavity->rotation, cavity->plane_position, cavity->radius, true, 0.,
        BTTracker::u, VirtualPlane::ignore, true);
    VirtualPlane* plane_ptr = new VirtualPlane(plane);
    // NOTE: VirtualPlaneManager now owns this memory
    _phaserVirtualPlanes->AddPlane(plane_ptr, NULL);
}


void FieldPhaser::SetPhases() {
    MAUSGeant4Manager* mgm = MAUSGeant4Manager::GetInstance();
    MAUSPrimaryGeneratorAction::PGParticle ref = mgm->GetReferenceParticle();

    SetUp();
    int n_attempts = 0;
    try {
        Squeak::mout(Squeak::info) << "Setting the phase " << std::flush;
        while (_phaserVirtualPlanes->GetPlanes().size() > 0 &&
               n_attempts < BTPhaser::GetInstance()->NumberOfCavities()*5) {
            ++n_attempts;
            Squeak::mout(Squeak::info) << "." << std::flush;
            Squeak::mout(Squeak::debug) << "Firing "
                << "x:" << ref.x << " y: " << ref.y << " z: " << ref.z << " t: " << ref.time
                << "px:" << ref.px << " py: " << ref.py << " pz: " << ref.pz
                << " energy: " << ref.energy << std::endl;

            MCEvent* event = mgm->RunParticle(ref);
            std::vector<VirtualHit>* v_hits = event->GetVirtualHits();
            if (v_hits == NULL || v_hits->size() == 0)
                break;
            else
                ref = TryToPhase(v_hits);
            delete event;
       }
    }
    catch (...) {}
    Squeak::mout(Squeak::info) << "\nMade " << n_attempts
          << " attempts to phase "
          << BTPhaser::GetInstance()->NumberOfCavities() << " cavities with "
          << _phaserVirtualPlanes->GetPlanes().size() << " remaining"
          << std::endl;
    for (size_t i = 0; i < _phaserVirtualPlanes->GetPlanes().size(); ++i) {
        Squeak::mout(Squeak::debug) << "Failed to phase cavity at position " <<
                _phaserVirtualPlanes->GetPlanes()[i]->GetPosition() << std::endl;
    }
    if (_phaserVirtualPlanes->GetPlanes().size() > 0) {
        TearDown();
        throw(Exceptions::Exception(Exceptions::recoverable, "Failed to phase cavities",
              "FieldPhaser::PhaseCavities"));
    }
    TearDown();
}

MAUSPrimaryGeneratorAction::PGParticle FieldPhaser::TryToPhase
                                            (std::vector<MAUS::VirtualHit>* v_hits) {
    MAUSGeant4Manager* mgm = MAUSGeant4Manager::GetInstance();
    std::set<int> phased_stations;
    std::vector<std::pair<VirtualHit, bool> > phased_hits;

    MAUSPrimaryGeneratorAction::PGParticle ref = mgm->GetReferenceParticle();

    for (unsigned int j = 0; j < v_hits->size(); ++j) {
        VirtualHit hit = v_hits->at(j);
        CLHEP::Hep3Vector pos(hit.GetPosition().x(),
                              hit.GetPosition().y(),
                              hit.GetPosition().z());
        MAUS::ThreeVector mom = hit.GetMomentum();
        Squeak::mout(Squeak::debug) << "    station: " << hit.GetStationId() << " "
            << "x:" << pos.x() << " y: " << pos.y() << " z: " << pos.z() << " t: " << hit.GetTime()
            << " px:" << mom.x() << " py: " << mom.y() << " pz: " << mom.z() << std::endl;
        bool is_phased = BTPhaser::GetInstance()->SetThePhase(
                                                          pos,
                                                          hit.GetTime());
        std::pair<VirtualHit, bool> phase_pair(hit, is_phased);
        phased_hits.push_back(phase_pair);
        if (is_phased)
            phased_stations.insert(hit.GetStationId());
    }
    _phaserVirtualPlanes->RemovePlanes(phased_stations);

    // reference particle can skip cavities so we have to look for the first
    // unphased cavity and start the particle from the previous hit

    // no cavities phased
    if (!phased_hits[0].second) {
        return ref;
    }

    // some cavities phased
    for (unsigned int j = 1; j < phased_hits.size(); ++j) {
        if (!phased_hits[j].second) {
            return MAUSPrimaryGeneratorAction::PGParticle
                                                       (phased_hits[j-1].first);
        }
    }

    // all cavities phased
    return ref;
}

void FieldPhaser::TearDown() {
    MAUSGeant4Manager::GetInstance()->GetPhysicsList()->BeginOfRunAction();
    // This will delete _phaserVirtualPlanes and hand ownership of
    // _g4managerVirtualPlanes to MAUSGeant4Manager
    MAUSGeant4Manager::GetInstance()->SetVirtualPlanes(_g4managerVirtualPlanes);
    // none of this memory is valid so put it to NULL
    _g4managerVirtualPlanes = NULL;
    _phaserVirtualPlanes = NULL;
}
}

