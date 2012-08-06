// MAUS WARNING: THIS IS LEGACY CODE.
/* CKOVSD.cc
*/
#include "DetModel/Ckov/CKOVSD.hh"

#include "Geant4/G4TransportationManager.hh"
#include "Geant4/G4FieldManager.hh"
#include "Geant4/G4Field.hh"

#include "Geant4/G4HCofThisEvent.hh"
#include "Geant4/G4Step.hh"
#include "Geant4/G4ThreeVector.hh"
#include "Geant4/G4SDManager.hh"
#include "Geant4/G4ios.hh"

#include "Interface/dataCards.hh"
#include <fstream>

#include "Interface/MICEEvent.hh"
#include "Config/MiceModule.hh"
#include "CLHEP/Units/PhysicalConstants.h"
#include <iostream>

CkovSD::CkovSD( MICEEvent* event, MiceModule* mod, bool dEdxCut ) : G4VSensitiveDetector( mod->fullName() )
{
  _dEdxCut = dEdxCut;
  _event   = event;
  _module  = mod;
}


void CkovSD::Initialize(G4HCofThisEvent* HCE)
{
  if( HCE ) ;

}

G4bool CkovSD::ProcessHits(G4Step* aStep,G4TouchableHistory* ROhist)
{
  G4double edep = aStep->GetTotalEnergyDeposit();

  //if( edep == 0. && _dEdxCut) return false;


  G4String particleName = aStep->GetTrack()->GetDefinition()->GetParticleName();

  if( particleName != "opticalphoton" ) return false;
  CkovHit* newHit = new CkovHit();


  newHit->SetTrackID( aStep->GetTrack()->GetTrackID() );

  newHit->SetVolumeName( aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() );

  newHit->SetEdep( edep );
  newHit->SetPos( aStep->GetPreStepPoint()->GetPosition() );
  newHit->SetMom( aStep->GetTrack()->GetMomentum() );
  newHit->SetTime( aStep->GetPreStepPoint()->GetGlobalTime() );
  newHit->SetEnergy( aStep->GetTrack()->GetTotalEnergy() );
  newHit->SetPID( aStep->GetTrack()->GetDefinition()->GetPDGEncoding() );
  newHit->SetCharge( aStep->GetTrack()->GetDefinition()->GetPDGCharge() );
  newHit->SetMass( aStep->GetTrack()->GetDefinition()->GetPDGMass() );
  newHit->SetPol( aStep->GetTrack()->GetDynamicParticle()->GetPolarization() );
  newHit->SetPmtNo( _module->propertyInt( "CkovPmtNum" ) );
  newHit->SetVesselNo( _module->mother()->propertyInt( "Plane" ) );

  Hep3Vector Direction = aStep->GetDeltaPosition().unit();
  Hep3Vector Position = newHit->GetPos();
  Hep3Vector Polarization = newHit->GetPol().unit();

  ProcessAngles( newHit, Position, Direction, Polarization );
  newHit->SetWavelenght( (c_light*h_Planck)/aStep->GetTrack()->GetTotalEnergy());
  //newHit->Print();
  //std::cout<<" Pol "<<newHit->GetPol()<<std::endl;
  _event->ckovHits.push_back( newHit );

  aStep->GetTrack()->SetTrackStatus(fStopAndKill);

  return true;
}

void CkovSD::EndOfEvent(G4HCofThisEvent* HCE)
{
}

void CkovSD::ProcessAngles( CkovHit* ckovHit, Hep3Vector HitPosition, Hep3Vector TrackDirection, Hep3Vector PhotonPol)
{
	Hep3Vector ModulePosition = _module->position();
	Hep3Vector norm = ( ModulePosition - HitPosition ).unit();
	double cos_Incid = norm.dot( TrackDirection );

   Hep3Vector PlaneOfIncidenceNorm = ( TrackDirection.cross( norm ) ).unit();
	double cos_Pol = PlaneOfIncidenceNorm.dot( PhotonPol );

	ckovHit->SetAngle( acos( cos_Incid ) );
	ckovHit->SetPAngle( acos( cos_Pol ) );
}
