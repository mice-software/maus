// MAUS WARNING: THIS IS LEGACY CODE.
//Multipole aperture
//Currently - builds a box inside another box, with straight or curved geometry
//Eventually - add arbitrary number of poles and square or circular cross sections
//But can't figure out G4Torus ... gives odd segv ... which is blocking

#ifndef Multipole_h
#define Multipole_h 1

#include "Geant4/G4VSolid.hh"
#include "Config/MiceModule.hh"

class MultipoleAperture
{
public:
	MultipoleAperture(MiceModule * mod);
	~MultipoleAperture();
	G4VSolid * getMultipoleAperture() {return _volume;}

private:
	G4VSolid * BuildBox();
	G4VSolid * BuildCurvedBox();
	G4VSolid * AddPole(int poleNumber, G4VSolid* inputSolid, bool isCurved);
	G4VSolid * BuildTorus() {return NULL;}//CRASHES - DONT USE YET  )
	G4VSolid * BuildPoles() {return NULL;}//CRASHES - DONT USE YET  ) Awaiting bug in G4Torus

	//If multipole has a rectangular cross section, vertical height and horizontal width of inner and outer walls  
	double _innerHeight;
	double _innerWidth;
	double _outerHeight;
	double _outerWidth;
	//If multipole aperture has a circular cross section, radius of inner and outer walls
	double _innerRadius;
	double _outerRadius;

	//Radius of curvature of aperture
	double _referenceCurvature;
	bool   _isCurved;

	//Length of aperture along central trajectory
	double _length;
	//Poles are cylinders or tori distributed at regular intervals around a circle centred on central trajectory
	//Radius of pole centers relative to central trajectory
	double _poleCenterRadius;
	//Radius of pole tips relative to pole centers
	double _poleTipRadius;
	//Number of poles
	int    _numberOfPoles;

	MiceModule* _mod;
	G4VSolid*   _volume;
};

#endif
