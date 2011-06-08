/* 
** Written for the MICE proposal simulation
** Ed McKigney - Oct 9 2002
** Rewritten for the MICE simulation
** Ed McKigney - Mar 6 2003
*/

#ifndef SCIFISPACEPOINT_HH
#define SCIFISPACEPOINT_HH

#include "CLHEP/Vector/ThreeVector.h"
#include "Calib/SciFiReconstructionParams.hh"
#include <iostream>
#include <vector>

#include "Interface/Memory.hh" 

using std::vector;

class SciFiDoubletCluster;
class MICERun;

/**
* The SciFiSpacePoint class defines a space point in 3D, determined from the crossing
* of three SciFiDoubletClusters (one for each projection ina given station).
*/

class SciFiSpacePoint
{
public:
	//! Default constructor
	SciFiSpacePoint();

        // Triplet constructor
	SciFiSpacePoint( MICERun*, SciFiDoubletCluster*, SciFiDoubletCluster* , SciFiDoubletCluster* );

        // Duplet constructor
        SciFiSpacePoint( MICERun*, SciFiDoubletCluster*, SciFiDoubletCluster* );

	//! Default destructor
	~SciFiSpacePoint();		

        // Triplet cluster initialisation
	void createCluster( SciFiDoubletCluster*, SciFiDoubletCluster* , SciFiDoubletCluster* );

        // Duplet cluster intialisation
        void createCluster( SciFiDoubletCluster*, SciFiDoubletCluster* );

	//! Assignment operator
	void operator=(const SciFiSpacePoint);

	//! Less than operator - compares the position of the space points in Z
	int operator<(const SciFiSpacePoint x) const {return pos.z()<x.pos.z();}

	//! Is equal to operator - compares the position of the space points in z
	bool operator==(const SciFiSpacePoint x) const {return pos.z()==x.pos.z();}

	//! Greater than operator - compares the position of the space points in z
	int operator>(const SciFiSpacePoint x) const {return pos.z()>x.pos.z();}

	//! return the position of this space point
	CLHEP::Hep3Vector getPos() const    {return pos;}

	//! return the uncertainty in the position of this space point
	CLHEP::Hep3Vector getPosE() const   {return posE;}

	CLHEP::Hep3Vector getTruePosition() const { return mcPos; }

	CLHEP::Hep3Vector getTrueMomentum() const { return mcMom; }

	//! return the time of the hits in this space point
	double getTime() const       {return time;}

	//! return the uncertainty in the time of this hits in this space point
	double getTimeE() const      {return timeE;}

        double getTimeRes() const	{ return timeRes; }

	//! return the total number of channels in this space point
	int getNChans() const        {return nChans;}

	//! return the number of channels in this space point associated to a muon
	int getNMuChans() const      {return nMuChans;}

	//! return the number of SciFiDoubletClusters in this space point
	int getNClusters() const     {return doubletClusters.size(); };

	//! return the total number of PE in this space point
	double getNPE() const        {return nPE;}

	//! return the internal chi2 of this space point, this measures how well the three clusters agree
	double getChi2() const       {return chi2;}

	//! return the station number of this space point
	int getStationNo() const     {return stationNo;}

	//! return the tracker number of this space point
	int getTrackerNo() const     {return trackerNo;}

	//! return 1 if this is a good triplet of clusters
	int goodTriplet(double) const;

	//! return one of the clusters that forms this space point
	SciFiDoubletCluster*  getDoubletCluster(int) const;

        //! return the number of clusters making up the space point
        int   getNumClusters(){return clusters;};

	//! dump some information about this space point to the standard output
	void Print() const 	     {std::cout << pos << " " << chi2 << std::endl;}
	
 	// return true if this space point is good and can be kept
	bool good() const;
	
	//! decided to keep this point, so mark the clusters in it accordingly
	void keep();

	//! return true if this point is used in a track
        bool used() const{ return m_used; };

        //! specify that the point has been used
        void setUsed();

	//! return the internal residual of the triplet if it is one
	double	internalResidual() const		{ return _dist; };

	//! return the name of the MiceModule that this point is in
	std::string	volumeName() const		{ return _volName; };

	void		setRun( MICERun* _r ) 		{ _run = _r; };

private:

	CLHEP::Hep3Vector	crossingPos( SciFiDoubletCluster*, SciFiDoubletCluster* );

	CLHEP::Hep3Vector pos, posE;
	double time, timeE, timeRes;
	int nChans, nMuChans;
	double nPE;
	double chi2;
	int stationNo, trackerNo;
	vector<SciFiDoubletCluster*> doubletClusters;
	bool m_used;
        int clusters;
	double	_dist;	//ME need to add this to persistency!
	MICERun*	_run;

        CLHEP::Hep3Vector mcPos, mcMom;

	std::string	_volName;	//Name of the MiceModule that this point is in
};

bool sortPoints( const SciFiSpacePoint*, const SciFiSpacePoint* );

#endif
