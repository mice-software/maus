// MAUS WARNING: THIS IS LEGACY CODE.
#include "KLSD.hh"
#include "Interface/dataCards.hh"
#include "Geant4/G4StepStatus.hh"
#include <cstring>
#include "Interface/MICEEvent.hh"
#include "Config/MiceModule.hh"
#include <iostream>

#include "src/common_cpp/DataStructure/KLChannelId.hh"

KLSD::KLSD( MiceModule* mod)
    : MAUSSD(mod), _hits(NULL)
{
}

void KLSD::Initialize(G4HCofThisEvent* HCE)
{

}

void KLSD::ClearHits() {
  if (_hits != NULL)
      delete _hits;
  _hits = new std::vector<MAUS::KLHit>();
}

G4bool KLSD::ProcessHits(G4Step* aStep, G4TouchableHistory* History)
{
  G4double edep = aStep->GetTotalEnergyDeposit();

  if( edep == 0. ) return false;

  MAUS::KLHit hit;
  MAUS::KLChannelId* channel_id = new MAUS::KLChannelId();

  channel_id->SetCell(_module->propertyInt( "Cell" ));
  hit.SetChannelId(channel_id);
  hit.SetEnergyDeposited(edep);

  G4Track* track = aStep->GetTrack();

  MAUS::ThreeVector pos(aStep->GetPreStepPoint()->GetPosition().x(),
                        aStep->GetPreStepPoint()->GetPosition().y(),
                        aStep->GetPreStepPoint()->GetPosition().z()); 
  hit.SetPosition(pos);
  MAUS::ThreeVector mom(aStep->GetPreStepPoint()->GetMomentum().x(),
                        aStep->GetPreStepPoint()->GetMomentum().y(),
                        aStep->GetPreStepPoint()->GetMomentum().z()); 
  hit.SetMomentum(mom);
  hit.SetTime(aStep->GetPreStepPoint()->GetGlobalTime());
  hit.SetCharge(track->GetDefinition()->GetPDGCharge());
  hit.SetParticleId(track->GetDefinition()->GetPDGEncoding());
  hit.SetEnergy(track->GetTotalEnergy());
  hit.SetTrackId(aStep->GetTrack()->GetTrackID());

  _hits->push_back(hit);
  return true;
}

void KLSD::EndOfEvent(G4HCofThisEvent* HCE)
{
 
}
