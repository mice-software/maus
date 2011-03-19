#include "MAUSPrimaryGeneratorAction.h"

#include <G4Event.hh>
#include <G4PrimaryVertex.hh>
#include <G4Track.hh>
#include <G4ios.hh>

namespace MAUS {

/// Define static location of generator action
MAUSPrimaryGeneratorAction*
MAUSPrimaryGeneratorAction::self = 0;

MAUSPrimaryGeneratorAction::MAUSPrimaryGeneratorAction() {
  if (self == 0) {
    self = this;
  } else {
    G4Exception(
        "Error, more than one MAUSPrimaryGeneratorAction instantiated.\n"
        "Sorry, but this is a no-no because MAUSSteppingAction relies on\n"
        "MAUSPrimaryGeneratorAction::GetTheMAUSPrimaryGeneratorAction().\n"
        "This is yucky, I know -- please rewrite MAUSSteppingAction AND\n"
        "all main() programs so that constructor accepts a pointer to\n"
        "the MAUSPrimaryGeneratorAction you really want them to use.");
  }

  gun = new G4ParticleGun();
  updated = false;

  //  Initialize stored values to zero
  pid    = 0.0;
  X      = 0.0;
  Y      = 0.0;
  Z      = 0.0;
  Energy = 0.0;
  Pxu    = 0.0;
  Pyu    = 0.0;
  Pzu    = 0.0;
}

void MAUSPrimaryGeneratorAction::GeneratePrimaries(G4Event* argEvent) {
  if (updated == false) {
    std::cout << "WARNING MAUSPrimaryGeneratorAction: not updated" << std::endl;
  }

  G4ParticleTable*      particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle      = particleTable->FindParticle(pid);
  gun->SetParticleDefinition(particle);

  // Get this class' variables to define next event.
  gun->SetParticlePosition(G4ThreeVector(X, Y, Z));
  gun->SetParticleEnergy(Energy);
  gun->SetParticleMomentumDirection(G4ThreeVector(Pxu, Pyu, Pzu));

  gun->GeneratePrimaryVertex(argEvent);

  updated = false;
}

}  // ends MAUS namespace
