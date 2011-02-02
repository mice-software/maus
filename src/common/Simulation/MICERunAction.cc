#include "MICERunAction.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VVisManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "Interface/dataCards.hh"
#include "Interface/RunHeader.hh"
#include "Interface/RunFooter.hh"
#include <iostream>
#include <fstream>
using namespace std;

MICERunAction::MICERunAction() { }

MICERunAction::~MICERunAction() { }

void MICERunAction::BeginOfRunAction(const G4Run* aRun)
{
  //Visualization
  if (G4VVisManager::GetConcreteInstance()) 
  {
     G4UImanager::GetUIpointer()->ApplyCommand("/vis/scene/notifyHandlers");    
  } 
}

void MICERunAction::EndOfRunAction(const G4Run* aRun)
{ 
}
