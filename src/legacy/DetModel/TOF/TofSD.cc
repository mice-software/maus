// MAUS WARNING: THIS IS LEGACY CODE.
#include "TofSD.hh"
#include "Interface/dataCards.hh"
#include <G4StepStatus.hh>
#include <cstring>
#include "Interface/MICEEvent.hh"
#include "Config/MiceModule.hh"
#include <iostream>

TofSD::TofSD( MiceModule* mod)
    : MAUSSD(mod)
{
}

void TofSD::Initialize(G4HCofThisEvent* HCE)
{

}

G4bool TofSD::ProcessHits(G4Step* aStep, G4TouchableHistory* History)
{
  G4double edep = aStep->GetTotalEnergyDeposit();

  if( edep == 0. ) return false;
  if (!_hits.isMember("tof_hits")) {
    _hits["tof_hits"] = Json::Value(Json::arrayValue);
  }
  int hit_i = _hits["tof_hits"].size();
  _hits["tof_hits"].append(Json::Value());

  Json::Value channel_id;

  channel_id["slab"] =  _module->propertyInt( "Slab" ) ;
  channel_id["plane"] =  _module->propertyInt( "Plane" ) ;
  channel_id["station_number"] = _module->propertyInt( "Station" ) ;

  _hits["tof_hits"][hit_i]["channel_id"] = channel_id;

  _hits["tof_hits"][hit_i]["energy_deposited"] = 0.0;
  _hits["tof_hits"][hit_i]["path_length"] = 0.0;

  G4Track* track = aStep->GetTrack();

  Json::Value threeVectorValue;
  
  threeVectorValue["x"] = aStep->GetPreStepPoint()->GetPosition().x();
  threeVectorValue["y"] = aStep->GetPreStepPoint()->GetPosition().y();
  threeVectorValue["z"] = aStep->GetPreStepPoint()->GetPosition().z(); 
        
  _hits["tof_hits"][hit_i]["position"] = threeVectorValue;

  threeVectorValue["x"] = track->GetMomentum().x();
  threeVectorValue["y"] = track->GetMomentum().y();
  threeVectorValue["z"] = track->GetMomentum().z();

  _hits["tof_hits"][hit_i]["momentum"] = threeVectorValue;
  
  _hits["tof_hits"][hit_i]["time"] = aStep->GetPreStepPoint()->GetGlobalTime();
  
  _hits["tof_hits"][hit_i]["charge"] = track->GetDefinition()->GetPDGCharge();
  _hits["tof_hits"][hit_i]["mass"] = track->GetDefinition()->GetPDGMass();
  _hits["tof_hits"][hit_i]["particle_id"] = track->GetDefinition()->GetPDGEncoding();
  _hits["tof_hits"][hit_i]["energy"] = track->GetTotalEnergy();
  _hits["tof_hits"][hit_i]["track_id"] = aStep->GetTrack()->GetTrackID();

  return true;
}

void TofSD::EndOfEvent(G4HCofThisEvent* HCE)
{
 
}
