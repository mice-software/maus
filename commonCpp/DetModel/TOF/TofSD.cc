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
#include "Interface/TofHit.hh"
#include <iostream>

TofSD::TofSD( MiceModule* mod, bool dEdxCut )
    : MAUSSD(mod, dEdxCut)
{
}

void TofSD::Initialize(G4HCofThisEvent* HCE)
{
  _hit = Json::Value();
  Json::Value hw_id;
  
  hw_id["type"] = "TOF";
  hw_id["slab"] =  _module->propertyInt( "Slab" ) ;
  hw_id["plane"] =  _module->propertyInt( "Plane" ) ;
  hw_id["station_number"] = _module->propertyInt( "Station" ) ;

  _hit["hw_id"] = hw_id;
  _hit["energy_deposited"] = 0.0;
  _hit["path_length"] = 0.0;
  _hit["time"] = 0.0;
  
  _isHit = false;
}

G4bool TofSD::ProcessHits(G4Step* aStep, G4TouchableHistory* History)
{
  G4double edep = aStep->GetTotalEnergyDeposit();
  if( edep == 0. && _dEdxCut )  return true;

  G4double length = aStep->GetStepLength();

  if(_hit["path_length"] == 0) {
  	G4Track* track = aStep->GetTrack();

        _hit["volume_name"] = _module->fullName();
        G4ThreeVector threeVectorTemp = aStep->GetPreStepPoint()->GetPosition();
        Json::Value threeVectorValue;
        threeVectorValue["x"] = threeVectorTemp.x();
        threeVectorValue["y"] = threeVectorTemp.y();
        threeVectorValue["z"] = threeVectorTemp.z(); 
        
        _hit["position"]["x"] = threeVectorTemp.x();
        
        /*  	_hit.SetVolumeName( _module->fullName() );
  	_hit.SetPosition( aStep->GetPreStepPoint()->GetPosition() );
  	_hit.SetTime( aStep->GetPreStepPoint()->GetGlobalTime() );
	_hit.setCharge( track->GetDefinition()->GetPDGCharge() );
  	_hit.SetMomentum( track->GetMomentum() );
  	_hit.SetEnergy( track->GetTotalEnergy() );
  	_hit.SetPID( track->GetDefinition()->GetPDGEncoding() );
  	_hit.SetMass( track->GetDefinition()->GetPDGMass() );
  	_hit.SetTrackID( track->GetTrackID() );*/
  }

  _hit["energy_deposited"] = _hit.get("energy_deposited", 0).asDouble() + edep;
  _hit["path_length"] = _hit.get("path_length", 0).asDouble() + length;

   return true;
}

void TofSD::EndOfEvent(G4HCofThisEvent* HCE)
{
  if( !(_hit["energy_deposited"] == 0. && _dEdxCut) && _hit["path_length"] != 0.){ 
    _isHit = true;
  }
 
}
