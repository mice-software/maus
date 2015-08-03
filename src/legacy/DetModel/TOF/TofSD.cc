// MAUS WARNING: THIS IS LEGACY CODE.
#include "TofSD.hh"
#include "Interface/dataCards.hh"
#include "Geant4/G4StepStatus.hh"
#include <cstring>
#include "Interface/MICEEvent.hh"
#include "Config/MiceModule.hh"
#include <iostream>

TofSD::TofSD( MiceModule* mod)
    : MAUSSD(mod), _hits(NULL)
{
}

void TofSD::Initialize(G4HCofThisEvent* HCE)
{

}

G4bool TofSD::ProcessHits(G4Step* aStep, G4TouchableHistory* History)
{
  G4double edep = aStep->GetTotalEnergyDeposit();

  if( edep == 0. ) return false;

  MAUS::TOFHit hit;
  MAUS::TOFChannelId* channel_id = new MAUS::TOFChannelId();

  channel_id->SetSlab(_module->propertyInt( "Slab" ));
  channel_id->SetPlane(_module->propertyInt( "Plane" ));
  channel_id->SetStation(_module->propertyInt( "Station" ));

  hit.SetChannelId(channel_id);
  hit.SetTrackId(aStep->GetTrack()->GetTrackID());
  hit.SetEnergyDeposited(aStep->GetTotalEnergyDeposit());
  hit.SetTime(aStep->GetPostStepPoint()->GetGlobalTime());
  hit.SetEnergy(aStep->GetTrack()->GetTotalEnergy());
  hit.SetParticleId(aStep->GetTrack()->GetDefinition()->GetPDGEncoding());
  hit.SetCharge(aStep->GetTrack()->GetDefinition()->GetPDGCharge());

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

void TofSD::ClearHits() {
  if (_hits != NULL) {
      delete _hits;
  }
  _hits = new std::vector<MAUS::TOFHit>();
}

void TofSD::TakeHits(MAUS::MCEvent* event) {
    if (event->GetTOFHits() == NULL)
        event->SetTOFHits(new std::vector<MAUS::Hit<MAUS::TOFChannelId> >());
    std::vector<MAUS::Hit<MAUS::TOFChannelId> >* ev_hits = event->GetTOFHits();
    ev_hits->insert(ev_hits->end(), _hits->begin(), _hits->end());
    delete _hits;
    _hits = new std::vector<MAUS::Hit<MAUS::TOFChannelId> >();
}


void TofSD::EndOfEvent(G4HCofThisEvent* HCE)
{
 
}
