/* 
SciFiPointRec - specific code to reconstruct space points in the SciFi tracker
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

#include "SciFiPointRec.hh"

void SciFiPointRec( struct MICEEvent& theEvent, struct MICERun* run, int& num_trip, int& num_dup )
{
  if( theEvent.sciFiClusters.size() > 500 ) // massive event, don't bother trying to make points
     return;

  // first, triplets...

  num_trip = 0;

  bool made_trips[2][6] = { { false, false, false, false, false, false },
                            { false, false, false, false, false, false } };

  //std::sort(theEvent.sciFiClusters.begin(), theEvent.sciFiClusters.end());
  for( unsigned int a = 0; a < theEvent.sciFiClusters.size(); ++a ) {
    // std::cout << theEvent.sciFiClusters[a]->getNPE() << std::endl;
    if( theEvent.sciFiClusters[a]->getDoubletNo() == 0 && ! theEvent.sciFiClusters[a]->used() )
    {
      int tr = theEvent.sciFiClusters[a]->getTrackerNo();
      int stat = theEvent.sciFiClusters[a]->getStationNo();

      for( unsigned int b = 0; b < theEvent.sciFiClusters.size(); ++b )
        if( theEvent.sciFiClusters[b]->getTrackerNo() == tr &&
	    theEvent.sciFiClusters[b]->getStationNo() == stat &&
	    theEvent.sciFiClusters[b]->getDoubletNo() == 1 &&
	     ! theEvent.sciFiClusters[b]->used() )
	  for( unsigned int c = 0; c < theEvent.sciFiClusters.size(); ++c )
	    if( theEvent.sciFiClusters[c]->getTrackerNo() == tr &&
	        theEvent.sciFiClusters[c]->getStationNo() == stat &&
		theEvent.sciFiClusters[c]->getDoubletNo() == 2 &&
	         ! theEvent.sciFiClusters[c]->used() )
	    {
	      // write out info for pattern recognition
	      double t1 = theEvent.sciFiClusters[a]->getTime();
	      double t2 = theEvent.sciFiClusters[b]->getTime();
	      double t3 = theEvent.sciFiClusters[c]->getTime();;

              SciFiSpacePoint* point = new SciFiSpacePoint( run, theEvent.sciFiClusters[a], theEvent.sciFiClusters[b], theEvent.sciFiClusters[c] );
		int clustersok(1);
		for ( int cs=0; cs <point->getNumClusters(); ++cs) {
			if (point->getDoubletCluster(cs)->getNPE() < 5) {
				clustersok = 0;
			}
		}
              if( point->getChi2() < 750.0 && fabs((t1+t3)/2-t2) < 10.0 && clustersok)
	      {
		//point->keep();
                theEvent.sciFiSpacePoints.push_back( point );
                ++num_trip;
		//if (clustersok) {
		  point->keep();
		made_trips[tr][stat] = true;
		//		std::cout << theEvent.sciFiSpacePoints.size() << " space points" << std::endl;
		
		}
              else
	        delete point;
	    }
    }
  }

  // now duplets

  num_dup = 0;

  for( unsigned int a = 0; a < theEvent.sciFiClusters.size(); ++a )
    if(  ! theEvent.sciFiClusters[a]->used() )
    {
      int tr = theEvent.sciFiClusters[a]->getTrackerNo();
      int stat = theEvent.sciFiClusters[a]->getStationNo();
      int doub = theEvent.sciFiClusters[a]->getDoubletNo();

      if( ! made_trips[tr][stat] )
      for( unsigned int b = a + 1; b < theEvent.sciFiClusters.size(); ++b )
        if( theEvent.sciFiClusters[b]->getTrackerNo() == tr &&
            theEvent.sciFiClusters[b]->getStationNo() == stat &&
            theEvent.sciFiClusters[b]->getDoubletNo() != doub &&
            ! theEvent.sciFiClusters[b]->used() &&
            fabs( theEvent.sciFiClusters[a]->getTime() - theEvent.sciFiClusters[b]->getTime() ) < 10.0 )
        {
          SciFiSpacePoint* dup = new SciFiSpacePoint( run, theEvent.sciFiClusters[a], theEvent.sciFiClusters[b] );
          if( dup->good() )
          {
            dup->keep();
            theEvent.sciFiSpacePoints.push_back( dup );
            ++num_dup;
          }
          else
            delete dup;
        }
    }
}
