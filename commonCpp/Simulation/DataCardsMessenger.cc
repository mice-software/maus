/*
 * DataCardsMessenger.cc	2003-06-16
 *
 * Implementation of the DataCardsMessenger class.
 */

#include "DataCardsMessenger.hh"
#include "Interface/dataCards.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4ios.hh"

DataCardsMessenger::DataCardsMessenger(dataCards* cards) {
  myDataCards = cards;
  dataCardsDir = new G4UIdirectory("/mice/dataCards/");
  dataCardsDir->SetGuidance("dataCards commands");

  printDataCardCmd = new G4UIcmdWithAString("/mice/dataCards/printDataCard",this);
  printDataCardCmd->SetGuidance("print a value from the dataCards");
  printDataCardCmd->SetParameterName("identifier",false);
}


DataCardsMessenger::~DataCardsMessenger() {
  delete printDataCardCmd;

  delete dataCardsDir;
}


void
DataCardsMessenger::SetNewValue(G4UIcommand* command,G4String newValue) {
  std::string identifier = (const char*)newValue;

  if (command == printDataCardCmd) {
    G4cout << "Value of " << identifier << ":\n" << identifier << "\n";
    myDataCards->printValue(identifier);
    G4cout << "\n";
  }
}

