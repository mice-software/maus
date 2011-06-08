/*
SciFiHelixTrackRec - pattern recognition for tracks in a solenoidal field for the Sci Fi tracker

M.Ellis - April, 2005
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

#include "KalmanSeed.hh"
#include "SciFiRec.hh"
#include "Interface/SciFiDigit.hh"
#include "SciFiKalTrack.hh"
#include "SciFiSpacePoint.hh"
#include "SciFiDoubletCluster.hh"
#include "Interface/dataCards.hh"
#include "SciFiSeed.hh"
#include "BeamTools/BTField.hh"
#include "BeamTools/BTFieldConstructor.hh"

using std::vector;

#include "SciFiHelixTrackRec.hh"

double solvePT( std::vector<SciFiSpacePoint*>&, double );

void	makeTracks( int tracker, MICEEvent& e, MICERun* r );

SciFiKalTrack* 	makeFivePointTrack( std::vector<std::vector<SciFiSpacePoint*> >&, MICEEvent&, MICERun*, std::vector<int>& );
SciFiKalTrack* 	makeFourPointTrack( std::vector<std::vector<SciFiSpacePoint*> >&, MICEEvent&, MICERun*, std::vector<int>& );
SciFiKalTrack* 	makeThreePointTrack( std::vector<std::vector<SciFiSpacePoint*> >&, MICEEvent&, MICERun*, std::vector<int>& );

void SciFiHelixTrackRec( MICEEvent& theEvent, MICERun* theRun )
{
  for( int tracker = 0; tracker <= 1; ++tracker )
    makeTracks( tracker, theEvent, theRun );
}

void	makeTracks( int tracker, MICEEvent& e, MICERun* r )
{
  // define a list of station numbers for each tracker that are in order
  // of increasing global z position

  std::vector<int> order;
  std::vector<const MiceModule*> track_mods = r->miceModule->findModulesByPropertyExists( "int", "Station" );

  // get the stations from this tracker

  std::map<double,int> stats;

  for( unsigned int i = 0; i < track_mods.size(); ++i )
    if( track_mods[i]->propertyExists( "Tracker", "int" ) && track_mods[i]->propertyInt( "Tracker" ) == tracker && track_mods[i]->propertyExistsThis( "Station", "int" ) )
       stats[track_mods[i]->globalPosition().z()] = track_mods[i]->propertyInt( "Station" );
    
  // loop over each entry in stats, if it is a tracking station, add it to the list

  for( std::map<double,int>::iterator it = stats.begin(); it!= stats.end(); ++it )
  {
    int stat = it->second;
    if( stat >= 1 && stat <= 5 )
      order.push_back( stat );
  }

  while( order.size() < 5 )
    order.push_back( -1 );

  std::vector<std::vector<SciFiSpacePoint*> > pnts(6);

  for( unsigned int i = 0; i < e.sciFiSpacePoints.size(); ++i )
    if( e.sciFiSpacePoints[i]->getTrackerNo() == tracker )
      pnts[ e.sciFiSpacePoints[i]->getStationNo() ].push_back( e.sciFiSpacePoints[i] );

  SciFiKalTrack* trk = makeFivePointTrack( pnts, e, r, order );

  if( ! trk )
    trk = makeFourPointTrack( pnts, e, r, order );

  if( ! trk )
    trk = makeThreePointTrack( pnts, e, r, order );


  if( trk ) {
	trk->keep();
    e.sciFiKalTracks.push_back( trk );
	}
}

SciFiKalTrack* 	makeFivePointTrack( std::vector<std::vector<SciFiSpacePoint*> >& pnts, 
				    MICEEvent& e, MICERun* r, std::vector<int>& order )
{
  if( order[0] < 1 || order[1] < 1 || order[2] < 1 || order[3] < 1 || order[4] < 1 )
    return NULL;

  bool mcs = r->DataCards->fetchValueString( "MultipleScatteringModel" ) != "none";

  double pguess = MyDataCards.fetchValueDouble( "RecP" );

  SciFiKalTrack* best = NULL;

  double B, Q, px, py, pz;
  
  Q = +1;

  bool useRecCuts = r->DataCards->fetchValueInt( "UseRecCuts" );
  double minpx = r->DataCards->fetchValueDouble( "RecMinPX" );
  double minpy = r->DataCards->fetchValueDouble( "RecMinPY" );
  double minpt = r->DataCards->fetchValueDouble( "RecMinPT" );
  double minpz = r->DataCards->fetchValueDouble( "RecMinPZ" );
  double maxpx = r->DataCards->fetchValueDouble( "RecMaxPX" );
  double maxpy = r->DataCards->fetchValueDouble( "RecMaxPY" );
  double maxpt = r->DataCards->fetchValueDouble( "RecMaxPT" );
  double maxpz = r->DataCards->fetchValueDouble( "RecMaxPZ" );

  for( unsigned int a = 0; a < pnts[order[0]].size(); ++a )
    for( unsigned int b = 0; b < pnts[order[1]].size(); ++b )
      for( unsigned int c = 0; c < pnts[order[2]].size(); ++c )
        for( unsigned int d = 0; d < pnts[order[3]].size(); ++d )
          for( unsigned int e = 0; e < pnts[order[4]].size(); ++e )
          {
	    int pos1;
   	    double px1, py1, pz1;
	    double sigpx, sigpy, sigpz;
 	    double sigpx1, sigpy1, sigpz1;
            double pos[4];
            double EMfield[6];
	    double sigx, sigy;

	    sigx = pnts[order[1]][b]->getPosE().x();
	    sigy = pnts[order[1]][b]->getPosE().y();

            pos[0] = pnts[order[1]][b]->getPos().x();
            pos[1] = pnts[order[1]][b]->getPos().y();
            pos[2] = pnts[order[1]][b]->getPos().z();
            pos[3] = pnts[order[1]][b]->getTime();

            r->btFieldConstructor->GetMagneticField()->GetFieldValue( pos, EMfield );

 	    B = EMfield[2];

	    if( pguess > 0 )
              pz = pguess;

            bool good = momentumFromPoints( pnts[order[0]][a]->getPos(), pnts[order[1]][b]->getPos(), pnts[order[2]][c]->getPos(),
                                            sigx, sigy, B, Q, px, py, pz, pos1, sigpx1, sigpy1, sigpz1, mcs );

	    px1 = px; py1 = py; pz1 = pz;

	    int pos2;
            double pt1 = sqrt( px * px + py * py );
            double p1 = sqrt( pt1 * pt1 + pz * pz );

            pos[0] = pnts[order[3]][d]->getPos().x();
            pos[1] = pnts[order[3]][d]->getPos().y();
            pos[2] = pnts[order[3]][d]->getPos().z();
            pos[3] = pnts[order[3]][d]->getTime();

            r->btFieldConstructor->GetMagneticField()->GetFieldValue( pos, EMfield );

 	    B = EMfield[2];

            good &= momentumFromPoints( pnts[order[2]][c]->getPos(), pnts[order[3]][d]->getPos(), pnts[order[4]][e]->getPos(),
                                        sigx, sigy, B, Q, px, py, pz, pos2, sigpx, sigpy, sigpz, mcs );

            double pt2 = sqrt( px * px + py * py );
            double p2 = sqrt( pt2 * pt2 + pz * pz );

	    SciFiSpacePoint* point = pnts[order[0]][a];
 	    if( pos1 == 2 ) point = pnts[order[1]][b];
	    else if( pos1 == 3 ) point = pnts[order[2]][c];

            SciFiSpacePoint* point2 = pnts[order[2]][c];
            if( pos2 == 2 ) point = pnts[order[3]][d];
            else if( pos2 == 3 ) point = pnts[order[4]][e];

            int seed = 1;
            if( point2->getPos().z() < point->getPos().z() )
            {
              seed = 2;
              point = point2;
            }

            if( good && fabs( pt1 - pt2 ) < 5 && fabs( p1 - p2 ) < 15 )
            {
              Hep3Vector p( px1, py1, pz1 );
              Hep3Vector sigp( sigpx1, sigpy1, sigpz1 );

              if( seed == 2)
              {
                p = Hep3Vector( px, py, pz );
                sigp = Hep3Vector( sigpx, sigpy, sigpz );
              }

              KalmanSeed kalSeed( point->getPos(), point->getPosE(), p, sigp, Q );

              kalSeed.setVolumeName( point->volumeName() );

   	      std::vector<SciFiSpacePoint*> pts;

              pts.push_back( pnts[order[0]][a] );
              pts.push_back( pnts[order[1]][b] );
              pts.push_back( pnts[order[2]][c] );
              pts.push_back( pnts[order[3]][d] );
              pts.push_back( pnts[order[4]][e] );

	      std::sort( pts.begin(), pts.end(), sortPoints );

              SciFiKalTrack* trk = new SciFiKalTrack( pts, kalSeed );

	      double ok = true;

	      if( trk->chi2Dof() <= 0. ) ok = false;

	      if( ok && useRecCuts )
	      {
	        Hep3Vector Mom = trk->momentumAtZ( point->getPos().z() );
                if( Mom.x() < minpx || Mom.x() > maxpx || Mom.y() < minpy || Mom.y() > maxpy || Mom.z() < minpz || Mom.z() > maxpz || 
                    sqrt( Mom.x() * Mom.x() + Mom.y() * Mom.y() ) < minpt || sqrt( Mom.x() * Mom.x() + Mom.y() * Mom.y() ) > maxpt )
	  	  ok = false;
	      }

	      if( trk->chi2Dof() <= 0. ) ok = false;

	      if( ok && ( ! best || trk->chi2Dof() < best->chi2Dof() ) )
              {
	     	if( best ) delete best;
		best = trk;
	      }
	      else
		delete trk;
            }
          }
  return best;

}

SciFiKalTrack* 	makeFourPointTrack( std::vector<std::vector<SciFiSpacePoint*> >& pnts, 
				    MICEEvent& e, MICERun* r, std::vector<int>& order )
{
  bool mcs = r->DataCards->fetchValueString( "MultipleScatteringModel" ) != "none";

  double pguess = MyDataCards.fetchValueDouble( "RecP" );

  SciFiKalTrack* best = NULL;

  double B, Q, px, py, pz;
  
  Q = +1;

  bool useRecCuts = r->DataCards->fetchValueInt( "UseRecCuts" );
  double minpx = r->DataCards->fetchValueDouble( "RecMinPX" );
  double minpy = r->DataCards->fetchValueDouble( "RecMinPY" );
  double minpt = r->DataCards->fetchValueDouble( "RecMinPT" );
  double minpz = r->DataCards->fetchValueDouble( "RecMinPZ" );
  double maxpx = r->DataCards->fetchValueDouble( "RecMaxPX" );
  double maxpy = r->DataCards->fetchValueDouble( "RecMaxPY" );
  double maxpt = r->DataCards->fetchValueDouble( "RecMaxPT" );
  double maxpz = r->DataCards->fetchValueDouble( "RecMaxPZ" );

  for( int st1 = 0; st1 < 5; ++st1 )
    for( int st2 = st1 + 1; st2 < 5; ++st2 )
      for( int st3 = st2 + 1; st3 < 5; ++st3 )
        for( int st4 = st3 + 1; st4 < 5; ++st4 )
  if( order[st1] > 0 && order[st2] > 0 && order[st3] > 0 && order[st4] > 0 )
  for( unsigned int a = 0; a < pnts[order[st1]].size(); ++a )
    for( unsigned int b = 0; b < pnts[order[st2]].size(); ++b )
      for( unsigned int c = 0; c < pnts[order[st3]].size(); ++c )
        for( unsigned int d = 0; d < pnts[order[st4]].size(); ++d )
          {
	    int pos1;
   	    double px1, py1, pz1;
	    double sigpx, sigpy, sigpz;
 	    double sigpx1, sigpy1, sigpz1;
            double pos[4];
            double EMfield[6];
	    double sigx, sigy;

	    sigx = pnts[order[st2]][b]->getPosE().x();
	    sigy = pnts[order[st2]][b]->getPosE().y();

            pos[0] = pnts[order[st2]][b]->getPos().x();
            pos[1] = pnts[order[st2]][b]->getPos().y();
            pos[2] = pnts[order[st2]][b]->getPos().z();
            pos[3] = pnts[order[st2]][b]->getTime();

            r->btFieldConstructor->GetMagneticField()->GetFieldValue( pos, EMfield );

 	    B = EMfield[2];

	    if( pguess > 0 )
              pz = pguess;

            bool good = momentumFromPoints( pnts[order[st1]][a]->getPos(), pnts[order[st2]][b]->getPos(), pnts[order[st3]][c]->getPos(),
                                            sigx, sigy, B, Q, px, py, pz, pos1, sigpx1, sigpy1, sigpz1, mcs );

	    px1 = px; py1 = py; pz1 = pz;

	    int pos2;
            double pt1 = sqrt( px * px + py * py );
            double p1 = sqrt( pt1 * pt1 + pz * pz );

            pos[0] = pnts[order[st3]][c]->getPos().x();
            pos[1] = pnts[order[st3]][c]->getPos().y();
            pos[2] = pnts[order[st3]][c]->getPos().z();
            pos[3] = pnts[order[st3]][c]->getTime();

            r->btFieldConstructor->GetMagneticField()->GetFieldValue( pos, EMfield );

 	    B = EMfield[2];

	    if( pguess > 0 )
              pz = pguess;

            good &= momentumFromPoints( pnts[order[st2]][b]->getPos(), pnts[order[st3]][c]->getPos(), pnts[order[st4]][d]->getPos(),
                                        sigx, sigy, B, Q, px, py, pz, pos2, sigpx, sigpy, sigpz, mcs );

            double pt2 = sqrt( px * px + py * py );
            double p2 = sqrt( pt2 * pt2 + pz * pz );

	    SciFiSpacePoint* point = pnts[order[st1]][a];
 	    if( pos1 == 2 ) point = pnts[order[st2]][b];
	    else if( pos1 == 3 ) point = pnts[order[st3]][c];

	    SciFiSpacePoint* point2 = pnts[order[st2]][b];
 	    if( pos2 == 2 ) point = pnts[order[st3]][c];
	    else if( pos2 == 3 ) point = pnts[order[st4]][d];

            int seed = 1;
            if( point2->getPos().z() < point->getPos().z() )
            {
              seed = 2;
              point = point2;
            }

            if( good && fabs( pt1 - pt2 ) < 5 && fabs( p1 - p2 ) < 15 )
            {
              Hep3Vector p( px1, py1, pz1 );
              Hep3Vector sigp( sigpx1, sigpy1, sigpz1 );

              if( seed == 2)
              {
		p = Hep3Vector( px, py, pz );
		sigp = Hep3Vector( sigpx, sigpy, sigpz );
	      }

	      KalmanSeed kalSeed( point->getPos(), point->getPosE(), p, sigp, Q );
              kalSeed.setVolumeName( point->volumeName() );

   	      std::vector<SciFiSpacePoint*> pts;

              pts.push_back( pnts[order[st1]][a] );
              pts.push_back( pnts[order[st2]][b] );
              pts.push_back( pnts[order[st3]][c] );
              pts.push_back( pnts[order[st4]][d] );

	      std::sort( pts.begin(), pts.end(), sortPoints );

              SciFiKalTrack* trk = new SciFiKalTrack( pts, kalSeed );

	      double ok = true;

	      if( trk->chi2Dof() <= 0. ) ok = false;

	      if( ok && useRecCuts )
              {
	        Hep3Vector Mom = trk->momentumAtZ( point->getPos().z() );
                if( Mom.x() < minpx || Mom.x() > maxpx || Mom.y() < minpy || Mom.y() > maxpy || Mom.z() < minpz || Mom.z() > maxpz || 
                  sqrt( Mom.x() * Mom.x() + Mom.y() * Mom.y() ) < minpt || sqrt( Mom.x() * Mom.x() + Mom.y() * Mom.y() ) > maxpt )
		  ok = false;
  	      }

	      if( ok && ( ! best || trk->chi2Dof() < best->chi2Dof() ) )
              {
	     	if( best ) delete best;
		best = trk;
	      }
	      else
		delete trk;
            }
          }
  return best;

}

SciFiKalTrack* 	makeThreePointTrack( std::vector<std::vector<SciFiSpacePoint*> >& pnts, 
				    MICEEvent& e, MICERun* r, std::vector<int>& order )
{
  bool mcs = r->DataCards->fetchValueString( "MultipleScatteringModel" ) != "none";

  double pguess = MyDataCards.fetchValueDouble( "RecP" );

  SciFiKalTrack* best = NULL;

  double B, Q, px, py, pz;
  
  Q = +1;

  bool useRecCuts = r->DataCards->fetchValueInt( "UseRecCuts" );
  double minpx = r->DataCards->fetchValueDouble( "RecMinPX" );
  double minpy = r->DataCards->fetchValueDouble( "RecMinPY" );
  double minpt = r->DataCards->fetchValueDouble( "RecMinPT" );
  double minpz = r->DataCards->fetchValueDouble( "RecMinPZ" );
  double maxpx = r->DataCards->fetchValueDouble( "RecMaxPX" );
  double maxpy = r->DataCards->fetchValueDouble( "RecMaxPY" );
  double maxpt = r->DataCards->fetchValueDouble( "RecMaxPT" );
  double maxpz = r->DataCards->fetchValueDouble( "RecMaxPZ" );

  for( int st1 = 0; st1 < 5; ++st1 )
    for( int st2 = st1 + 1; st2 < 5; ++st2 )
      for( int st3 = st2 + 1; st3 < 5; ++st3 )
  if( order[st1] > 0 && order[st2] > 0 && order[st3] > 0 )
  for( unsigned int a = 0; a < pnts[order[st1]].size(); ++a )
    for( unsigned int b = 0; b < pnts[order[st2]].size(); ++b )
      for( unsigned int c = 0; c < pnts[order[st3]].size(); ++c )
          {
	    int pos1;
   	    double px1, py1, pz1;
 	    double sigpx1, sigpy1, sigpz1;
            double pos[4];
            double EMfield[6];
	    double sigx, sigy;

	    sigx = pnts[order[st2]][b]->getPosE().x();
	    sigy = pnts[order[st2]][b]->getPosE().y();

            pos[0] = pnts[order[st2]][b]->getPos().x();
            pos[1] = pnts[order[st2]][b]->getPos().y();
            pos[2] = pnts[order[st2]][b]->getPos().z();
            pos[3] = pnts[order[st2]][b]->getTime();

            r->btFieldConstructor->GetMagneticField()->GetFieldValue( pos, EMfield );

 	    B = EMfield[2];

            pz = pguess;

            bool good = momentumFromPoints( pnts[order[st1]][a]->getPos(), pnts[order[st2]][b]->getPos(), pnts[order[st3]][c]->getPos(),
                                            sigx, sigy, B, Q, px, py, pz, pos1, sigpx1, sigpy1, sigpz1, mcs );

	    px1 = px; py1 = py; pz1 = pz;

	    SciFiSpacePoint* point = pnts[order[st1]][a];
 	    if( pos1 == 2 ) point = pnts[order[st2]][b];
	    else if( pos1 == 3 ) point = pnts[order[st3]][c];

            if( good )
            {
	      KalmanSeed kalSeed( point->getPos(), point->getPosE(), Hep3Vector( px1, py1, pz1 ), Hep3Vector( sigpx1, sigpy1, sigpz1 ), Q );
              kalSeed.setVolumeName( point->volumeName() );

   	      std::vector<SciFiSpacePoint*> pts;

              pts.push_back( pnts[order[st1]][a] );
              pts.push_back( pnts[order[st2]][b] );
              pts.push_back( pnts[order[st3]][c] );

	      std::sort( pts.begin(), pts.end(), sortPoints );

              SciFiKalTrack* trk = new SciFiKalTrack( pts, kalSeed );

	      double ok = true;

	      if( trk->chi2Dof() <= 0. ) ok = false;

	      if( ok && useRecCuts )
	      {
	        Hep3Vector Mom = trk->momentumAtZ( point->getPos().z() );
                if( Mom.x() < minpx || Mom.x() > maxpx || Mom.y() < minpy || Mom.y() > maxpy || Mom.z() < minpz || Mom.z() > maxpz || 
                    sqrt( Mom.x() * Mom.x() + Mom.y() * Mom.y() ) < minpt || sqrt( Mom.x() * Mom.x() + Mom.y() * Mom.y() ) > maxpt )
		  ok = false;
	      }

	      if( ok && ( ! best || trk->chi2Dof() < best->chi2Dof() ) )
              {
	     	if( best ) delete best;
		best = trk;
	      }
	      else
		delete trk;
            }
          }
  return best;
}
