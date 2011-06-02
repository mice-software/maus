// MAUS WARNING: THIS IS LEGACY CODE.
// @(#) $Id: BeamlineParameters.hh,v 1.5 2008-03-20 17:04:27 torun Exp $  $Name:  $
//
//  MICE beam parameters
//
//  Yagmur Torun 

#ifndef BEAMLINEPARAMETERS_H
#define BEAMLINEPARAMETERS_H 1

#include "CLHEP/Vector/ThreeVector.h"
using CLHEP::Hep3Vector;
#include "CLHEP/Units/SystemOfUnits.h"

#include <string>

/**	class BeamlineParameters defines the parameters for the beamline
	Only the portion of the beamline implemented in MICE is included;
	that is, Q4 - Q9, and detectors within them.
 **/
class BeamlineParameters {
public:
	std::string	fBeamlineMaterial;
	double	fQ35FieldLength;
	double	fQ35IronLength;
	double	fQ35IronRadius;
	double	fQ35ApertureRadius;
	double	fQ35PoleTipRadius;
	double	fQ35CoilRadius;
	double	fQ35CoilHalfwidth;
	double	fQ35MaxStep;
	std::string	fQ35IronMaterial;
	int		fQ35Kill;
	std::string	fQ35Fringe;
	double	fQ35FringeFactor;
	double	fQ4Gradient;
	Hep3Vector	fQ4Position;
	double	fQ5Gradient;
	Hep3Vector	fQ5Position;
	double	fQ6Gradient;
	Hep3Vector	fQ6Position;
	double	fQ7Gradient;
	Hep3Vector	fQ7Position;
	double	fQ8Gradient;
	Hep3Vector	fQ8Position;
	double	fQ9Gradient;
	Hep3Vector	fQ9Position;

	static BeamlineParameters* fInstance;
public:
	static BeamlineParameters* getInstance();
	BeamlineParameters();
	
	std::string	BeamlineMaterial()	{ return fBeamlineMaterial; }
	double	Q35FieldLength()	{ return fQ35FieldLength; }
	double	Q35IronLength()		{ return fQ35IronLength; }
	double	Q35IronRadius()		{ return fQ35IronRadius; }
	double	Q35ApertureRadius()	{ return fQ35ApertureRadius; }
	double	Q35PoleTipRadius()	{ return fQ35PoleTipRadius; }
	double	Q35CoilRadius()		{ return fQ35CoilRadius; }
	double	Q35CoilHalfwidth()	{ return fQ35CoilHalfwidth; }
	double	Q35MaxStep()		{ return fQ35MaxStep; }
	std::string	Q35IronMaterial()	{ return fQ35IronMaterial; }
	int   	Q35Kill() 		{ return fQ35Kill; }
	std::string	Q35Fringe()		{ return fQ35Fringe; }
	double	Q35FringeFactor()	{ return fQ35FringeFactor; }
	double	Q4Gradient()		{ return fQ4Gradient; }
	Hep3Vector	Q4Position()		{ return fQ4Position; }
	double	Q5Gradient()		{ return fQ5Gradient; }
	Hep3Vector	Q5Position()		{ return fQ5Position; }
	double	Q6Gradient()		{ return fQ6Gradient; }
	Hep3Vector	Q6Position()		{ return fQ6Position; }
	double	Q7Gradient()		{ return fQ7Gradient; }
	Hep3Vector	Q7Position()		{ return fQ7Position; }
	double	Q8Gradient()		{ return fQ8Gradient; }
	Hep3Vector	Q8Position()		{ return fQ8Position; }
	double	Q9Gradient()		{ return fQ9Gradient; }
	Hep3Vector	Q9Position()		{ return fQ9Position; }
};

#endif

