#include <limits>
#include <queue>

#include <G4Event.hh>
#include <G4PrimaryVertex.hh>
#include <G4Track.hh>
#include <G4ios.hh>

#include <CLHEP/Random/Random.h>

#include "MAUSPrimaryGeneratorAction.hh"

namespace MAUS {

/// Define static location of generator action
MAUSPrimaryGeneratorAction*
MAUSPrimaryGeneratorAction::self = 0;

MAUSPrimaryGeneratorAction::MAUSPrimaryGeneratorAction() {
  gun = new G4ParticleGun();
}

MAUSPrimaryGeneratorAction::PGParticle MAUSPrimaryGeneratorAction::Pop() {
  MAUSPrimaryGeneratorAction::PGParticle part = _part_q.front();
  _part_q.pop();
  return part;
}

void MAUSPrimaryGeneratorAction::GeneratePrimaries(G4Event* argEvent) {
  if (_part_q.size() == 0)
    throw(Squeal(Squeal::recoverable,
                 "Run out of primary particles",
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
                 "Run out of primary particles",
                 "MAUSPrimaryGeneratorAction::GeneratePrimaries"));

  gun->SetParticleDefinition(particle);

  // Get this class' variables to define next event.
  gun->SetParticlePosition(G4ThreeVector
                                    (part.x, part.y, part.z));
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
