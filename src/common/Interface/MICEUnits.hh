// MAUS WARNING: THIS IS LEGACY CODE.
// MICEUnits.hh
//
// Set units for MICE datacards from the datacard input file
//
// If unit not found, returns _default = 1.
// and error message
//


#ifndef MICEUNITS_HH
#define MICEUNITS_HH 1

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Evaluator/Evaluator.h"
#include <string>
#include <map>
#include <iostream>
#include <cctype>

#include "Interface/Squeal.hh"

class MICEUnits
{
public :
	MICEUnits();
	~MICEUnits()
	{;}

	double GetUnits(std::string unitString);

private :
	//Set the units to the correct values
	void SetUnits();

	double _default;
	std::map<const std::string, double> _units;
	HepTool::Evaluator  _evaluator;
};

#endif

