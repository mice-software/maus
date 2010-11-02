#include "KLDigit.hh"

#include "MICEEvent.hh"
#include "dataCards.hh"
#include "KLHit.hh"
#include "VmefAdcHit.hh"
#include "CLHEP/Random/Randomize.h"
using CLHEP::RandPoisson;
using CLHEP::RandGauss;
#include "CLHEP/Units/SystemOfUnits.h"
using CLHEP::eplus;
#include "gsl/gsl_sf_gamma.h"

KLDigit::KLDigit()
{
 miceMemory.addNew( Memory::KLDigit ); 
}

KLDigit::KLDigit(VmefAdcHit* aHit, int side, int cell)
{
  miceMemory.addNew( Memory::KLDigit ); 

  // initialize
  m_adc = aHit->GetCharge(ceaMinMax);
  m_side = side;
  m_cell = cell;
  m_fadc = aHit;
}

KLDigit::~KLDigit()
{
 miceMemory.addDelete( Memory::KLDigit );
}

void KLDigit::Print()
{
  	std::cout << "____________________________" << std::endl;
  	std::cout << "  KLDigit" 		<< std::endl;
  	std::cout << "  Cell Number : "<< m_cell << std::endl;
  	std::cout << "  PMT Number : " << m_side << std::endl;
  	std::cout << "  ADC : " << m_adc << std::endl;
  	std::cout << "____________________________" << std::endl;
}
