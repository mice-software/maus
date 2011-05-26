/*
 * MiceMessenger.cc		2003-06-25
 *
 * Implementation of the MiceMessenger class.
 */

#include "Simulation/MiceMessenger.hh"
#include "Interface/dataCards.hh"
#include "G4UIdirectory.hh"
#include "G4ios.hh"

MiceMessenger::MiceMessenger() {
  miceDir = new G4UIdirectory("/mice/");
  miceDir->SetGuidance("MICE-specific commands");

  //  dataCardsMessenger = new DataCardsMessenger(&MyDataCards);

  G4cout << "/mice/ menu successfully created.\n";
}


MiceMessenger::~MiceMessenger() {
  //  delete dataCardsMessenger;

  delete miceDir;
}


void
MiceMessenger::SetNewValue(G4UIcommand* command,G4String newValue) {
  // do nothing
}

