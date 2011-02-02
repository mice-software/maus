/* 
** Written for the MICE proposal simulation
** Some examples from the ROOT project at CERN used
** Ed McKigney - Oct 9 2002
*/

#include <vector>
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Matrix/Matrix.h"

#include "SciFiSpacePoint.hh"
#include "SciFiDoubletCluster.hh"
#include "Interface/MICERun.hh"
#include "Config/MiceModule.hh"
#include "Interface/dataCards.hh"
#include "Interface/MiceMaterials.hh"
#include "CLHEP/Units/SystemOfUnits.h"
#include <cmath>

using std::vector;
using CLHEP::Hep3Vector;

extern std::vector<const MiceModule*> modules;		// Defined and set in Cluster reconstruction

SciFiSpacePoint::SciFiSpacePoint()
{
  miceMemory.addNew( Memory::SciFiSpacePoint );

        pos = Hep3Vector(0,0,0);
	posE = Hep3Vector(0,0,0);
	time = 0;
	timeE = 0;
	nChans = 0;
	nMuChans = 0;
	nPE = 0;
	chi2 = -1;
	trackerNo = -1;
	stationNo = -1;
	m_used = false;
	clusters = 0;
	_run = NULL;
}


//triplet constructor

SciFiSpacePoint::SciFiSpacePoint( MICERun* run, SciFiDoubletCluster* clust1, 
                                  SciFiDoubletCluster* clust2, SciFiDoubletCluster* clust3  )

{
  _volName = "";
  _run = run;
  miceMemory.addNew( Memory::SciFiSpacePoint );
  createCluster(clust1, clust2, clust3);
}

SciFiSpacePoint::SciFiSpacePoint( MICERun* run, SciFiDoubletCluster* clust1, 
                                  SciFiDoubletCluster* clust2)
{
  _volName = "";
  _run = run;
  miceMemory.addNew( Memory::SciFiSpacePoint );
  createCluster(clust1, clust2);
}

void SciFiSpacePoint::createCluster( SciFiDoubletCluster* clust1, SciFiDoubletCluster* clust2,
				     SciFiDoubletCluster* clust3  )
{
  clusters = 3;
  m_used = false;

  nPE = clust1->getNPE() + clust2->getNPE() + clust3->getNPE();
 
  Hep3Vector p1 = crossingPos( clust1, clust2 );
  Hep3Vector p2 = crossingPos( clust1, clust3 );
  Hep3Vector p3 = crossingPos( clust2, clust3 );

  pos = ( p1 + p2 + p3 ) / 3.;

  Hep3Vector p;
  SciFiDoubletCluster* xclust = NULL;

  if( clust1->getDoubletNo() == 1 )
  {
    xclust = clust1;
    p = p3;
  }
  else if( clust2->getDoubletNo() == 1 )
  {
    xclust = clust2;
    p = p2;
  }
  else
  {
    xclust = clust3;
    p = p1;
  }

  SciFiDoubletCluster* vclust = NULL;
  SciFiDoubletCluster* wclust = NULL;

  if( clust1->getDoubletNo() == 0 ) vclust = clust1;
  if( clust2->getDoubletNo() == 0 ) vclust = clust2;
  if( clust3->getDoubletNo() == 0 ) vclust = clust3;

  if( clust1->getDoubletNo() == 2 ) wclust = clust1;
  if( clust2->getDoubletNo() == 2 ) wclust = clust2;
  if( clust3->getDoubletNo() == 2 ) wclust = clust3;

  // at this point, the vector p has the position determined
  // by the intersection of the V and W views

  // now determine the perpendicular distance from the hit on the
  // X view, to the intersection of the V and W views

  Hep3Vector x_dir = xclust->direction();
  Hep3Vector x_pos = xclust->position();

  // assume that the station is perpendicular to the Z axis!

  double x1 = x_pos.x();
  double y1 = x_pos.y();
  double x2 = x_pos.x() + 10. * x_dir.x();
  double y2 = x_pos.y() + 10. * x_dir.y();
  double x0 = p.x();
  double y0 = p.y();

  _dist = ( ( x2 - x1 ) * ( y1 - y0 ) - ( x1 - x0 ) * ( y2 - y1 ) ) /
          sqrt( ( x2 - x1 ) * ( x2 - x1 ) + ( y2 - y1 ) * ( y2 - y1 ) );

  trackerNo = clust1->getTrackerNo();
  stationNo = clust1->getStationNo();

  //finding the micemodule this spacepoint is in

  const MiceModule* mod = NULL;
   
  for( unsigned int j = 0; /*! mod && */ j < modules.size(); ++j )
  {
    if( modules[j]->propertyExists( "Tracker", "int" ) && modules[j]->propertyInt( "Tracker" ) == clust1->getTrackerNo() &&
        modules[j]->propertyExists( "Station", "int" ) && modules[j]->propertyInt( "Station" ) == clust1->getStationNo() &&
        modules[j]->propertyExists( "Plane", "int" ) && modules[j]->propertyInt( "Plane" ) == clust1->getDoubletNo() )
	 mod = modules[j];
  }

  double thickness = 1. * CLHEP::m;
  double fibrePitch = 1. * CLHEP::m;
  double X0 = 1. * CLHEP::cm;

  if( mod )
    {
      MiceModule* moth = mod->mother();
     
      thickness = moth->dimensions().z();
      std::string material = mod->propertyString( "Material" );
      fibrePitch = mod->propertyDouble( "Pitch" );
      X0 = _run->miceMaterials->radiationLength( material );
      _volName = moth->fullName();
    }
  else
  {
    std::cerr << "ERROR! Unable to find the MiceModule for the station " << clust1->getStationNo() << " in tracker " << clust1->getTrackerNo() << " with plane " << clust1->getDoubletNo() << std::endl;
  }

  double xRes(0.0);
  double yRes(0.0);
  if (clust1->getStationNo() == 5 && clust1->getTrackerNo() == 0) {
    xRes = 0.3714;
    yRes = 0.3668;
  }
  else {
    xRes = 0.3626;
    yRes = 0.3584;
  }
  //Error with & w/o mcs for muon only

/*
  double mom = _run->DataCards->fetchValueDouble( "RecP" ) / MeV;
  double momsq = mom * mom;
  double massMuon = 105.658389 / MeV;
  double thickInRadLength = thickness / X0; 
  double s = 1 + ( 0.038 * log( thickInRadLength ) );
  double theta = 0.;

  if (_run->DataCards->fetchValueString("MultipleScatteringModel") != "none" ) 
  {
    theta = ( ( 13.6 / MeV ) / ( momsq / massMuon ) ) * sqrt( thickInRadLength ) * s;
  }  

  double deltaXY = theta * thickness / 2.;

  double sigX2 = xRes * xRes + deltaXY * deltaXY;
  double sigY2 = yRes * yRes + deltaXY * deltaXY;
*/

  double sigZ = 0.081; 

  posE.setX( xRes );
  posE.setY( yRes );
  posE.setZ( sigZ );

  // calculate chi2 by comparing distance of clust3 to new space point

  chi2 = ( _dist * _dist ) / 0.064;	//ME this needs to be done properly!

  // set Station and Tracker numbers

  stationNo = clust1->getStationNo();
  trackerNo = clust1->getTrackerNo();

  // store clusters

  doubletClusters.push_back( clust1 );
  doubletClusters.push_back( clust2 );
  doubletClusters.push_back( clust3 );

  // determine time

  time = ( clust1->getTime() + clust2->getTime() + clust3->getTime() ) / 3.0;

  timeE = 0;
  timeE += ( clust1->getTime() - time ) * ( clust1->getTime() );
  timeE += ( clust2->getTime() - time ) * ( clust2->getTime() );
  timeE += ( clust3->getTime() - time ) * ( clust3->getTime() );
  timeE = sqrt( timeE );


  timeRes = clust1->getTime() - time;

  // Monte Carlo truth values

  SciFiDoubletCluster* closest = clust1;

  if( fabs( clust2->getTruePosition().z() - pos.z() ) < fabs( closest->getTruePosition().z() - pos.z() ) )
    closest = clust2;

  if( fabs( clust3->getTruePosition().z() - pos.z() ) < fabs( closest->getTruePosition().z() - pos.z() ) )
    closest = clust3;

  mcPos = closest->getTruePosition();
  mcMom = closest->getTrueMomentum();
}

//duplet constructor

void SciFiSpacePoint::createCluster( SciFiDoubletCluster* clust1, SciFiDoubletCluster* clust2 )
{
  m_used = false;
  clusters = 2;
  _dist = 1e9;

  nPE = clust1->getNPE() + clust2->getNPE();

  // code assumes that clust1 and clust2 are in the same Station of the same Tracker

  pos = crossingPos( clust1, clust2 );

  //finding the micemodule this spacepoint is in

  const MiceModule* mod = NULL;
    
  for( unsigned int j = 0; ! mod && j < modules.size(); ++j )
    if( modules[j]->propertyExists( "Tracker", "int" ) && modules[j]->propertyInt( "Tracker" ) == clust1->getTrackerNo() &&
        modules[j]->propertyExists( "Station", "int" ) && modules[j]->propertyInt( "Station" ) == clust1->getStationNo() &&
        modules[j]->propertyExists( "Plane", "int" ) && modules[j]->propertyInt( "Plane" ) == clust1->getDoubletNo() )
	 mod = modules[j];

  double thickness = 1. * CLHEP::m;
  double fibrePitch = 1. * CLHEP::m;
  double X0 = 1. * CLHEP::cm;

  if( mod )
    {
      MiceModule* moth = mod->mother();
     
      thickness = moth->dimensions().z();
      std::string material = mod->propertyString( "Material" );
      fibrePitch = mod->propertyDouble( "Pitch" );
      X0 = _run->miceMaterials->radiationLength( material );
      _volName = moth->fullName();
    }

  double xRes(0.0); double yRes(0.0);

  if (clust1->getStationNo() == 5 && clust1->getTrackerNo() == 0) {
    if ((clust1->getDoubletNo() == 1.0 && clust2->getDoubletNo() == 2.0) ||  (clust1->getDoubletNo() == 2.0 && clust2->getDoubletNo() == 1.0)) {
      xRes = 0.4383;
      yRes = 0.5632;
    }
    if ((clust1->getDoubletNo() == 0.0 && clust2->getDoubletNo() == 2.0) || (clust1->getDoubletNo() == 2.0 && clust2->getDoubletNo() == 0.0)) {
      xRes = 0.6275;
      yRes = 0.3727;
    }
    if ((clust1->getDoubletNo() == 0.0 && clust2->getDoubletNo() == 1.0) || (clust1->getDoubletNo() == 1.0 && clust2->getDoubletNo() == 0.0)) {
      xRes = 0.4423;
      yRes = 0.5732;
    }
  }
  else {

  if ((clust1->getDoubletNo() == 1.0 && clust2->getDoubletNo() == 2.0) ||  (clust1->getDoubletNo() == 2.0 && clust2->getDoubletNo() == 1.0)) {
    xRes = 0.4314;
    yRes = 0.3584;
  }
  if ((clust1->getDoubletNo() == 0.0 && clust2->getDoubletNo() == 2.0) || (clust1->getDoubletNo() == 2.0 && clust2->getDoubletNo() == 0.0)) {
    xRes = 0.6197;
    yRes = 0.3602;
  }
  if ((clust1->getDoubletNo() == 0.0 && clust2->getDoubletNo() == 1.0) || (clust1->getDoubletNo() == 1.0 && clust2->getDoubletNo() == 0.0)) {
    xRes = 0.4327;
    yRes = 0.5623;
  }
  }

  //double xRes = fibrePitch * 0.794;
  //double yRes = fibrePitch * 1.217;

  //Error with & w/o mcs for muon only
/*
  double mom = _run->DataCards->fetchValueDouble( "RecP" ) / MeV;
  double momsq = mom * mom;
  double massMuon = 105.658389 / MeV;
  double thickInRadLength = thickness / X0; 
  double s = 1 + ( 0.038 * log( thickInRadLength ) );
  double theta = 0.;

  if (_run->DataCards->fetchValueString("MultipleScatteringModel") != "none" ) 
  {
    theta = ( ( 13.6 / MeV ) / ( momsq / massMuon ) ) * sqrt( thickInRadLength ) * s;
  }  

  double deltaXY = theta * thickness / 2.;

  double sigX2 = xRes * xRes + deltaXY * deltaXY;
  double sigY2 = yRes * yRes + deltaXY * deltaXY;
*/

  double sigZ = 0.081; 

  posE.setX( xRes );
  posE.setY( yRes );
  posE.setZ( sigZ );

  // calculate chi2 by comparing distance of clust3 to new space point

  chi2 = -1.0;

  // set Station and Tracker numbers

  stationNo = clust1->getStationNo();
  trackerNo = clust1->getTrackerNo();

  // store clusters

  doubletClusters.push_back( clust1 );
  doubletClusters.push_back( clust2 );

  // determine time

  time = ( clust1->getTime() + clust2->getTime() ) / 2.0;

  timeE = 0;
  timeE += ( clust1->getTime() - time ) * ( clust1->getTime() );
  timeE += ( clust2->getTime() - time ) * ( clust2->getTime() );
  timeE = sqrt( timeE );

  // Monte Carlo truth values

  SciFiDoubletCluster* closest = clust1;

  if( fabs( clust2->getTruePosition().z() - pos.z() ) < fabs( closest->getTruePosition().z() - pos.z() ) )
    closest = clust2;

  mcPos = closest->getTruePosition();
  mcMom = closest->getTrueMomentum();
}

void SciFiSpacePoint::operator=(SciFiSpacePoint right)
{
	pos = right.pos;
	posE = right.posE;
	time = right.time;
	timeE = right.timeE;
	nChans = right.nChans;
	nMuChans = right.nMuChans;
	nPE = right.nPE;
	chi2 = right.chi2;
	stationNo = right.stationNo;
	trackerNo = right.trackerNo;
	doubletClusters.assign(right.doubletClusters.begin(), right.doubletClusters.end());
        m_used = right.m_used;
  	mcPos = right.mcPos;
	mcMom = right.mcMom;
}

SciFiDoubletCluster* SciFiSpacePoint::getDoubletCluster (int i) const
{
	return doubletClusters[i];
}

int SciFiSpacePoint::goodTriplet(double cut) const
{
	if (chi2 > cut) return 0;

	int plane[4] = {0, 0, 0, 0};

	for (vector<SciFiDoubletCluster*>::const_iterator it = doubletClusters.begin();
	it != doubletClusters.end(); it++)
	{
		int tno = (*it)->getDoubletNo();
		plane[tno < 3 && tno >= 0 ? tno : 3] = 1;
	}

	return((chi2<cut)*plane[0]*plane[1]*plane[2]);
}


SciFiSpacePoint::~SciFiSpacePoint()
{
  miceMemory.addDelete( Memory::SciFiSpacePoint );
	
	doubletClusters.clear();
}

void SciFiSpacePoint::keep()
{
  if( doubletClusters.size() == 3 ) // only lock of clusters in a good triplet!
    for( unsigned int i = 0; i < doubletClusters.size(); ++i )
      doubletClusters[i]->setUsed();
}

bool  SciFiSpacePoint::good() const
{
  bool ok = false;

  if( doubletClusters.size() == 3 ) // a triplet
    ok = true;
  else if( doubletClusters.size() == 2 ) // a duplet
  {
    double r = sqrt( pos.x() * pos.x() + pos.y() * pos.y() );
    if( r < 180.0 )
      ok = true;
  }

  return ok;
}

void	SciFiSpacePoint::setUsed()
{ 
  m_used = true;

  for( unsigned int i = 0; i < doubletClusters.size(); ++i )
    doubletClusters[i]->setInTrack();
}

Hep3Vector	SciFiSpacePoint::crossingPos( SciFiDoubletCluster* c1, SciFiDoubletCluster* c2 )
{
  Hep3Vector d1 = c1->direction();
  Hep3Vector d2 = c2->direction();

  CLHEP::HepMatrix m1( 3, 3, 0 );

  m1[0][0] = ( c2->position() - c1->position() ).x();
  m1[0][1] = ( c2->position() - c1->position() ).y();
  m1[0][2] = ( c2->position() - c1->position() ).z();

  m1[1][0] = d2.x();
  m1[1][1] = d2.y();
  m1[1][2] = d2.z();

  m1[2][0] = ( d1.cross( d2 ) ).x();
  m1[2][1] = ( d1.cross( d2 ) ).y();
  m1[2][2] = ( d1.cross( d2 ) ).z();

  CLHEP::HepMatrix m2( 3, 3, 0 );

  m2[0][0] = ( c2->position() - c1->position() ).x();
  m2[0][1] = ( c2->position() - c1->position() ).y();
  m2[0][2] = ( c2->position() - c1->position() ).z();

  m2[1][0] = d1.x();
  m2[1][1] = d1.y();
  m2[1][2] = d1.z();

  m2[2][0] = ( d1.cross( d2 ) ).x();
  m2[2][1] = ( d1.cross( d2 ) ).y();
  m2[2][2] = ( d1.cross( d2 ) ).z();

  double t1 = m1.determinant() / pow( ( d1.cross( d2 ) ).mag(), 2. );
  double t2 = m2.determinant() / pow( ( d1.cross( d2 ) ).mag(), 2. );

  Hep3Vector p1 = c1->position() + t1 * d1;
  Hep3Vector p2 = c2->position() + t2 * d2;

  Hep3Vector pos = ( p1 + p2 ) / 2.;

  return pos;
}

bool	sortPoints( const SciFiSpacePoint * left, const SciFiSpacePoint* right )
{
  return( left->getPos().z() < right->getPos().z() );
}
