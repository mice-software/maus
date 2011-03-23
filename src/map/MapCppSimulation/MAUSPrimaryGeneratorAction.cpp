#include <queue>

#include <G4Event.hh>
#include <G4PrimaryVertex.hh>
#include <G4Track.hh>
#include <G4ios.hh>

#include <CLHEP/Random/Random.h>

#include "MAUSPrimaryGeneratorAction.h"

namespace MAUS {

/// Define static location of generator action
MAUSPrimaryGeneratorAction*
MAUSPrimaryGeneratorAction::self = 0;

MAUSPrimaryGeneratorAction::MAUSPrimaryGeneratorAction() {
  gun = new G4ParticleGun();
}

void MAUSPrimaryGeneratorAction::GeneratePrimaries(G4Event* argEvent) {
  if (_part_q.size() == 0)
    throw(Squeal(Squeal::recoverable,
                 "Run out of primary particles",
                 "MAUSPrimaryGeneratorAction::GeneratePrimaries"));
  PGParticle part = _part_q.front();
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle = particleTable->FindParticle(part.pid);
  gun->SetParticleDefinition(particle);

  // Get this class' variables to define next event.
  gun->SetParticlePosition(G4ThreeVector
                                    (part.x, part.y, part.z));
  gun->SetParticleEnergy(part.energy); //BUG? IIRC that is Kinetic Energy
  gun->SetParticleMomentumDirection(G4ThreeVector
                                 (part.px, part.py, part.pz));
  gun->GeneratePrimaryVertex(argEvent);
  CLHEP::HepRandom::setTheSeed(static_cast<unsigned int>(part.seed));
  _part_q.pop();
}

}  // ends MAUS namespace
