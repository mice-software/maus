/*
** Modified example 1 from the GEANT4 distribution to simulate the
** MICE scintillating fiber tracker for the MICE proposal
** Ed McKigney - August 21, 2002

David Adey - Modified October 19, 2010

*/
#include "SciFiSD.hh"

#include "G4TransportationManager.hh"
#include "G4FieldManager.hh"
#include "G4Field.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

#include <fstream>

#include "Interface/MICEEvent.hh"
#include "Config/MiceModule.hh"

SciFiSD::SciFiSD( MiceModule* mod, bool dEdxCut ) : MAUSSD(mod, dEdxCut)
{

}


G4bool SciFiSD::ProcessHits(G4Step* aStep,G4TouchableHistory* ROhist)
{
  _hits.push_back(Json::Value());
  
  G4double edep = aStep->GetTotalEnergyDeposit();

  if( edep == 0. && _dEdxCut ) return false;
  else if( edep == 0. ) edep = 1.0 * MeV;	// fake energy deposit just so that we get some photons digitised out of this hit!

  // determine the fibre number based on the position in the tracker and the
  // MiceModule information about the fibre orientation and numbering scheme

  Hep3Vector pos = _module->globalPosition();
  Hep3Vector perp( -1., 0., 0. );
  double chanWidth = 1.4945; // effective channel width without overlap
  perp *= _module->globalRotation();

  Hep3Vector delta = aStep->GetPostStepPoint()->GetPosition() - pos;

  double dist = delta.x() * perp.x() + delta.y() * perp.y() + delta.z() * perp.z();
  double fibre = _module->propertyDouble( "CentralFibre" ) + dist * 2.0 / ( _module->propertyDouble( "Pitch" ) * 7.0 );

  double centFibre = _module->propertyDouble( "CentralFibre" );
  int  firstChan = (int)(fibre + 0.5);
  int secondChan(-1);
  double overlap = chanWidth - (0.1365/2.0);
   double underlap = (0.1365/2.0);
  int nChans(1);
  double distInChan = ((firstChan - centFibre)*chanWidth) - dist;

  if ((sqrt(distInChan*distInChan) > overlap) || (sqrt(distInChan*distInChan) < underlap)) { // distance in channel greater than section which does not overlap 
	nChans = 2;
	if (distInChan < 0) {
		secondChan = firstChan - 1;
	}
	else {
		secondChan = firstChan + 1;
	}
  }

  int hit_i = _hits.size();
  _hits.push_back(Json::Value());

  Json::Value channel_id;
  channel_id["type"] = "Tracker";
  _hits[hit_i]["track_id"] = aStep->GetTrack()->GetTrackID();
  channel_id["fiber_number"] = firstChan;

  //_hits[hit_i]["volume_name"] = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName();
  
  channel_id[ "tracker_number" ] = _module->propertyInt( "Tracker" );
  channel_id[ "station_number" ] = _module->propertyInt( "Station" );
  channel_id[ "plane_number" ] = _module->propertyInt( "Plane" );
  _hits[hit_i]["channel_id"] = channel_id;

  if (nChans == 2) {
    _hits[hit_i]["energy_deposited"] = edep/2.0;
  }
  else {
    _hits[hit_i]["energy_deposited"] = edep;
  }
  
  _hits[hit_i]["time"] = aStep->GetPostStepPoint()->GetGlobalTime();
  _hits[hit_i]["energy"] = aStep->GetTrack()->GetTotalEnergy();
  _hits[hit_i]["pid"] = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();
  _hits[hit_i]["charge"] =  aStep->GetTrack()->GetDefinition()->GetPDGCharge();
  _hits[hit_i]["mass"] = aStep->GetTrack()->GetDefinition()->GetPDGMass();
      
  if (nChans == 2) {
    _hits.push_back(_hits[hit_i]);
    _hits[_hits.size() - 1]["channel_id"]["fiber_number"] = secondChan;
  }

  _isHit = true;
  return true;
}

void SciFiSD::EndOfEvent(G4HCofThisEvent* HCE)
{
  // Do nothing
}
