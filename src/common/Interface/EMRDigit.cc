#include "Interface/EMRDigit.hh"

#include "Interface/MICEEvent.hh"
#include "Interface/dataCards.hh"
#include "Interface/EMRHit.hh"
#include "Interface/VmefAdcHit.hh"
#include "CLHEP/Random/Randomize.h"
using CLHEP::RandPoisson;
using CLHEP::RandGauss;
#include "CLHEP/Units/SystemOfUnits.h"
using CLHEP::eplus;
#include "gsl/gsl_sf_gamma.h"

EMRDigit::EMRDigit()
{
 miceMemory.addNew( Memory::EMRDigit ); 
}

EMRDigit::~EMRDigit()
{
 miceMemory.addDelete( Memory::EMRDigit );
}
