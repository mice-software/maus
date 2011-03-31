//-----------------------------------------------------------------------------
//
//     File :     TofSD.cc
//     Purpose :  MICE Time-of-Flight detector for the MICE proposal. Sensitive
//                detector definition.
//     Created :  14-Sep-2002  by Steve Kahn
//
//-----------------------------------------------------------------------------

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
  _hits.push_back(Json::Value());
  Json::Value channel_id;
  
  channel_id["type"] = "TOF";
  channel_id["slab"] =  _module->propertyInt( "Slab" ) ;
  channel_id["plane"] =  _module->propertyInt( "Plane" ) ;
  channel_id["station_number"] = _module->propertyInt( "Station" ) ;

  _hits[0]["channel_id"] = channel_id;

  _hits[0]["energy_deposited"] = 0.0;
  _hits[0]["path_length"] = 0.0;
  

  _isHit = false;
}

G4bool TofSD::ProcessHits(G4Step* aStep, G4TouchableHistory* History)
{
  G4double edep = aStep->GetTotalEnergyDeposit();
  if( edep == 0. )  return true;

  G4double length = aStep->GetStepLength();

  if(_hits[0].get("path_length", 0.0).asDouble() == 0) {
  	G4Track* track = aStep->GetTrack();

        //        _hits[0]["volume_name"] = _module->fullName();

        Json::Value threeVectorValue;

        threeVectorValue["x"] = aStep->GetPreStepPoint()->GetPosition().x();
        threeVectorValue["y"] = aStep->GetPreStepPoint()->GetPosition().y();
        threeVectorValue["z"] = aStep->GetPreStepPoint()->GetPosition().z(); 
        
        _hits[0]["position"] = threeVectorValue;

        threeVectorValue["x"] = track->GetMomentum().x();
        threeVectorValue["y"] = track->GetMomentum().y();
        threeVectorValue["z"] = track->GetMomentum().z();

        _hits[0]["momentum"] = threeVectorValue;

        _hits[0]["time"] = aStep->GetPreStepPoint()->GetGlobalTime();

        _hits[0]["charge"] = track->GetDefinition()->GetPDGCharge();
        _hits[0]["mass"] = track->GetDefinition()->GetPDGMass();
        _hits[0]["particle_id"] = track->GetDefinition()->GetPDGEncoding();
        _hits[0]["energy"] = track->GetTotalEnergy();
        _hits[0]["track_id"] = aStep->GetTrack()->GetTrackID();
  
  
    Json::StyledWriter writer;
    // Make a new JSON document for the configuration. Preserve original comments.
    for (int i=0; i< _hits.size(); i++){
      std::string outputConfig = writer.write( _hits[i]);
      std::cout<<"doc["<<i<<"]: "<<outputConfig<<std::endl;
    }
  }

  _hits[0]["energy_deposited"] = _hits[0].get("energy_deposited", 0).asDouble() + edep;
  _hits[0]["path_length"] = _hits[0].get("path_length", 0).asDouble() + length;

   return true;
}

void TofSD::EndOfEvent(G4HCofThisEvent* HCE)
{
  if( !(_hits[0]["energy_deposited"] == 0.) && _hits[0]["path_length"] != 0.){ 
    _isHit = true;
  }
 
}
