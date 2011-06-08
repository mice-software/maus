/* 
** Written for the MICE proposal simulation
** Some examples from the ROOT project at CERN used
** Ed McKigney - Oct 9 2002
** Rewritten
** Ed McKigney - Mar 6 2003
*/

#include <vector>
#include "CLHEP/Vector/ThreeVector.h"
#include "Interface/SciFiHit.hh"
#include "Interface/SciFiDigit.hh"
#include "SciFiDoubletCluster.hh"
#include "SciFiSpacePoint.hh"

using std::vector;

SciFiDoubletCluster::SciFiDoubletCluster()
{
	miceMemory.addNew( Memory::SciFiDoubletCluster );
	
	aveChanNoWeighted = -1;
	aveChanNoUnweighted = -1;
	lowerChanNo = -1;
	upperChanNo = -2;
	doubletNo = -1;
	stationNo = -1;
	trackerNo = -1;
	nMu = 0;
	time = 0;
	nPE = 0;
	truePos = Hep3Vector(0,0,0);
	trueMom = Hep3Vector(0,0,0);
	m_used = false;
  	m_in_track = false;
	n_chan = 0;
	_module = NULL;
}

SciFiDoubletCluster::SciFiDoubletCluster( SciFiDigit* digi )
{
  miceMemory.addNew( Memory::SciFiDoubletCluster );
  
  seed = digi;
  neighbour = NULL;

  digits.push_back( seed );

  digi->setUsed();

  nPE = seed->getNPE();

  if( seed->mcHit() )
  {
    truePos = seed->mcHit()->GetPos();
    trueMom = seed->mcHit()->GetMom();
  }

  trackerNo = seed->getTrackerNo();
  stationNo = seed->getStationNo();
  doubletNo = seed->getPlaneNo();
  lowerChanNo = (int) seed->getChanNo();
  upperChanNo = (int) seed->getChanNo();
  aveChanNoWeighted = seed->getChanNo();
  aveChanNoUnweighted = seed->getChanNo();

  time = seed->getTdcCounts();

  m_used = false;
  m_in_track = false;
  n_chan = 1;
}

void SciFiDoubletCluster::copy( const SciFiDoubletCluster& m_cluster )
{
  seed = m_cluster.seedDigi();
  neighbour = NULL;                     //ME this is not used anymore
  digits = m_cluster.allDigits();
  aveChanNoWeighted = m_cluster.getChanNoU();
  aveChanNoUnweighted = m_cluster.getChanNoW();
  lowerChanNo = m_cluster.getLowerChanNo();
  upperChanNo = m_cluster.getUpperChanNo();
  doubletNo = m_cluster.getDoubletNo();
  stationNo = m_cluster.getStationNo();
  trackerNo = m_cluster.getTrackerNo();
  nMu = m_cluster.getNMuChans();
  time = m_cluster.getTime();
  truePos = m_cluster.getTruePosition();
  trueMom = m_cluster.getTrueMomentum();
  m_used = m_cluster.used();
  m_in_track = m_cluster.inTrack();
  n_chan = m_cluster.n_chan;
  truePos = m_cluster.truePos;
  trueMom = m_cluster.trueMom;
  _position = m_cluster._position;
  _direction = m_cluster._direction;
  nPE = m_cluster.nPE;
}

SciFiDoubletCluster::SciFiDoubletCluster( SciFiDigit* digi1, SciFiDigit* digi2 )
{
  miceMemory.addNew( Memory::SciFiDoubletCluster );
  
  seed = digi1;
  neighbour = digi2;

  digits.push_back( seed );
  digits.push_back( neighbour );

  m_used = false;
  m_in_track = false;

  nPE = seed->getNPE() + neighbour->getNPE();

  trackerNo = seed->getTrackerNo();
  stationNo = seed->getStationNo();
  doubletNo = seed->getPlaneNo();
  lowerChanNo = (int) seed->getChanNo();
  upperChanNo = (int) seed->getChanNo();
  aveChanNoWeighted = seed->getChanNo();
  aveChanNoUnweighted = seed->getChanNo();

  nMu = 0;
  time = seed->getTdcCounts();

  m_used = false;
  m_in_track = false;
}

void	SciFiDoubletCluster::addDigit( SciFiDigit* digi )
{
  digits.push_back( digi );

  digi->setUsed();

  nPE += digi->getNPE();
  n_chan++;
}

void    SciFiDoubletCluster::keep()
{
  seed = digits[0];
  time = seed->getTdcCounts();

  for( unsigned int i = 0; i < digits.size(); ++i )
    if( digits[i]->getTdcCounts() < time )
    {
      seed = digits[i];
      time = seed->getTdcCounts();
    }
}

void SciFiDoubletCluster::operator=(SciFiDoubletCluster right)
{
	aveChanNoWeighted = right.aveChanNoWeighted;
	aveChanNoUnweighted = right.aveChanNoUnweighted;
	lowerChanNo = right.lowerChanNo;
	upperChanNo = right.upperChanNo;
	doubletNo = right.doubletNo;
	stationNo = right.stationNo;
	trackerNo = right.trackerNo;
	nMu = right.nMu;
	time = right.time;
	nPE = right.nPE;
	truePos = right.truePos;
	trueMom = right.trueMom;
	seed = right.seed;
	neighbour = right.neighbour;
        digits = right.digits;
	_position = right._position;
   	_direction = right._direction;
}

bool SciFiDoubletCluster::neighbor(SciFiDoubletCluster nn) const
{
        bool neigh = false;

	// Merge overlapping clusters of channels
	if (doubletNo == nn.doubletNo && stationNo == nn.stationNo 
		&& trackerNo == nn.trackerNo )
        {
	  if (nn.lowerChanNo-1 <= upperChanNo && nn.lowerChanNo >= lowerChanNo)
		neigh = true;
	  else if (nn.upperChanNo+1 >= lowerChanNo && nn.upperChanNo <= upperChanNo)
		neigh = true;
        }
	 
	return neigh;
}

bool SciFiDoubletCluster::spacePointCandidate(SciFiDoubletCluster nn) const
{
        bool point = false;

	if (nn.stationNo == stationNo && nn.trackerNo == trackerNo
		&& nn.doubletNo != doubletNo)
          point = true;

	return point;
}

void SciFiDoubletCluster::merge(SciFiDoubletCluster tm)
{
	// Don't merge clusters that aren't neighbors!!
	if (!neighbor(tm)) return;

	aveChanNoWeighted =
	  (aveChanNoWeighted*nPE+tm.aveChanNoWeighted*tm.nPE)/(nPE+tm.nPE);
	nPE = nPE+tm.nPE;

	lowerChanNo = lowerChanNo < tm.lowerChanNo ? lowerChanNo:tm.lowerChanNo;
	upperChanNo = upperChanNo > tm.upperChanNo ? upperChanNo:tm.upperChanNo;
	aveChanNoUnweighted = ((double)upperChanNo + (double)lowerChanNo)/2;

	time = time < tm.time ? time : tm.time;
	nMu += tm.nMu;
}

