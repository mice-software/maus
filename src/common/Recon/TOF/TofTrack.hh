//TofTrack.hh

#ifndef TOFTRACK_HH
#define TOFTRACK_HH

#include<math.h>
#include"Recon/TOF/TofSpacePoint.hh"
#include"Interface/SpecialHit.hh"
#include"CLHEP/Vector/ThreeVector.h"
#include"CLHEP/Units/PhysicalConstants.h"

const double muon_mass = 105.6583692 * CLHEP::MeV;

class TofTrack {

public :

	TofTrack();
	TofTrack( TofSpacePoint const *, TofSpacePoint const *, double m = muon_mass );
	TofTrack( SpecialHit const *, SpecialHit const *, double m = muon_mass );

	bool 		IsGood() 		const { return good; };
	bool 		IsMonteCarloTruth() 	const { return truth; };

	TofSpacePoint 	const * GetFirstSpacePoint() 		const { return firstSpacePoint; };
	TofSpacePoint 	const * GetSecondSpacePoint() 	const { return secondSpacePoint; };
	SpecialHit 	const * GetFirstHit() 		const { return firstHit; };
	SpecialHit 	const * GetSecondHit() 		const { return secondHit; };

	double 		GetMass() 		const { return mass; };
	void 		SetMass( double Mass ) 	{ mass = Mass; };

	int		GetFirstStationNo() 	const { return firstStationNo; };
	int		GetSecondStationNo() 	const { return secondStationNo; };

	Hep3Vector 	GetFirstPosition() 	const { return firstPosition; };
	Hep3Vector 	GetSecondPosition() 	const { return secondPosition; };
	double 		GetFirstTime()		const { return firstTime; };
	double 		GetSecondTime()		const { return secondTime; };

	void		SetFirstPosition(  Hep3Vector p ) 	{ firstPosition  = p; };
	void		SetSecondPosition( Hep3Vector p )	{ secondPosition = p; };
	void		SetFirstTime(  double t )		{ firstTime  = t; };
	void		SetSecondTime( double t )		{ secondTime = t; };

	double 		GetTimeOfFlight() 	const;
	Hep3Vector 	GetDisplacement() 	const;

	double 		GetMiddleMomentum() 	const;
	double		GetBeta()            const;
	double		GetGamma()            const;
	CLHEP::Hep3Vector	GetInitialMomentum() 	const;
	CLHEP::Hep3Vector 	GetFinalMomentum() 	const;

private :

	bool 			good;
	bool 			truth;

	double			mass;

	TofSpacePoint 		const * firstSpacePoint;
	TofSpacePoint 		const * secondSpacePoint;
	SpecialHit 		const * firstHit;
	SpecialHit 		const * secondHit;

	int 			firstStationNo;
	int 			secondStationNo;

	CLHEP::Hep3Vector 	firstPosition;
	CLHEP::Hep3Vector 	secondPosition;
	double 			firstTime;
	double 			secondTime;

};

#endif
