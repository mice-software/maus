/* GlobalRec
*/

#include <iostream>
#include <fstream>
#include <algorithm>
#include "CLHEP/Vector/ThreeVector.h"
#include "GlobalRec.hh"

using std::vector;

#include "MouseTrack.hh"
#include "Recon/SciFi/SciFiKalTrack.hh"
#include "Recon/TOF/TofSpacePoint.hh"

GlobalRec::GlobalRec( MICEEvent& e, MICERun* r ) : theEvent( e ), m_run( r )
{
}

GlobalRec::~GlobalRec()
{
}

void GlobalRec::Process()
{
  int rec_level = m_run->DataCards->fetchValueInt( "RecLevel" );

  if( rec_level < 4 ) // no matching or PID required so exit
    return;

  // loop over SciFiKaltracks, putting them into a vector of MouseTracks

  std::vector<MouseTrack*> tracks;

  for( unsigned int i = 0; i < theEvent.sciFiKalTracks.size(); ++i )
    tracks.push_back( theEvent.sciFiKalTracks[i] );

  // now loop over each track and pick the best TofSpacePoint to add to it
  // ME - this will need to be extended to allow matching of the best 
  // point in multiple stations!

  for( unsigned int i = 0; i < tracks.size(); ++i )
  {
    // loop over TofSpacePoints

    double best_chi2 = 1e9;
    TofSpacePoint* best = NULL;
    Extrapolation best_extrap;

    for( unsigned int j = 0; j < theEvent.tofSpacePoints.size(); ++j )
    {
      TofSpacePoint* pt = theEvent.tofSpacePoints[j];
      Hep3Vector pos = pt->position();

      Extrapolation extrap = tracks[i]->extrapolateToZ( pos.z() );
      double chi2 = extrap.chi2MatchPos( pos );

      if( extrap.good() && chi2 > 0. && chi2 < best_chi2 )
      {
        best_chi2 = chi2;
        best = pt;
        best_extrap = extrap;
      }

      // if a good match has been found, make a TofPid object and add it to the track

      if( best_chi2 < 1e3 )
      {
	std::cout << "MATCHING: Matched a TofSpacePoint " << best->position() << " with chi2 " << best_chi2 << std::endl;
        tracks[i]->addTofPid( new TofPid( best_extrap, best ) ); 
      }
    }
  }
}
