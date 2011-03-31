/*
** Modified example 1 from the GEANT4 distribution to simulate the
** MICE scintillating fiber tracker for the MICE proposal
** Ed McKigney - August 21, 2002
*/
#include "EMRSD.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

#include "Interface/dataCards.hh"
#include <fstream>

#include "Interface/MICEEvent.hh"
#include "Interface/dataCards.hh"
#include "Config/MiceModule.hh"
#include <G4StepStatus.hh>
#include <cstring>
#include <iostream>

EMRSD::EMRSD( MiceModule* mod) : MAUSSD( mod )
{
}

EMRSD::~EMRSD()
{
}

void EMRSD::Initialize(G4HCofThisEvent* HCE)
{
/*
	//_hits.push_back(Json::Value());
	Json::Value channel_id;

	channel_id[ "type" ] = "EMR";
	channel_id[ "layer" ] =  _module->propertyInt( "Layer" ) ;
	channel_id[ "bar" ] =  _module->propertyInt( "Cell" ) ;
	_hits[ 0 ][ "channel_id" ] = channel_id;
    _hits[ 0 ][ "energy_deposited" ] = 0.0;
	_hits[ 0 ][ "path_length" ] = 0.0;
	_isHit = false;
*/
}

G4bool EMRSD::ProcessHits(G4Step* aStep,G4TouchableHistory* ROhist)
{
	std::cout << _module->fullName() << std::endl;
	/*
	Json::Value hit;
    G4double length = aStep->GetStepLength();

	if( _hits[ 0 ].get( "path_length", 0.0 ).asDouble() == 0 ) {
		G4Track* track = aStep->GetTrack();
	        //        _hits[0]["volume_name"] = _module->fullName();
        Json::Value threeVectorValue;

        //Set hit position
        threeVectorValue[ "x" ] = aStep->GetPreStepPoint()->GetPosition().x();
        threeVectorValue[ "y" ] = aStep->GetPreStepPoint()->GetPosition().y();
        threeVectorValue[ "z" ] = aStep->GetPreStepPoint()->GetPosition().z();
        _hits[ 0 ][ "position" ] = threeVectorValue;

        //Set hit momentum
	    threeVectorValue[ "x" ] = track->GetMomentum().x();
	    threeVectorValue[ "y" ] = track->GetMomentum().y();
	    threeVectorValue[ "z" ] = track->GetMomentum().z();
        _hits[ 0 ][ "momentum"] = threeVectorValue;

        //Set hit time, charge, mass, pdg code, total energy, track id and deposited energy
        _hits[ 0 ][ "time" ] = aStep->GetPreStepPoint()->GetGlobalTime();
        _hits[ 0 ][ "charge" ] = track->GetDefinition()->GetPDGCharge();
	    _hits[ 0 ][ "mass" ] = track->GetDefinition()->GetPDGMass();
	    _hits[ 0 ][ "particle_id" ] = track->GetDefinition()->GetPDGEncoding();
	    _hits[ 0 ][ "energy" ] = track->GetTotalEnergy();
	    _hits[ 0 ][ "track_id" ] = aStep->GetTrack()->GetTrackID();
	    _hits[ 0 ][ "energy_deposited" ] = aStep->GetTotalEnergyDeposit();

	    Json::StyledWriter writer;
	    // Make a new JSON document for the configuration. Preserve original comments.
	    for (int i=0; i< _hits.size(); ++i ){
	      std::string outputConfig = writer.write( _hits[ i ] );
	      std::cout << "doc[" << i << "]: "<< outputConfig << std::endl;
	    }
	  }

	  _hits[ 0 ][ "energy_deposited" ] = _hits[0].get("energy_deposited", 0 ).asDouble() + Edep;
	  _hits[ 0 ][ "path_length" ] = _hits[ 0 ].get( "path_length", 0 ).asDouble() + length;
	  //ME - probably other stuff needed here!
		_hits.push_back( hit );

	  return true;
*/
}

void EMRSD::EndOfEvent( G4HCofThisEvent* HCE )
{
	/*
	if( !( ( _hits[ 0 ][ "energy_deposited" ] == 0. ) ) && _hits[ 0 ][ "path_length" ] != 0. ){
	    _isHit = true;
	}
	*/
}
