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
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <limits>
#include <queue>

#include "G4Event.hh"
#include "G4PrimaryVertex.hh"
#include "G4Track.hh"
#include "G4ios.hh"

#include "CLHEP/Random/Random.h"

#include "src/common/Simulation/MAUSPrimaryGeneratorAction.hh"

namespace MAUS {

/// Define static location of generator action
MAUSPrimaryGeneratorAction*
MAUSPrimaryGeneratorAction::self = 0;

MAUSPrimaryGeneratorAction::MAUSPrimaryGeneratorAction() {
  gun = new G4ParticleGun();
}

MAUSPrimaryGeneratorAction::~MAUSPrimaryGeneratorAction() {
  delete gun;
}

MAUSPrimaryGeneratorAction::PGParticle MAUSPrimaryGeneratorAction::Pop() {
  MAUSPrimaryGeneratorAction::PGParticle part = _part_q.front();
  _part_q.pop();
  return part;
}

void MAUSPrimaryGeneratorAction::GeneratePrimaries(G4Event* argEvent) {
  if (_part_q.size() == 0)
    throw(Squeal(Squeal::recoverable,
                 "No primary particles",
                 "MAUSPrimaryGeneratorAction::GeneratePrimaries"));
  PGParticle part = Pop();
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle = particleTable->FindParticle(part.pid);
  if ( particle == NULL )
    throw(Squeal(Squeal::recoverable,
                 "Particle pid not recognised",
                 "MAUSPrimaryGeneratorAction::GeneratePrimaries"));
  if ( part.energy < particle->GetPDGMass() )
    throw(Squeal(Squeal::recoverable,
                 "Particle total energy less than particle mass",
                 "MAUSPrimaryGeneratorAction::GeneratePrimaries"));

  gun->SetParticleDefinition(particle);

  // Get this class' variables to define next event.
  gun->SetParticlePosition(G4ThreeVector
                                    (part.x, part.y, part.z));
  gun->SetParticleTime(part.time);
  gun->SetParticleEnergy(part.energy-particle->GetPDGMass());
  gun->SetParticleMomentumDirection(G4ThreeVector
                                 (part.px, part.py, part.pz));
  gun->GeneratePrimaryVertex(argEvent);
  if ( part.seed < 0 || part.seed > std::numeric_limits<unsigned int>::max() ) {
    throw(Squeal(Squeal::recoverable,
                 "Random seed out of range",
                 "MAUSPrimaryGeneratorAction::GeneratePrimaries"));
  }

  CLHEP::HepRandom::setTheSeed(static_cast<unsigned int>(part.seed));
}

}  // ends MAUS namespace
