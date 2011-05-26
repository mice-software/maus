/* 
** Written for the MICE proposal simulation
** Ed McKigney - Aug 21 2002
*/

#include <iostream>
#include <fstream>
#include <algorithm>
#include "CLHEP/Vector/ThreeVector.h"
#include "Recon/SciFi/SciFiRec.hh"
#include "Interface/SciFiDigit.hh"
#include "Recon/SciFi/SciFiTrack.hh"
#include "Recon/SciFi/SciFiKalTrack.hh"
#include "Recon/SciFi/SciFiSpacePoint.hh"
#include "Recon/SciFi/SciFiDoubletCluster.hh"
#include "Interface/dataCards.hh"
#include "Interface/MICEEvent.hh"
#include "Interface/MICERun.hh"
#include "Recon/SciFi/SciFiDigitRec.hh"
#include "Recon/SciFi/SciFiCMN.hh"
#include "Recon/SciFi/SciFiClusterRec.hh"
#include "Recon/SciFi/SciFiPointRec.hh"
#include "Recon/SciFi/SciFiHelixTrackRec.hh"
#include "Recon/SciFi/SciFiStraightTrackRec.hh"
#include "Interface/ZustandVektor.hh"

#include "Interface/VlpcHit.hh"

SciFiRec::SciFiRec( MICEEvent& e, MICERun* run ) : theEvent( e )
{
  active = 0;

  m_run = run;
  m_cable = m_run->vlpcCable;
  m_calib = m_run->vlpcCalib;
} 

void SciFiRec::Process( )
{
  int rec_level = m_run->DataCards->fetchValueInt( "RecLevel" );

  if( m_calib && m_cable && theEvent.sciFiDigits.size() == 0 )
  {
    //SciFiCMN( theEvent, m_cable, m_calib );
    SciFiDigitRec( theEvent, m_calib, m_cable );
  }

  // Loop over fiber hits, constructing the clusters...

  if( rec_level >= 1 && theEvent.sciFiClusters.size() == 0 )
    SciFiClusterRec( theEvent, m_run );

  // Now loop over each of the stations of each tracker and make space points...

  int num_trip = 0;
  int num_dup = 0;

  if( rec_level >= 2 && theEvent.sciFiSpacePoints.size() == 0 )
    SciFiPointRec( theEvent, m_run, num_trip, num_dup );

  // Now try to make SciFiKalTracks directly from Space Points

  if( rec_level >= 3 && theEvent.sciFiKalTracks.size() == 0 )
  {
    //if( fabs( recPars.getBField() ) > 1e-4 )			// find tracks in a solenoidal field  */
   //   SciFiHelixTrackRec( theEvent, m_run );
   // else
      SciFiStraightTrackRec( theEvent, m_run );
  }

  // Finally, look for ZustandVektors created from the Monte Carlo hits in the
  // tracker reference planes, and make a corresponding ZustandVektor from the track
  // extrapolation


  if( rec_level >= 4 )
  for( unsigned int i = 0; i < theEvent.zustandVektors.size(); ++i )
    if( theEvent.zustandVektors[i]->GetTruth() )
    {
      ZustandVektor* vekt = theEvent.zustandVektors[i];
      SciFiKalTrack* closest = NULL;
      double closest_dist = 1e9;

      for( unsigned int j = 0; j < theEvent.sciFiKalTracks.size(); ++j )
        if( fabs( theEvent.sciFiKalTracks[j]->point(0)->getPos().z() - vekt->GetPosition().z() ) < closest_dist )
        {
          closest_dist = fabs( theEvent.sciFiKalTracks[j]->point(0)->getPos().z() - vekt->GetPosition().z() );
	  closest = theEvent.sciFiKalTracks[j];
	}

      if( closest && closest_dist < 150 * cm )
      {
        double zpos = vekt->GetPosition().z();
        
	  Hep3Vector pos = closest->positionAtZ(zpos);
	  Hep3Vector  posErr = closest->posErrorAtZ(zpos);
	  Hep3Vector mom = closest->momentumAtZ(zpos);
	  Hep3Vector momErr = closest->momErrorAtZ(zpos);

	  //if( posM.num_row() == 3 && posM.num_col() == 3 && momM.num_row() == 3 && momM.num_col() == 3 )
          
	    ZustandVektor* v = new ZustandVektor();
	    v->SetPosition( Hep3Vector( pos[0], pos[1], pos[2] ) );
	    v->SetMomentum( Hep3Vector( mom[0], mom[1], mom[2] ) );
	    v->SetTruth( false );
	    v->SetMatrixElement( 0, 0, posErr[0] * posErr[0]);
	    v->SetMatrixElement( 0, 1, 0.0);
	    v->SetMatrixElement( 0, 2, 0.0);
	    v->SetMatrixElement( 1, 0, 0.0);
	    v->SetMatrixElement( 1, 1, posErr[1] * posErr[1]);
	    v->SetMatrixElement( 1, 2, 0.0);
	    v->SetMatrixElement( 2, 0, 0.0);
	    v->SetMatrixElement( 2, 1, 0.0);
	    v->SetMatrixElement( 2, 2, posErr[2] * posErr[2]);
	    v->SetMatrixElement( 4, 4, momErr[0] * momErr[0]);
	    v->SetMatrixElement( 4, 5, 0.0);
	    v->SetMatrixElement( 4, 6, 0.0);
	    v->SetMatrixElement( 5, 4, 0.0);
	    v->SetMatrixElement( 5, 5, momErr[1] * momErr[1]);
	    v->SetMatrixElement( 5, 6, 0.0);
	    v->SetMatrixElement( 6, 4, 0.0);
	    v->SetMatrixElement( 6, 5, 0.0);
	    v->SetMatrixElement( 6, 6, momErr[2] * momErr[2]);

	    // for now, we don't have PID, etc, so don't fill PID or time, energy, etc...

	    theEvent.zustandVektors.push_back( v );
      }
    }
}

SciFiRec::~SciFiRec()
{
  // delete SciFi objects

  for( unsigned int i = 0; i < theEvent.sciFiDigits.size(); ++i )
    delete theEvent.sciFiDigits[i];

  theEvent.sciFiDigits.clear();

  for( unsigned int i = 0; i < theEvent.sciFiClusters.size(); ++i )
    delete theEvent.sciFiClusters[i];

  theEvent.sciFiClusters.clear();

  for( unsigned int i = 0; i < theEvent.sciFiSpacePoints.size(); ++i )
    delete theEvent.sciFiSpacePoints[i];

  theEvent.sciFiSpacePoints.clear();

  for( unsigned int i = 0; i < theEvent.sciFiTracks.size(); ++i )
    delete theEvent.sciFiTracks[i];

  theEvent.sciFiTracks.clear();

  for( unsigned int i = 0; i < theEvent.sciFiKalTracks.size(); ++i )
    delete theEvent.sciFiKalTracks[i];

  theEvent.sciFiKalTracks.clear();
}
