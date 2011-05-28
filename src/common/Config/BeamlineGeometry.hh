// MAUS WARNING: THIS IS LEGACY CODE.
// @(#) $Id: BeamlineGeometry.hh,v 1.4 2008-03-20 17:04:27 torun Exp $  $Name:  $
//
//  MICE beamline geometry parameters
//
//  Yagmur Torun 

#ifndef BEAMLINEGEOMETRY_H
#define BEAMLINEGEOMETRY_H 1

#include "CLHEP/Vector/ThreeVector.h"
using CLHEP::Hep3Vector;
#include "CLHEP/Units/SystemOfUnits.h"

#include <string>

/**	BeamlineGeometry defines the layout of the MICE beamline
	Only the portion of the beamline implemented in MICE is included;
	that is, Q4 - Q9, and detectors within them.
 **/
class BeamlineGeometry {
public:
	std::string	fBeamlineMaterial;
	Hep3Vector	fQ4Position;
	Hep3Vector	fQ5Position;
	Hep3Vector	fQ6Position;
	Hep3Vector	fQ7Position;
	Hep3Vector	fQ8Position;
	Hep3Vector	fQ9Position;

	static BeamlineGeometry* fInstance;
public:
	static BeamlineGeometry* getInstance();
	BeamlineGeometry();
	
	std::string	BeamlineMaterial()	{ return fBeamlineMaterial; }
	Hep3Vector	Q4Position()		{ return fQ4Position; }
	Hep3Vector	Q5Position()		{ return fQ5Position; }
	Hep3Vector	Q6Position()		{ return fQ6Position; }
	Hep3Vector	Q7Position()		{ return fQ7Position; }
	Hep3Vector	Q8Position()		{ return fQ8Position; }
	Hep3Vector	Q9Position()		{ return fQ9Position; }
};

#endif

