// MAUS WARNING: THIS IS LEGACY CODE.
/*
** Modified example 1 from the GEANT4 distribution to simulate the
** MICE scintillating fiber tracker for the MICE proposal
** Ed McKigney - August 21, 2002
*/
#include "DetModel/KL/KLSD.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

#include "Interface/dataCards.hh"
#include <fstream>

#include "Interface/MICEEvent.hh"
#include "Config/MiceModule.hh"

KLSD::KLSD( MICEEvent* event, MiceModule* mod ) : G4VSensitiveDetector( mod->fullName() )
{
  _event = event;
  _module = mod;
}

void KLSD::Initialize(G4HCofThisEvent* HCE)
{
  if( HCE ) ;
}

G4bool KLSD::ProcessHits(G4Step* aStep,G4TouchableHistory* ROhist)
{
  G4Track* aTrack = aStep->GetTrack();
  G4double edep = aStep->GetTotalEnergyDeposit();
  G4double stepl = aTrack->GetStepLength();


  if ((edep == 0.) && (stepl == 0.) ) return false;
  if(edep == 0) return true; 

  G4VPhysicalVolume* physVol = aStep->GetTrack()->GetVolume();

  KLHit* hitKL = new KLHit();

  hitKL->SetCellNumber(_module->propertyInt( "Cell" ));
  hitKL->SetCopyNumber(physVol->GetCopyNo());

  //ME - probably other stuff needed here!
  hitKL->SetEdep     (edep);
  hitKL->SetPosition (aTrack->GetPosition() );
  hitKL->SetMomentum (aTrack->GetMomentum());
  hitKL->SetTime     (aTrack->GetGlobalTime());
  hitKL->SetEnergy   (aTrack->GetTotalEnergy());
  hitKL->SetTrackID  (aTrack->GetTrackID());
  hitKL->SetPID      (aTrack->GetDefinition()->GetPDGEncoding());
  hitKL->SetCharge   (aTrack->GetDefinition()->GetPDGCharge() );
  hitKL->SetMass     (aTrack->GetDefinition()->GetPDGMass() );

  _event->klHits.push_back( hitKL );
  return true;
}

void KLSD::EndOfEvent(G4HCofThisEvent* HCE)
{
  if( HCE ) ;
}
