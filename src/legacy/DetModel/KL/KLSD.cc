// MAUS WARNING: THIS IS LEGACY CODE.
#include "KLSD.hh"
#include "Interface/dataCards.hh"
#include "Geant4/G4StepStatus.hh"
#include <cstring>
#include "Interface/MICEEvent.hh"
#include "Config/MiceModule.hh"
#include <iostream>

KLSD::KLSD( MiceModule* mod)
    : MAUSSD(mod)
{
}

void KLSD::Initialize(G4HCofThisEvent* HCE)
{

}

G4bool KLSD::ProcessHits(G4Step* aStep, G4TouchableHistory* History)
{
  G4double edep = aStep->GetTotalEnergyDeposit();

  if( edep == 0. ) return false;
  if (!_hits.isMember("kl_hits")) {
    _hits["kl_hits"] = Json::Value(Json::arrayValue);
  }
  int hit_i = _hits["kl_hits"].size();
  _hits["kl_hits"].append(Json::Value());

  Json::Value channel_id;

  channel_id["cell"] =  _module->propertyInt( "Cell" ) ;

  _hits["kl_hits"][hit_i]["channel_id"] = channel_id;

  _hits["kl_hits"][hit_i]["energy_deposited"] = 0.0;

  G4Track* track = aStep->GetTrack();

  Json::Value threeVectorValue;
  
  threeVectorValue["x"] = aStep->GetPreStepPoint()->GetPosition().x();
  threeVectorValue["y"] = aStep->GetPreStepPoint()->GetPosition().y();
  threeVectorValue["z"] = aStep->GetPreStepPoint()->GetPosition().z(); 
        
  _hits["kl_hits"][hit_i]["position"] = threeVectorValue;

  threeVectorValue["x"] = track->GetMomentum().x();
  threeVectorValue["y"] = track->GetMomentum().y();
  threeVectorValue["z"] = track->GetMomentum().z();

  _hits["kl_hits"][hit_i]["momentum"] = threeVectorValue;
  
  _hits["kl_hits"][hit_i]["time"] = aStep->GetPreStepPoint()->GetGlobalTime();
  
  _hits["kl_hits"][hit_i]["charge"] = track->GetDefinition()->GetPDGCharge();
  _hits["kl_hits"][hit_i]["particle_id"] = track->GetDefinition()->GetPDGEncoding();
  _hits["kl_hits"][hit_i]["energy"] = track->GetTotalEnergy();
  _hits["kl_hits"][hit_i]["track_id"] = aStep->GetTrack()->GetTrackID();
  _hits["kl_hits"][hit_i]["energy_deposited"] = aStep->GetTotalEnergyDeposit();

  return true;
}

void KLSD::EndOfEvent(G4HCofThisEvent* HCE)
{
 
}
