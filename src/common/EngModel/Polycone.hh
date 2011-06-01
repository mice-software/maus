// MAUS WARNING: THIS IS LEGACY CODE.
// @(#) $Id: Polycone.hh,v 1.4 2008-08-08 08:24:43 rogers Exp $  $Name:  $
//
// Polycone class reads in a file of doubles and creates a polycone
//
// Chris Rogers
//
// Input file format should be
//
// NumberOfPoints 22
// Direction      Increasing
// Units          mm
// R         ZInner         ZOuter
// 0         0              0.150
// ...       ...            ...   (continue to 22 lines)
//
// Two types of cone, specified in MiceModule:
// "Shell" is a two layered cone e.g. an RF window
// "Fill"  is a solid cone e.g. absorber LH2 without any window
// I do pay attention to the units in the header!
// If the polycone is "Fill", I ignore ZOuter and GetShell() returns NULL
// ZInner is assumed to be downstream of ZOuter
//
// Algorithm has to go from R, ZInner, ZOuter (file format)
//                       to Z, RInner, ROuter (for Geant4)
// Hence all the messing with splines
//
// R, Z must be monotonic (i.e. always getting bigger or smaller)
// Otherwise use two files!
//

#ifndef POLYCONE_HH
#define POLYCONE_HH 1

#include <vector>
#include <iostream>
#include <fstream>
#include <string>

#include "globals.hh"
#include "G4Polycone.hh"
#include "Interface/MICEUnits.hh"
#include "Interface/Spline1D.hh"
#include "Config/MiceModule.hh"

class Polycone
{
public:
	//Constructor taking a MiceModule
	Polycone(MiceModule *theModule); //e.g. for a window
	Polycone(); 
	~Polycone();

	//Return either the shell or the fill depending on the PolyconeType
	G4Polycone * GetPolycone();
	//Return a solid corresponding to the inner volume
	G4Polycone * GetFill();
	//Return a solid corresponding to a thin shell
	G4Polycone * GetShell();

	std::vector<double> ZCoords();
	std::vector<double> RInner();
	std::vector<double> ROuter();
	bool                IsShell() {return _polyconeType == "Shell";} 

private:

        std::string	replaceVariables( std::string );

	//File I/O
	void ReadPolyconeFile(std::istream & fin);
	void GetNumberOfCoordinates(std::istream & fin);
	void GetUnits(std::istream & fin);
	//Get coordinates for inner and outer surface
	void GetShellCoordinates(std::istream & fin);
	//Get coordinates ignoring anything for the outer surface
	void GetFillCoordinates(std::istream & fin);
	//Print out inner, outer surfaces - for testing
	void Print(std::ostream & out);

	//Splines for G4Polycone constructor
	double * _zCoordinates, * _rInner, * _rOuter;
	//other useful stuff
	double _units;
	int    _numberOfCoordinates;
	std::string _polyconeType;
	std::string _name;
	std::string _direction;
	static const double _phiStart, _phiEnd;
};

#endif
