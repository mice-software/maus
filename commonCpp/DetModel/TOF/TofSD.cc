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

TofSD::TofSD( MICEEvent* event, MiceModule* mod, bool dEdxCut )
 : G4VSensitiveDetector( mod->fullName()) 
{
  std::cout<<"special detector name:"<< mod->fullName() <<std::endl;
  miceMemory.addNew( Memory::TofSD ); 
  _dEdxCut = dEdxCut;
  _event   = event;
  _module  = mod;
  _event_document = "{}";
}

TofSD::~TofSD() 
{
  miceMemory.addDelete( Memory::TofSD );
}

void TofSD::Initialize(G4HCofThisEvent* HCE)
{
  if( HCE ) ;

  bool parsingSuccessful = _reader.parse( "{ }", _root );
  if ( !parsingSuccessful )
  {
    // report to the user the failure and their locations in the document.
    std::cout  << "Failed to parse configuration\n"
               << _reader.getFormatedErrorMessages();
    return;
  }

  _root[slab] =  _module->propertyInt( "Slab" ) ;
  _root["plane"] =  _module->propertyInt( "Plane" ) ;
  _root["station_number"] = _module->propertyInt( "Station" ) ;
  _root["energy_deposited"] = 0.0;
  _root["path_length"] = 0.0;
  _root["time"] = 0.0;
  
  _hit.SetStripNo( _module->propertyInt( "Slab" ) );
  _hit.SetPlaneNo( _module->propertyInt( "Plane" ) );
  _hit.SetStationNo( _module->propertyInt( "Station" ) );
  _hit.SetEdep( 0. );  
  _hit.SetPathLength( 0. );
  _hit.SetTime( 0. );
  //_hit.Print();

}

G4bool TofSD::ProcessHits(G4Step* aStep, G4TouchableHistory* History)
{
  std::cout<<"hi "<<aStep->GetTotalEnergyDeposit()<<std::endl;
  G4double edep = aStep->GetTotalEnergyDeposit();
  if( edep == 0. && _dEdxCut )  return true;

  G4double length = aStep->GetStepLength();

  if(_hit.GetPathLength() == 0)
  {
  	G4Track* track = aStep->GetTrack();

        _root["volume_name"] = _module->fullName();

  	_hit.SetVolumeName( _module->fullName() );
  	_hit.SetPosition( aStep->GetPreStepPoint()->GetPosition() );
  	_hit.SetTime( aStep->GetPreStepPoint()->GetGlobalTime() );
	_hit.setCharge( track->GetDefinition()->GetPDGCharge() );
  	_hit.SetMomentum( track->GetMomentum() );
  	_hit.SetEnergy( track->GetTotalEnergy() );
  	_hit.SetPID( track->GetDefinition()->GetPDGEncoding() );
  	_hit.SetMass( track->GetDefinition()->GetPDGMass() );
  	_hit.SetTrackID( track->GetTrackID() );
  }

  _root["energy_deposited"] = _root.get("energy_deposited").asDouble() + edep;
  _root["path_length"] = _root.get("path_length", 0).asDouble() + length;

  _hit.AddEdep( edep );
  _hit.AddPathLength( length );
	
   return true;
}

void TofSD::EndOfEvent(G4HCofThisEvent* HCE)
{
  if( !(_hit.GetEdep() == 0. && _dEdxCut) && _hit.GetPathLength() != 0.){ 
  	//_hit.Print();
 	_event->tofHits.push_back( new TofHit(_hit) );
  }

  Json::StyledWriter writer;
  // Make a new JSON document for the configuration. Preserve original comments.

  _event_document = writer.write( _root ); 
 
}
