/* 
** Written for the MICE proposal simulation
** Ed McKigney - Aug 21 2002
** Rewritten for the MICE simulation
** Ed McKigney - Mar 9 2003
*/

#ifndef SCIFITRACK_HH
#define SCIFITRACK_HH

#include "CLHEP/Vector/ThreeVector.h"
#include <iostream>
#include <vector>

#include "SciFiSpacePoint.hh"
#include "SciFiDoubletCluster.hh"
#include "Calib/SciFiReconstructionParams.hh"

#include "Interface/Memory.hh" 

/**
* The SciFiTrack is a description of a track in a single tracker that takes
* 3D space point information from the SciFiSpacePoint class and performs a
* helix fit using MINUIT (interfaced through ROOT).
*/

class SciFiTrack
{
public:

	//! Default constructor
	SciFiTrack();

	//! Default destructor
	~SciFiTrack();

	//! assignment operator
	void operator=(SciFiTrack);

	//! return the x position of the center of the circle which describes the helix
	double getX0() const   		{return track_x0;}

	//! return the y position of the center of the circle which describes the helix
	double getY0() const  		{return track_y0;}

	//! return the radius of the circle which describes the helix
	double getR()  const 		{return track_r;}

	//! return alpha  - need to add here definition of this helix parameter
	double getAlpha() const 	{return track_alpha;}

	//! return the RMS of the helix parameter alpha
	double getAlphaRMS() const	{return track_rms_alpha;}

	//! return phi0 - need to add here definition of this helix parameter
	double getPhi0() const		{return track_phi0;}

	//! return the transverse momentum of the track in MeV/c
	double getPt() const   		{return track_pt;}

	//! return the i,j component of the covariance matrix of the track fit from MINUIT
	double getCov(int i, int j) const	{return track_cov[i][j];}

	//! return the chi2 of the track fit from MINUIT
	double getChi2() const		{return track_chi2;}

	//! return the status of the track fit
	int getStatus() const  		{return recStatus;}

	//! return the number of space points in the track
	int getNPoints() const 		{return track_npoints;}

	//! return the number of channels in all the space points in the track
	int getNChans() const  	{return nChans;}

	//! return the number of channels in all the space points in the track from a muon
 	int getNMuChans() const {return nMuChans;}

	//! return the true value of X0 from the Monte Carlo
	double getMCX0() const 		{return mc_track_x0;}

	//! return the true value of Y0 from the Monte Carlo
	double getMCY0() const 		{return mc_track_y0;}

	//! return the true radius of the circle from the Monte Carlo
	double getMCR() const  		{return mc_track_r;}

	//! return the true value of alpha from the Monte Carlo
	double getMCAlpha() const	{return mc_track_alpha;}

	//! return the true value of phi0 from the Monte Carlo
	double getMCPhi0() const	{return mc_track_phi0;}

	//! return the true transverse momentum from the Monte Carlo in MeV/c
	double getMCPt() const 		{return mc_track_pt;}

	//! return the number of space points in this track
	int    getNSpacePoints() const	{return spacePoints.size();}

	//! return the ith space point in this track
	SciFiSpacePoint& getSpacePoint(int i) const {return *(spacePoints[i]);}
	      
	//! return the position of the extrapolation of this track to a plane in z, the integer determines if the reconstrcuted (0) or true (1) track parameters should be used
	Hep3Vector GetPos(double, int) const;

	//! return the momentum of the track at an extrapolation to a plane in z, the integer determines if the reconstructed (0) or true (1) track parameters should be used
	Hep3Vector GetMom(double, int) const;

	//! return the energy of the particle
	double GetEnergy() const;

	//! return the momentum of the particle
	double GetMomentum() const;

	//! not sure what this one does...
	double GetMomentum(double, double, double);

	//! return the chi2 of the match between the extrapolation of this track to the SciFiSpace Point
	double extrapolate( const SciFiSpacePoint& );

	//! add a SciFiSpacePoint to this track
	void addPoint(SciFiSpacePoint& );

	//! from 3 SpacePoints, determine the helix track parameters directly
	void solve3(SciFiReconstructionParams &);

	//! fit the track using MINUIT
	void fit(SciFiReconstructionParams &);

	//! determine if the SciFiTrack passed as an argument is a subset of this track
	int subset(SciFiTrack);

	//! perform some analysis of the Monte Carlo
	void ProcessMC(SciFiReconstructionParams &);

	//! dump some information about this track to the standard output
	void Print() const
	{
		std::cout << "--------" << std::endl;
		std::cout << "npoint = " << track_npoints << " " << track_chi2 
			  << " " << recStatus << std::endl;
		for (unsigned int i=0; i<spacePoints.size(); i++)
			spacePoints[i]->Print();
		std::cout << "--------" << std::endl;
	}
private:
	double GetMomentumLoss(double);

	std::vector<SciFiSpacePoint*> spacePoints;

	// Reconstruced track parameters
	double track_x0, track_y0, track_r; 
	double track_alpha, track_phi0, track_pt;
	double track_rms_alpha, track_chi2;
	double eLossCorr;
	double track_cov[5][5];
	int track_npoints;

        double z_ref; //relative offset used for local coordinate system 

	// MC track parameters
	double mc_track_x0, mc_track_y0, mc_track_r; 
	double mc_track_alpha, mc_track_phi0, mc_track_pt;

	int nChans;
	int nMuChans;

	int recStatus;
};

#endif
