// SciFiKalTrack.hh
//
// M.Ellis 20/10/2003
//
// Takes a normal SciFiTrack and refits it using the Kalman Filter

#ifndef SCIFIKALTRACK_HH
#define SCIFIKALTRACK_HH

#include "CLHEP/Vector/ThreeVector.h"
#include <iostream>
#include <vector>

#include "Recon/SciFi/SciFiTrack.hh"

#include "Recon/Global/MouseTrack.hh"

#include "Recon/SciFi/KalmanSeed.hh"

#include "Interface/Memory.hh" 

#include "recpack/RecpackManager.h"

/**
* A SciFiKalTrack is a track in the Scintillating Fibre tracker that has been fitted
* using the Kalman track fitting package.
* Depending on which #define is uncommented in the .cc file, the track fit will be
* performed with 3D space points (from the SciFiSpacePoint) or with fibres, using
* the SciFiDoubletMeas class.
* The seed state for the track fit is determined from the fitted parameters returned
* by the SciFiTrack.
*/

class SciFiKalTrack : public MouseTrack
{
public:

        SciFiKalTrack();

	//! constructor taking a vector of SciFiSpace points, for use with a magnetic field
	SciFiKalTrack( std::vector<SciFiSpacePoint*>&, KalmanSeed );

	//! constructor taking a vector of SciFiSpace points, for use with no magnetic field
	SciFiKalTrack( std::vector<SciFiSpacePoint*>&, double, double );

	//! Default destructor
	~SciFiKalTrack();

	//! return the pointer to the SciFiTrack that acted as seed
	const SciFiTrack*	sciFiTrack() const;

        // return the tracker number that this track is in (0 = input, 1 = output)
        int			tracker() const { return m_tracker; };

	const Trajectory*	trajectory() const { return m_it; };

	// add a point to this track
	void			addPoint( SciFiSpacePoint* );

	// chi2 if this point is added
	double			chi2AddPoint( SciFiSpacePoint* );

	//! return the chi2 of this track divided by the number of degrees of freedom
	double			chi2Dof() const		{ return m_chi2; };

	//! return an IState corresponding to the extrapolation of this track to a plane in Z
	State*			extrapToZ( double ) const;

	//! return the position of the extrapolation of this track to a plane in Z
	Hep3Vector		positionAtZ( double ) const;

	//! return the uncertainties of the extrapolation of this track to a plane in Z
	Hep3Vector		posErrorAtZ( double ) const;

	//! return the momentum of track at the extrapolation to a plane in Z
	Hep3Vector		momentumAtZ( double ) const;

	//! return the uncertainites in the momentum of the track at the extrapolation to a plane in Z
	Hep3Vector		momErrorAtZ( double ) const;

        //! return an estimate for the time the particle passed a plane in Z
        double                  timeAtZ( double ) const;

        unsigned int		numPoints() const { return m_points.size(); };

        SciFiSpacePoint*	point( int i ) const { return m_points[i]; };

        void			refit();

	void			keep();

        std::string		model() const		{ return _model; };

	KalmanSeed                    m_seed;

private:

	int				m_tracker;

	const SciFiTrack* 		m_track;

	vector<SciFiSpacePoint*>	m_points;

	measurement_vector		m_measurements;

	Trajectory*			m_it;

	Recpack::RecpackManager*	m_manager;

	double				m_chi2;

        std::string			_model;
};

#endif
