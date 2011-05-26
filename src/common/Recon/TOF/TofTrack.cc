//TofTrack.cc

#include<iostream>
using namespace std;

#include"Recon/TOF/TofTrack.hh"
#include"Recon/TOF/TofSpacePoint.hh"
#include"Interface/SpecialHit.hh"
#include "Interface/MICEUnits.hh"

TofTrack::TofTrack() {
	truth = false;
	firstStationNo = 0;
	secondStationNo = 0;
	good = false;
	firstPosition.set(0.,0.,0.);
	secondPosition.set(0.,0.,0.);
	firstTime = 0.;
	secondTime = 0.;
}

TofTrack::TofTrack( TofSpacePoint const * first, TofSpacePoint const * second, double particleMass ) {
	truth = false;

	mass = particleMass;

	if ( first->position().z() < second->position().z() ) {
		firstSpacePoint = first;
		secondSpacePoint = second;
	}
	else {
		firstSpacePoint = second;
		secondSpacePoint = first;
	}

	firstStationNo = first->station();
	secondStationNo = second->station();

	if ( firstStationNo != secondStationNo ) good = true;
	else good = false;

	firstPosition = firstSpacePoint->position();
	secondPosition = secondSpacePoint->position();
	firstTime = firstSpacePoint->time();
	secondTime = secondSpacePoint->time();

}

TofTrack::TofTrack( SpecialHit const * first, SpecialHit const * second, double particleMass ) {

	truth = true;

	mass = particleMass;

	if ( first->GetPosition().z() < second->GetPosition().z() ) {
		firstHit = first;
		secondHit = second;
	}
	else {
		firstHit = second;
		secondHit = first;
	}

	firstStationNo = first->GetStationNo();
	secondStationNo = second->GetStationNo();

	if ( firstStationNo != secondStationNo ) good = true;
	else good = false;

	firstPosition = firstHit->GetPosition();
	secondPosition = secondHit->GetPosition();
	firstTime = firstHit->GetTime();
	secondTime = secondHit->GetTime();

}

double TofTrack::GetTimeOfFlight() const { 
	return GetSecondTime() - GetFirstTime(); 
}

Hep3Vector TofTrack::GetDisplacement() const { 
	return GetSecondPosition() - GetFirstPosition(); 
}

double TofTrack::GetMiddleMomentum() const { 
	return GetDisplacement().mag() * mass / sqrt( pow( GetTimeOfFlight(), 2 ) * CLHEP::c_squared - pow( GetDisplacement().mag(), 2 ) ); 
}

double TofTrack::GetBeta() const 
{
  double beta = GetDisplacement().mag()/( GetTimeOfFlight() * CLHEP::c_light);
  return beta;
}

double TofTrack::GetGamma() const 
{
  double gamma = 1./sqrt(1.- pow( GetBeta(), 2));
  return gamma;
}

CLHEP::Hep3Vector TofTrack::GetInitialMomentum() const { 
	return CLHEP::Hep3Vector( 0., 0., GetMiddleMomentum() ); 
}

CLHEP::Hep3Vector TofTrack::GetFinalMomentum() const { 
	return CLHEP::Hep3Vector( 0., 0., GetMiddleMomentum() ); 
}


