/* 
SciFiPointRec - specific code to reconstruct space points in the SciFi tracker
M.Ellis - April 2005
D.Adey - Dec 2010
*/

#include <iostream>
#include <fstream>
#include <algorithm>
#include "CLHEP/Vector/ThreeVector.h"
#include "Recon/SciFi/SciFiRec.hh"
#include "Interface/SciFiDigit.hh"
#include "Calib/SciFiReconstructionParams.hh"
#include "Recon/SciFi/SciFiTrack.hh"
#include "Recon/SciFi/SciFiKalTrack.hh"
#include "Recon/SciFi/SciFiSpacePoint.hh"
#include "Recon/SciFi/SciFiDoubletCluster.hh"
#include "Interface/dataCards.hh"
using std::vector;

#include "Recon/SciFi/SciFiPointRec.hh"

bool clustComp(SciFiDoubletCluster* i,SciFiDoubletCluster* j) { return (i->getNPE() < j->getNPE()); }

void SciFiPointRec( struct MICEEvent& theEvent, struct MICERun* run, int& num_trip, int& num_dup )
{
  if( theEvent.sciFiClusters.size() > 100 ) // massive event, don't bother trying to make points
     return;

  // first, triplets...

  num_trip = 0;

  bool made_trips[2][6] = { { false, false, false, false, false, false },
                            { false, false, false, false, false, false } };

	vector<SciFiDoubletCluster*> clustvect = theEvent.sciFiClusters;;
 	sort(clustvect.begin(), clustvect.end(),clustComp);
  	//reverse(clustvect.begin(), clustvect.end());
	int clustvectsize = clustvect.size();
	vector<SciFiDoubletCluster*> clust[2][6][3];	

	/*sort clusters into tracker, station, plane*/

	for (unsigned int cla=0; cla<clustvectsize; ++cla) {		
		SciFiDoubletCluster* first = clustvect[cla];
		int tr = first->getTrackerNo();
		int stat = first->getStationNo();
		int plane = first->getDoubletNo();
		clust[tr][stat][plane].push_back(first);
	}

	for (unsigned int t=0; t<2; ++t) {
		for (unsigned int s=1; s<=5; ++s) {
			for (unsigned int ca=0; ca<clust[t][s][0].size(); ++ca)  if (! clust[t][s][0][ca]->used() ) {	
			for (unsigned int cb=0; cb<clust[t][s][1].size(); ++cb)  if (! clust[t][s][1][cb]->used() ) {
			for (unsigned int cc=0; cc<clust[t][s][2].size(); ++cc)  if (! clust[t][s][2][cc]->used() ) {
				SciFiSpacePoint* point = new SciFiSpacePoint( run, (clust[t][s][0])[ca], (clust[t][s][1])[cb], (clust[t][s][2])[cc] );
				int uvwsum(0);
                		for (unsigned int cs=0; cs <point->getNumClusters(); ++cs) {
                        		uvwsum += point->getDoubletCluster(cs)->getChanNoW();
                		}
				int trk = point->getTrackerNo();
				int station = point->getStationNo();	
              			if ((trk == 0 && station == 5 && uvwsum < 322 && uvwsum > 318) || (!(trk == 0 && station == 5) && uvwsum < 321 && uvwsum > 316))
                		{
               				point->keep();
					for (unsigned int j=0; j<point->getNumClusters(); ++j) {
						point->getDoubletCluster(j)->setInTrip();
					}
                			theEvent.sciFiSpacePoints.push_back( point );
              			}
				else delete point;
			}
			}
			}

			for (unsigned int dca=0; dca<clust[t][s][0].size(); ++dca)  if (! clust[t][s][0][dca]->used() ) {
                        for (unsigned int dcb=0; dcb<clust[t][s][1].size(); ++dcb)  if (! clust[t][s][1][dcb]->used() ) {	
				SciFiSpacePoint* duplet = new SciFiSpacePoint(run, clust[t][s][0][dca], clust[t][s][1][dcb]);
				if ( duplet->good() ) {
					theEvent.sciFiSpacePoints.push_back(duplet);
					 for (unsigned int j=0; j<duplet->getNumClusters(); ++j) {
                                                duplet->getDoubletCluster(j)->setInDoub();
                                         }
				}
			}
			}
			for (unsigned int dcc=0; dcc<clust[t][s][2].size(); ++dcc)  if (! clust[t][s][2][dcc]->used() ) {
                        for (unsigned int dcb=0; dcb<clust[t][s][1].size(); ++dcb)  if (! clust[t][s][1][dcb]->used() ) {
                                SciFiSpacePoint* duplet = new SciFiSpacePoint(run, clust[t][s][2][dcc], clust[t][s][1][dcb]);
                                if ( duplet->good() ) {
					theEvent.sciFiSpacePoints.push_back(duplet);
					 for (unsigned int j=0; j<duplet->getNumClusters(); ++j) {
                                                duplet->getDoubletCluster(j)->setInDoub();
                                         }
				}
                        }
                        }
			for (unsigned int dca=0; dca<clust[t][s][0].size(); ++dca)  if (! clust[t][s][0][dca]->used() ) {
                        for (unsigned int dcc=0; dcc<clust[t][s][2].size(); ++dcc)  if (! clust[t][s][2][dcc]->used() ) {
                                SciFiSpacePoint* duplet = new SciFiSpacePoint(run, clust[t][s][0][dca], clust[t][s][2][dcc]);
                                if (duplet->good() ) {
					theEvent.sciFiSpacePoints.push_back(duplet);
					 for (unsigned int j=0; j<duplet->getNumClusters(); ++j) {
                                                duplet->getDoubletCluster(j)->setInDoub();
                                         }
				}
                        }
                        }

		}
	}	



}
