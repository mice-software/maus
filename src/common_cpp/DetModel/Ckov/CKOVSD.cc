// MAUS WARNING: THIS IS LEGACY CODE.
/* CKOVSD.cc
*/
#include "CKOVSD.hh"

#include <cstring>
#include <iostream>

#include "Geant4/G4StepStatus.hh"

#include "Interface/dataCards.hh"
#include "Interface/MICEEvent.hh"
#include "Config/MiceModule.hh"

CkovSD::CkovSD(MiceModule* md) : 
    MAUSSD(md), _hits(NULL)
{

}


void CkovSD::Initialize(G4HCofThisEvent* HCE)
{

}

G4bool CkovSD::ProcessHits(G4Step* aStep, G4TouchableHistory* History)
{
  G4double edep = aStep->GetTotalEnergyDeposit();

  if(edep == 0.) return false;

  G4String particleName = aStep->GetTrack()->GetDefinition()->GetParticleName();
  
  /* If the PMTs are defined as the SD, use this, or, use
   * the uncommented block.
  if( particleName != "opticalphoton" ) return false;
  */

  MAUS::CkovHit hit;
  MAUS::CkovChannelId* channel_id = new MAUS::CkovChannelId();

  /*
   * If the PMTs are defined as the SD, use this, or, use
   * the uncommented block.
  channel_id->SetStation(_module->propertyInt("Station"));
  //channel_id->SetPMT(_module->propertyInt("pmt"));
  hit.SetTime(aStep->GetPostStepPoint()->GetGlobalTime());

  */
  channel_id->SetStation(_module->propertyInt("CkovStation"));
  //channel_id->SetStation(0);

  hit.SetChannelId(channel_id);
  hit.SetTrackId(aStep->GetTrack()->GetTrackID());
  hit.SetEnergyDeposited(aStep->GetTotalEnergyDeposit());
  hit.SetTime(aStep->GetPostStepPoint()->GetGlobalTime());
  hit.SetEnergy(aStep->GetTrack()->GetTotalEnergy());
  hit.SetParticleId(aStep->GetTrack()->GetDefinition()->GetPDGEncoding());
  hit.SetCharge(aStep->GetTrack()->GetDefinition()->GetPDGCharge());
  hit.SetMass(aStep->GetTrack()->GetDefinition()->GetPDGMass());

  hit.SetPosition(MAUS::ThreeVector(
    aStep->GetPostStepPoint()->GetPosition().x(),
    aStep->GetPostStepPoint()->GetPosition().y(),
    aStep->GetPostStepPoint()->GetPosition().z()
  ));
  hit.SetMomentum(MAUS::ThreeVector(
    aStep->GetPostStepPoint()->GetMomentum().x(),
    aStep->GetPostStepPoint()->GetMomentum().y(),
    aStep->GetPostStepPoint()->GetMomentum().z()
  ));
  
  _hits->push_back(hit);
  return true;
}

void CkovSD::ClearHits(){
  if (_hits != NULL){
    delete _hits;
  }
  _hits = new std::vector<MAUS::CkovHit>();
}

void CkovSD::TakeHits(MAUS::MCEvent* event) {
  if (event->GetCkovHits() == NULL) {
    event->SetCkovHits(new std::vector<MAUS::Hit<MAUS::CkovChannelId> >());
  }
  std::vector<MAUS::Hit<MAUS::CkovChannelId> >* ev_hits = event->GetCkovHits();
  ev_hits->insert(ev_hits->end(), _hits->begin(), _hits->end());
  delete _hits;
  _hits = new std::vector<MAUS::Hit<MAUS::CkovChannelId> >();
}
/*
  // This is for sending the tracks to each PMT.

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
*/

void CkovSD::EndOfEvent(G4HCofThisEvent* HCE)
{
}
/*
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
*/
