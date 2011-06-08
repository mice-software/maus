/* 
SciFiClusterRec - specific reconstruction code to find clusters of hits

M.Ellis - April 2005
*/

#include <iostream>
#include <fstream>
#include <algorithm>
#include "CLHEP/Vector/ThreeVector.h"
#include "SciFiRec.hh"
#include "Interface/SciFiDigit.hh"
#include "Calib/SciFiReconstructionParams.hh"
#include "SciFiTrack.hh"
#include "SciFiKalTrack.hh"
#include "SciFiSpacePoint.hh"
#include "SciFiDoubletCluster.hh"
#include "Interface/dataCards.hh"
using std::vector;

#include "SciFiClusterRec.hh"

std::vector<const MiceModule*> modules(0);

void SciFiClusterRec( MICEEvent& theEvent, MICERun* run )
{
  double minPE = run->DataCards->fetchValueDouble( "SciFiTrackRecNPECut" );

  // create a list of potential hits that have at least half the minimum required light

  std::vector<SciFiDigit*> seeds;

  for( unsigned int i = 0; i < theEvent.sciFiDigits.size(); ++i )
    if( theEvent.sciFiDigits[i]->getPhotoElectrons() > minPE / 2.0 &&
        ! theEvent.sciFiDigits[i]->isUsed() )
      seeds.push_back( theEvent.sciFiDigits[i] );

  // sort the vector of seeds by decreasing light yield so that we use the highest signal
  // hits first

  sort( seeds.begin(), seeds.end() );

  // now merge neighbours and check total light

  for( unsigned int i = 0; i < seeds.size(); ++i )
    if( ! seeds[i]->isUsed() )
    {
      SciFiDigit* seed = seeds[i];
      SciFiDigit* neigh = NULL;

      int tracker = seed->getTrackerNo();
      int station = seed->getStationNo();
      int plane = seed->getPlaneNo();
      int fibre = seed->getChanNo();

      for( unsigned int j = i + 1; j < seeds.size(); ++j )
        if( ! seeds[j]->isUsed() &&
	    seeds[j]->getTrackerNo() == tracker && seeds[j]->getStationNo() == station &&
	    seeds[j]->getPlaneNo() == plane && abs( seeds[j]->getChanNo() - fibre ) < 2 )
	  neigh = seeds[j];

      double pe = seed->getPhotoElectrons();

      if( neigh )
	pe += neigh->getPhotoElectrons();

      if( pe > minPE )
      {
	SciFiDoubletCluster* clust = new SciFiDoubletCluster( seed );
	if( neigh )
	  clust->addDigit( neigh );
	clust->keep();
	theEvent.sciFiClusters.push_back( clust );
      }
    }

  // lastly loop over clusters and determine the fibre position and orientation

  if( modules.size() == 0 )
    modules = run->miceModule->findModulesByPropertyString( "SensitiveDetector", "SciFi" );

  for( unsigned int i = 0; i < theEvent.sciFiClusters.size(); ++i )
  {
    SciFiDoubletCluster* clust = theEvent.sciFiClusters[i];

    //find the MiceModule for the plane that this cluster is in

    const MiceModule* mod = NULL;

    for( unsigned int j = 0; ! mod && j < modules.size(); ++j )
      if( modules[j]->propertyExists( "Tracker", "int" ) && modules[j]->propertyInt( "Tracker" ) == clust->getTrackerNo() &&
	  modules[j]->propertyExists( "Station", "int" ) && modules[j]->propertyInt( "Station" ) == clust->getStationNo() &&
	  modules[j]->propertyExists( "Plane", "int" ) && modules[j]->propertyInt( "Plane" ) == clust->getDoubletNo() )
	 mod = modules[j];

    Hep3Vector pos( 0, 0, 0 );
    Hep3Vector dir( 0, 1, 0 );

    if( mod )
    {
      Hep3Vector perp( -1., 0., 0 );
      
      dir *= mod->globalRotation();
      perp *= mod->globalRotation();

      double dist = mod->propertyDouble( "Pitch" ) * 7.0 / 2.0 * ( clust->getChanNoW() - mod->propertyDouble( "CentralFibre" ) );

      pos = dist * perp + mod->globalPosition();

      clust->setModule( mod );
    }

    clust->setPosition( pos );
    clust->setDirection( dir );
  }
}
