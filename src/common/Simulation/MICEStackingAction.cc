#include "MICEStackingAction.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4Track.hh"
#include "G4TrackStatus.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "Interface/dataCards.hh"
 

MICEStackingAction::MICEStackingAction():
ih_NTu_PionPlus(0), ih_NTu_MuonPlus(0), newPion(false),inclusiveTracking(0) 
{ 
}

MICEStackingAction::~MICEStackingAction()
{  }

G4ClassificationOfNewTrack 
MICEStackingAction::ClassifyNewTrack(const G4Track * aTrack)
{
  G4ThreeVector momentum = aTrack->GetMomentum();
  G4ThreeVector point = aTrack->GetPosition();
  //ME - unused float xvec[8];
  //ME - unused double pTot = 0.;
// 
// Kill all secondaries for now.. 
//  
    //ME - unused int hepId = aTrack->GetDefinition()->GetPDGEncoding();
//    if (abs(hepId) != 13) return fKill;
    return fWaiting;
}

void MICEStackingAction::NewStage()
{
}
    
void MICEStackingAction::PrepareNewEvent()
{ 
}


