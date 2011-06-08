/* 
** Written for the MICE proposal simulation
** Ed McKigney - Oct 9 2002
** Rewritten for the MICE proposal simulation
** Ed McKigney - Mar 6 2003
*/

#ifndef SCIFIDOUBLETCLUSTER_HH
#define SCIFIDOUBLETCLUSTER_HH

#include "CLHEP/Vector/ThreeVector.h"
#include <iostream>
#include "Calib/SciFiReconstructionParams.hh"
#include "SciFiSpacePoint.hh"

#include "Interface/SciFiDigit.hh"

#include "Interface/Memory.hh" 

#include "Config/MiceModule.hh"

/**
* SciFiDoubletCluster describes a reconstructed cluster of hits in the fibres of a single
* layer (doublet) of scintillating fibre.
* A doubletcluster is defined by the station, tracker, and plane containing the fibres,
* the channel numbers (or fibre numbers) of the fibres in the cluster as well as the
* amount of light reconstructed from these fibres.
*/

class SciFiDoubletCluster
{
public:
	//! Default constructor
	SciFiDoubletCluster();

	//! Constructor taking information about the double cluster (i.e. station, plane, position, etc)
	SciFiDoubletCluster( SciFiDigit* );

        SciFiDoubletCluster( SciFiDigit*, SciFiDigit* );

	//! Default destructor
	~SciFiDoubletCluster() { miceMemory.addDelete( Memory::SciFiDoubletCluster ); };

	// copy contents of one cluster into this one
        void	copy( const SciFiDoubletCluster& );

	// add a digit to this cluster

        void	addDigit( SciFiDigit* );

	const MiceModule*		module() const			{ return _module; };

	void			setModule( const MiceModule* mod )	{ _module = mod; };

	//! assignment operator
	void operator=(const SciFiDoubletCluster);

	//! less than operator - compares the station and tracker numbers of two doublet clusters
	int operator<(const SciFiDoubletCluster &dc) const { return getNPE() < dc.getNPE(); }

	//! greater than operator - compares the station and tracker numbers of two doublet clusters
	int operator>(const SciFiDoubletCluster &dc) const { return getNPE() > dc.getNPE(); }

	Hep3Vector	position() const		{ return _position; };

	Hep3Vector	direction() const		{ return _direction; };

	void		setPosition( Hep3Vector pos )	{ _position = pos; };

	void		setDirection( Hep3Vector dir )	{ _direction = dir; };

        SciFiDigit*	seedDigi() const { return seed; };

        std::vector<SciFiDigit*>	allDigits() const { return digits; };

	//! return the time of the hit in this cluster
	double getTime() const    {return time;}

	//! return the number of channels (fibres) with a hit from a muon in this cluster
	double getNMuChans() const {return nMu;}

	//! return the number of channels (fibres) with a hit in this cluster
	double getNChans() const   {return n_chan;}

	//! return the total number of PE in this cluster
	double getNPE() const     {return nPE;}

	void	setNPE( double pe )	{ nPE = pe; };

	double getAdc() const { return seed->getAdcCounts(); };

	//! return the unweighted average channel number of this cluster
	double getChanNoU() const    {return aveChanNoWeighted;}

	//! return the weighted average channel number of this cluster
	double getChanNoW() const    {return aveChanNoUnweighted;}

	//! return the lowest channel number of this cluster
	int getLowerChanNo() const{return lowerChanNo;}

	//! return the highest channel number of this cluster
	int getUpperChanNo() const{return upperChanNo;}

	//! return the doublet number - this is the same as the plane or projection (U, X, V)
	int getDoubletNo() const  {return doubletNo;}

	//! return the station number
	int getStationNo() const  {return stationNo;}

	//! return the tracker number
	int getTrackerNo() const  {return trackerNo;}

	//! return the true position of the particle in this cluster from the Monte Carlo
	Hep3Vector getTruePosition() const {return truePos;}

	//! return the true momentum of the particle in this cluster from the Monte Carlo
	Hep3Vector getTrueMomentum() const {return trueMom;}

	//! return true if the SciFiDoubletCluster passed as an argument is a neighbour to this cluster
	bool neighbor (SciFiDoubletCluster) const;

	//! return true if the SciFiDoubletCluster passed as an argument makes a valid space point candidate with this cluster 
	bool spacePointCandidate (SciFiDoubletCluster) const;

	//! merge the SciFiDoubletCluster passed as an argument with this cluster
	void merge (SciFiDoubletCluster);

        // keep this cluster, calculate the correct time and particle ID, etc...
        void keep();

	//! dump some information about this cluster
	void Print() const
	{
		std::cout << "-----------------------------";
		std::cout << "aveChanNoWeighted = " << aveChanNoWeighted;
		std::cout << "aveChanNoUnweighted = " << aveChanNoUnweighted;
		std::cout << "(lower =  " << lowerChanNo ;
		std::cout << ", upper =  " << upperChanNo << " )" ;
		std::cout << " : " << doubletNo << " " << stationNo << " ";
		std::cout << trackerNo << " : " << nMu << " " << time;
		std::cout << " " << nPE << std::endl;
	}

	//! return true if this cluster has been used in a space point
	bool	used() const { return m_used; };

	//! tag this cluster as being used in a space point
	void	setUsed() { m_used = true; };

	bool	inTrack() const { return m_in_track; };

      	void	setInTrack() { m_in_track = true; };

private:

	SciFiDigit*	seed;
	SciFiDigit*	neighbour;
	int n_chan;

        std::vector<SciFiDigit*> digits;

	double aveChanNoWeighted, aveChanNoUnweighted; 
	int lowerChanNo, upperChanNo;

	int doubletNo, stationNo, trackerNo;

	double nMu;
	double time;
	double nPE;

	Hep3Vector truePos, trueMom;

	Hep3Vector 	_position;
	Hep3Vector	_direction;

	bool m_used;

	bool m_in_track;

	const MiceModule* _module;
};

#endif
