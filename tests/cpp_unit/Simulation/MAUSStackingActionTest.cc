/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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
 * along with MAUS.  If not, see <http://  www.gnu.org/licenses/>.
 */

#include "gtest/gtest.h"

#include "Geant4/G4DynamicParticle.hh"
#include "Geant4/G4ParticleDefinition.hh"
#include "Geant4/G4ParticleTable.hh"
#include "Geant4/G4Track.hh"
#include "Geant4/G4ClassificationOfNewTrack.hh"

#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"
#include "src/common_cpp/Simulation/MAUSTrackingAction.hh"
#include "src/common_cpp/Simulation/MAUSStackingAction.hh"

namespace MAUS {
// config["default_keep_or_kill"] = true;
// config["keep_or_kill_particles"] = "{\"neutron\":False}";
// config["kinetic_energy_threshold"] = 0.1;

TEST(MAUSStackingActionTest, ConstructorAccessorsTest) {
    MAUSStackingAction msa;
    std::map<std::string, bool> keep_or_kill;
    keep_or_kill["neutron"] = false;
    EXPECT_TRUE(msa.GetDefaultKeepOrKill());
    EXPECT_EQ(msa.GetPdgPidKeepKill()["neutron"], false);
    EXPECT_EQ(msa.GetKineticEnergyThreshold(), 0.1);
}

TEST(MAUSStackingActionTest, AccessorsMutatorsTest) {
    MAUSStackingAction msa;
    std::map<std::string, bool> keep_or_kill;
    keep_or_kill["proton"] = false;
    msa.SetDefaultKeepOrKill(false);
    EXPECT_FALSE(msa.GetDefaultKeepOrKill());

    msa.SetPdgPidKeepKill(keep_or_kill);
    EXPECT_EQ(msa.GetPdgPidKeepKill()["proton"], false);

    msa.SetKineticEnergyThreshold(2);
    EXPECT_EQ(msa.GetKineticEnergyThreshold(), 2);
}

// make a track with 1 MeV KE
G4Track* MakeTrack() {
    G4ParticleDefinition* pd =
                         G4ParticleTable::GetParticleTable()->FindParticle(-13);
    G4DynamicParticle* dyn =
                       new G4DynamicParticle(pd, G4ThreeVector(0., 0., 1.), 1.);
    G4Track* track = new G4Track(dyn, 0., G4ThreeVector(0., 0., 0.));
    track->SetTrackID(0);
    MAUSTrackingAction* ta = MAUSGeant4Manager::GetInstance()->GetTracking();
    ta->SetTracks(new std::vector<Track>());
    ta->PreUserTrackingAction(track);
    return track;
}

TEST(MAUSStackingActionTest, ClassifyNewTrackTest) {
    MAUSStackingAction msa;

    // pid is mu+; ke > 0.1 - not in keep_or_kill - default should be to run
    G4Track* track_1 = MakeTrack();

    msa.SetDefaultKeepOrKill(true);
    EXPECT_EQ(msa.ClassifyNewTrack(track_1), fWaiting);
    msa.SetDefaultKeepOrKill(false);
    EXPECT_EQ(msa.ClassifyNewTrack(track_1), fKill);
    std::map<std::string, bool> _keepers = msa.GetPdgPidKeepKill();
    _keepers["mu+"] = true;
    msa.SetPdgPidKeepKill(_keepers);
    EXPECT_EQ(msa.ClassifyNewTrack(track_1), fWaiting);
    _keepers["mu+"] = false;
    msa.SetPdgPidKeepKill(_keepers);
    msa.SetDefaultKeepOrKill(true);
    EXPECT_EQ(msa.ClassifyNewTrack(track_1), fKill);
    _keepers["mu+"] = true;
    msa.SetPdgPidKeepKill(_keepers);
    msa.SetKineticEnergyThreshold(1.1);
    EXPECT_EQ(msa.ClassifyNewTrack(track_1), fKill);
    msa.SetKineticEnergyThreshold(0.9);
    EXPECT_EQ(msa.ClassifyNewTrack(track_1), fWaiting);
}
}
