/*
SciFiStraightTrackRec - pattern recognition for tracks in no magnetic field for the Sci Fi tracker

A.Fish - November, 2006
*/

#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>

#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Matrix/Matrix.h"

#include "Interface/MICEEvent.hh"
#include "Interface/MICERun.hh"

#include "Recon/SciFi/KalmanSeed.hh"
#include "Recon/SciFi/SciFiRec.hh"
#include "Interface/SciFiDigit.hh"
#include "Recon/SciFi/SciFiKalTrack.hh"
#include "Recon/SciFi/SciFiSpacePoint.hh"
#include "Recon/SciFi/SciFiDoubletCluster.hh"
#include "Interface/dataCards.hh"
#include "Recon/SciFi/SciFiSeed.hh"
#include "BeamTools/BTField.hh"
#include "BeamTools/BTFieldConstructor.hh"

using std::vector;

#include "Recon/SciFi/SciFiStraightTrackRec.hh"

static const int MAXCOMB = 10000; //ME - should make this a data card at some stage if it proves useful

void	makeStraightTracks( int tracker, MICEEvent& e, MICERun* r );

SciFiKalTrack* 	makeFivePointStraightTrack( std::vector<std::vector<SciFiSpacePoint*> >&, MICEEvent&, MICERun* );
SciFiKalTrack* 	makeFourPointStraightTrack( std::vector<std::vector<SciFiSpacePoint*> >&, MICEEvent&, MICERun* );
SciFiKalTrack* 	makeThreePointStraightTrack( std::vector<std::vector<SciFiSpacePoint*> >&, MICEEvent&, MICERun* );

void SciFiStraightTrackRec( MICEEvent& theEvent, MICERun* theRun )
{
  for( int tracker = 0; tracker <= 1; ++tracker )
    makeStraightTracks( tracker, theEvent, theRun );
}

void	makeStraightTracks( int tracker, MICEEvent& e, MICERun* r )
{
  bool made = true;

  while( made )
  {
    std::vector<std::vector<SciFiSpacePoint*> > pnts(6);

    for( unsigned int i = 0; i < e.sciFiSpacePoints.size(); ++i )
      if( e.sciFiSpacePoints[i]->getTrackerNo() == tracker && e.sciFiSpacePoints[i]->used() == false )
        pnts[ e.sciFiSpacePoints[i]->getStationNo() ].push_back( e.sciFiSpacePoints[i] );

    made = false;

    SciFiKalTrack* trk = makeFivePointStraightTrack( pnts, e, r );

    if( ! trk )
      trk = makeFourPointStraightTrack( pnts, e, r );

    if( ! trk )
      trk = makeThreePointStraightTrack( pnts, e, r );

    if( trk )
    {
      trk->keep();
      e.sciFiKalTracks.push_back( trk );
      made = true;
    }
  }
}

SciFiKalTrack* 	makeFivePointStraightTrack( std::vector<std::vector<SciFiSpacePoint*> >& pnts, 
				    MICEEvent& e, MICERun* r )
{
  int comb = pnts[1].size() * pnts[2].size() * pnts[3].size() * pnts[4].size() * pnts[5].size();

  if( comb > MAXCOMB )
    return NULL;

  double P =  r->DataCards->fetchValueDouble( "RecP" );
  double Q = r->DataCards->fetchValueDouble( "BeamCharge" );
  double cut = r->DataCards->fetchValueDouble( "SciFiTrackRecChi2Cut" );

  SciFiKalTrack* best = NULL;

  for( unsigned int a = 0; a < pnts[1].size(); ++a )
    for( unsigned int b = 0; b < pnts[2].size(); ++b )
      for( unsigned int c = 0; c < pnts[3].size(); ++c )
        for( unsigned int d = 0; d < pnts[4].size(); ++d )
          for( unsigned int e = 0; e < pnts[5].size(); ++e )
          {
  	    SciFiSpacePoint* A = pnts[1][a];
  	    SciFiSpacePoint* B = pnts[2][b];
  	    SciFiSpacePoint* C = pnts[3][c];
  	    SciFiSpacePoint* D = pnts[4][d];
  	    SciFiSpacePoint* E = pnts[5][e];
 
	    double mx = ( A->getPos().x() - E->getPos().x() ) / ( A->getPos().z() - E->getPos().z() );
	    double my = ( A->getPos().y() - E->getPos().y() ) / ( A->getPos().z() - E->getPos().z() );

	    double dxb = A->getPos().x() + mx * ( B->getPos().z() - A->getPos().z() ) - B->getPos().x();
	    double dxc = A->getPos().x() + mx * ( C->getPos().z() - A->getPos().z() ) - C->getPos().x();
	    double dxd = A->getPos().x() + mx * ( D->getPos().z() - A->getPos().z() ) - D->getPos().x();

	    double dyb = A->getPos().y() + my * ( B->getPos().z() - A->getPos().z() ) - B->getPos().y();
	    double dyc = A->getPos().y() + my * ( C->getPos().z() - A->getPos().z() ) - C->getPos().y();
	    double dyd = A->getPos().y() + my * ( D->getPos().z() - A->getPos().z() ) - D->getPos().y();

	    if( fabs(dxb) < 15. * mm && fabs(dxc) < 15. * mm && fabs(dxd) < 15. * mm &&
	        fabs(dyb) < 15. * mm && fabs(dyc) < 15. * mm && fabs(dyd) < 15. * mm )
            {
	      std::vector<SciFiSpacePoint*> pnts;

	      pnts.push_back( A );
	      pnts.push_back( B );
	      pnts.push_back( C );
	      pnts.push_back( D );
	      pnts.push_back( E );

	      SciFiKalTrack* trk = new SciFiKalTrack( pnts, P, Q );

	      if( ! best || trk->chi2Dof() < best->chi2Dof() )
	      {
	        if( best )
	          delete best;
	        best = trk;
	      }
	      else
	        delete trk;
            }
          }

  if( best && best->chi2Dof() > cut ) // best track is not good enough
  {
    delete best;
    best = NULL;
  }

  return best;
}

SciFiKalTrack* 	makeFourPointStraightTrack( std::vector<std::vector<SciFiSpacePoint*> >& pnts, 
				    MICEEvent& e, MICERun* r )
{
  int comb = pnts[1].size() * pnts[2].size() * pnts[3].size() * pnts[4].size() * pnts[5].size();

  if( comb > MAXCOMB )
    return NULL;

  double P = r->DataCards->fetchValueDouble( "RecP" );
  double Q = r->DataCards->fetchValueDouble( "BeamCharge" );
  double cut = r->DataCards->fetchValueDouble( "SciFiTrackRecChi2Cut" );

  SciFiKalTrack* best = NULL;

  for(int st1 = 1; st1 <= 2; ++st1 )
    for( int st2 = st1 + 1; st2 <= 3; ++st2 )
      for( int st3 = st2 + 1; st3 <= 4; ++st3 )
        for( int st4 = st3 + 1; st4 <= 5; ++st4 )
  for( unsigned int a = 0; a < pnts[st1].size(); ++a )
    for( unsigned int b = 0; b < pnts[st2].size(); ++b )
      for( unsigned int c = 0; c < pnts[st3].size(); ++c )
        for( unsigned int d = 0; d < pnts[st4].size(); ++d )
        {
  	    SciFiSpacePoint* A = pnts[st1][a];
  	    SciFiSpacePoint* B = pnts[st2][b];
  	    SciFiSpacePoint* C = pnts[st3][c];
  	    SciFiSpacePoint* D = pnts[st4][d];
 
	    double mx = ( A->getPos().x() - D->getPos().x() ) / ( A->getPos().z() - D->getPos().z() );
	    double my = ( A->getPos().y() - D->getPos().y() ) / ( A->getPos().z() - D->getPos().z() );

	    double dxb = A->getPos().x() + mx * ( B->getPos().z() - A->getPos().z() ) - B->getPos().x();
	    double dxc = A->getPos().x() + mx * ( C->getPos().z() - A->getPos().z() ) - C->getPos().x();

	    double dyb = A->getPos().y() + my * ( B->getPos().z() - A->getPos().z() ) - B->getPos().y();
	    double dyc = A->getPos().y() + my * ( C->getPos().z() - A->getPos().z() ) - C->getPos().y();

	    if( fabs(dxb) < 15. * mm && fabs(dxc) < 15. * mm &&
	        fabs(dyb) < 15. * mm && fabs(dyc) < 15. * mm )
            {
	      std::vector<SciFiSpacePoint*> pnts;

	      pnts.push_back( A );
	      pnts.push_back( B );
	      pnts.push_back( C );
	      pnts.push_back( D );

	      SciFiKalTrack* trk = new SciFiKalTrack( pnts, P, Q );

	      if( ! best || trk->chi2Dof() < best->chi2Dof() )
	      {
	        if( best )
	          delete best;
	        best = trk;
	      }
	      else
	        delete trk;
            }
          }

  if( best && best->chi2Dof() > cut ) // best track is not good enough
  {
    delete best;
    best = NULL;
  }

  return best;
}

SciFiKalTrack* 	makeThreePointStraightTrack( std::vector<std::vector<SciFiSpacePoint*> >& pnts, 
				    MICEEvent& e, MICERun* r )
{
  int comb = pnts[1].size() * pnts[2].size() * pnts[3].size() * pnts[4].size() * pnts[5].size();

  if( comb > MAXCOMB )
    return NULL;

  double P = r->DataCards->fetchValueDouble( "RecP" );
  double Q = r->DataCards->fetchValueDouble( "BeamCharge" );
  double cut = r->DataCards->fetchValueDouble( "SciFiTrackRecChi2Cut" );

  SciFiKalTrack* best = NULL;

  for(int st1 = 1; st1 <= 3; ++st1 )
    for( int st2 = st1 + 1; st2 <= 4; ++st2 )
      for( int st3 = st2 + 1; st3 <= 5; ++st3 )
  for( unsigned int a = 0; a < pnts[st1].size(); ++a )
    for( unsigned int b = 0; b < pnts[st2].size(); ++b )
      for( unsigned int c = 0; c < pnts[st3].size(); ++c )
        {
  	    SciFiSpacePoint* A = pnts[st1][a];
  	    SciFiSpacePoint* B = pnts[st2][b];
  	    SciFiSpacePoint* C = pnts[st3][c];
 
	    double mx = ( A->getPos().x() - C->getPos().x() ) / ( A->getPos().z() - C->getPos().z() );
	    double my = ( A->getPos().y() - C->getPos().y() ) / ( A->getPos().z() - C->getPos().z() );

	    double dxb = A->getPos().x() + mx * ( B->getPos().z() - A->getPos().z() ) - B->getPos().x();
	    double dyb = A->getPos().y() + my * ( B->getPos().z() - A->getPos().z() ) - B->getPos().y();

	    if( fabs(dxb) < 15. * mm && 
	        fabs(dyb) < 15. * mm )
            {
	      std::vector<SciFiSpacePoint*> pnts;

	      pnts.push_back( A );
	      pnts.push_back( B );
	      pnts.push_back( C );

	      SciFiKalTrack* trk = new SciFiKalTrack( pnts, P, Q );

	      if( ! best || trk->chi2Dof() < best->chi2Dof() )
	      {
	        if( best )
	          delete best;
	        best = trk;
	      }
	      else
	        delete trk;
            }
          }

  if( best && best->chi2Dof() > cut ) // best track is not good enough
  {
    delete best;
    best = NULL;
  }

  return best;
}
