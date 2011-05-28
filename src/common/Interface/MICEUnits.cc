// MAUS WARNING: THIS IS LEGACY CODE.
// MICEUnits.cc

#include "Interface/MICEUnits.hh"
using CLHEP::meter;
using CLHEP::kilogram;
using CLHEP::second;
using CLHEP::ampere;
using CLHEP::kelvin;
using CLHEP::mole;
using CLHEP::candela;

//HepTool::Evaluator  MICEUnits::_evaluator;

MICEUnits::MICEUnits() : _default(1.), _units(), _evaluator()
{
	SetUnits();
}

double MICEUnits::GetUnits(const std::string unitString)
{
  if( unitString == "none" )
     return 1.;

	double unitsMultiplier = 1;
	std::string localString=unitString;

	unitsMultiplier = _evaluator.evaluate(localString.c_str());

	if(_evaluator.status() != HepTool::Evaluator::OK && _evaluator.status() != HepTool::Evaluator::WARNING_BLANK_STRING)
	{
		_evaluator.print_error();
		throw(Squeal(Squeal::recoverable, "Error parsing unit "+unitString, "MICEUnits::GetUnits") );
	}
	if(_evaluator.status() != HepTool::Evaluator::OK && _evaluator.status())
		return _default;

	return unitsMultiplier;
}

void MICEUnits::SetUnits()
{
	_evaluator.setStdMath();
	_evaluator.setSystemOfUnits(meter,kilogram,second,ampere,kelvin,mole,candela);
}



