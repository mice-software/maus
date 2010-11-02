/*
** Modified example 1 from the GEANT4 distribution to simulate the
** MICE scintillating fiber tracker for the MICE proposal
** Ed McKigney - August 21, 2002
*/
#include "EMRSD.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

#include "Interface/dataCards.hh"
#include <fstream>

#include "Interface/MICEEvent.hh"
#include "Config/MiceModule.hh"

EMRSD::EMRSD( MICEEvent* event, MiceModule* mod ) : G4VSensitiveDetector( mod->fullName() )
{
  miceMemory.addNew( Memory::EMRSD );
	
  _event = event;
  _module = mod;
}

EMRSD::~EMRSD()
{
  miceMemory.addDelete( Memory::EMRSD );
}

void EMRSD::Initialize(G4HCofThisEvent* HCE)
{
  if( HCE ) ;
}

G4bool EMRSD::ProcessHits(G4Step* aStep,G4TouchableHistory* ROhist)
{
  G4Track* aTrack = aStep->GetTrack();
  G4double edep = aStep->GetTotalEnergyDeposit();
  G4double stepl = aTrack->GetStepLength();


  if ((edep == 0.) && (stepl == 0.) ) return false;
  if(edep == 0) return true; 

  // ME - unused G4VPhysicalVolume* physVol = aStep->GetTrack()->GetVolume();

  EMRHit* hitEMR = new EMRHit();

  //ME - probably other stuff needed here!
  hitEMR->SetEdep     (edep);
  hitEMR->SetPosition (aTrack->GetPosition() );
  hitEMR->SetMomentum (aTrack->GetMomentum());
  hitEMR->SetTime     (aTrack->GetGlobalTime());
  hitEMR->SetEnergy   (aTrack->GetTotalEnergy());
  hitEMR->SetTrackID  (aTrack->GetTrackID());
  hitEMR->SetPID      (aTrack->GetDefinition()->GetPDGEncoding());
  hitEMR->SetCharge   (aTrack->GetDefinition()->GetPDGCharge() );
  hitEMR->SetMass     (aTrack->GetDefinition()->GetPDGMass() );

  _event->emrHits.push_back( hitEMR );

  return true;
}

void EMRSD::EndOfEvent(G4HCofThisEvent* HCE)
{
  if( HCE ) ;
}
