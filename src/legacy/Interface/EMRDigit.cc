#include "EMRDigit.hh"

#include "MICEEvent.hh"
#include "dataCards.hh"
#include "EMRHit.hh"
#include "VmefAdcHit.hh"
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
