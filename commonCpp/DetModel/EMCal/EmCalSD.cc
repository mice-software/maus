/*
** Modified example 1 from the GEANT4 distribution to simulate the
** MICE scintillating fiber tracker for the MICE proposal
** Ed McKigney - August 21, 2002
*/
#include "EmCalSD.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

#include "Interface/dataCards.hh"
#include <fstream>

#include "Interface/MICEEvent.hh"
#include "Config/MiceModule.hh"

EmCalSD::EmCalSD( MICEEvent* event, MiceModule* mod ) : G4VSensitiveDetector( mod->fullName() )
{
  miceMemory.addNew( Memory::EmCalSD );
	
  _event = event;
  _module = mod;
}

EmCalSD::~EmCalSD()
{
  miceMemory.addDelete( Memory::EmCalSD );
}

void EmCalSD::Initialize(G4HCofThisEvent* HCE)
{
  if( HCE ) ;
}

G4bool EmCalSD::ProcessHits(G4Step* aStep,G4TouchableHistory* ROhist)
{
  G4Track* aTrack = aStep->GetTrack();
  G4double edep = aStep->GetTotalEnergyDeposit();
  G4double stepl = aTrack->GetStepLength();


  if ((edep == 0.) && (stepl == 0.) ) return false;
  if(edep == 0) return true; 

  G4VPhysicalVolume* physVol = aStep->GetTrack()->GetVolume();

  EmCalHit* hitEmCal = new EmCalHit();
  hitEmCal->SetLayerNumber(_module->propertyInt( "Layer" ));
  hitEmCal->SetCellNumber(_module->propertyInt( "Cell" ));
  hitEmCal->SetCopyNumber(physVol->GetCopyNo());
  hitEmCal->SetEdep     (edep);
  hitEmCal->SetPosition (aTrack->GetPosition() );
  hitEmCal->SetMomentum (aTrack->GetMomentum());
  hitEmCal->SetTime     (aTrack->GetGlobalTime());
  hitEmCal->SetEnergy   (aTrack->GetTotalEnergy());
  hitEmCal->SetTrackID  (aTrack->GetTrackID());
  hitEmCal->SetPID      (aTrack->GetDefinition()->GetPDGEncoding());
  hitEmCal->SetCharge   (aTrack->GetDefinition()->GetPDGCharge() );
  hitEmCal->SetMass     (aTrack->GetDefinition()->GetPDGMass() );

  _event->emCalHits.push_back( hitEmCal );
  return true;
}

void EmCalSD::EndOfEvent(G4HCofThisEvent* HCE)
{
  if( HCE ) ;
}
