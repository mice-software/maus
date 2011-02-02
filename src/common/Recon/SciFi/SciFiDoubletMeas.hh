/* 
   SciFiDoubletMeas - a measurement in the Kalman fit based on a a single Doublet cluster in the Sci Fi
*/

#ifndef SCIFIDOUBLETMEAS_HH
#define SCIFIDOUBLETMEAS_HH

#include "CLHEP/Vector/ThreeVector.h"
#include <iostream>
#include "SciFiDoubletCluster.hh"


/**
* A SciFiDoubletMeas is a class used to describe a measurement based on a cluster of
* fibres, that is used in the Kalman Helix fit.
* This measurement class takes a reconstructed SciFiDoubletCluster and implements several
* methods of the MeasurementB class (from the Kalman package) such that it can be added
* to a trajectory and a Helix fit applied.
* The measurement determines the position and orientation of the central fibre in the cluster
* and passes this information and the covariance of the measurement to the Kalman track fit
*/

/*
class SciFiDoubletMeas : public MeasurementB // so that a doublet cluster can be used as a measurement in the track fit!
{
public:

	//! Constructor taking a pointer to a SciFiDoubletCluster
	SciFiDoubletMeas( SciFiDoubletCluster* );

	//! Default destructor
	~SciFiDoubletMeas();

	//! return the cluster that this measurement represents
	const SciFiDoubletCluster*	cluster() const { return m_clust; };

	// functions needed due to inheritance from MeasurementB:

	//! return the measurement hyper vector
   	const HyperVector& measHV() const;

	//! return the type of measurement
    	string type() const;

	//! return a reference to the measurement surface this measurement is in
    	const ISurface& surface() const;

	//! dump some information about this measurement to the standard output
    	void info() const;

	//! set the measurement surface of this measurement
    	void setSurface (ISurface*);

	//! set the measurement surface of this measurement
    	void setSurface( const ISurface& );

	//! return the name of the volume that this measurement is in
    	string volumeName() const;

	//! set the name of the volume that this measurement is in
    	void   setVolumeName( string );

	//! dump some information about this measurement to the standard output
    	void   info( int ) const;

private:

	SciFiDoubletCluster*		m_clust;

	// for the MeasurementB:

  	ISurface* m_surface;
   	string m_name;
   	HyperVector m_hv;
};
*/

#endif
