//KLCellHit.cc
//
// A class representing a cell hit in KL

#include "Interface/KLDigit.hh"
#include "Interface/VmefAdcHit.hh"
#include "Recon/KL/KLCellHit.hh"
#include "Interface/MICERun.hh"
#include "Config/MiceModule.hh"

#include <iostream>
using namespace std;

KLCellHit::KLCellHit( KLDigit* dig1, KLDigit* dig2 )
:theRun(NULL), theModule(NULL)
{
  miceMemory.addNew( Memory::KLCellHit );
  
  _digit1 = dig1;
  _digit2 = dig2;
  m_good = false;

  // check if this is a good cell hit

  int side1 = _digit1->GetSide();
  int side2 = _digit2->GetSide();

  int cell1 = _digit1->GetCell();
  int cell2 = _digit2->GetCell();

  int adc1 = _digit1->GetADC();
  int adc2 = _digit2->GetADC();

 if( cell1 == cell2 && side1 != side2 )
     { 
       m_good = true;
       m_cellNumber = cell1;
       m_adc = adc1 + adc2;
       m_adcProd = 2 * adc1 * adc2 / (adc1 + adc2);
     }
}  

KLCellHit::KLCellHit( MICERun* r, const MiceModule* m, KLDigit* dig1 )
:theRun(r), theModule(m)
{
  miceMemory.addNew( Memory::KLCellHit );
  
  _digit1 = dig1;
  _digit2 = NULL;

  // check if this is a good cell hit

  int cell1 = _digit1->GetCell();
  int adc1 = _digit1->GetADC();
 
  m_good = true;
  m_cellNumber = cell1;
  m_adc = adc1;
  m_adcProd = adc1;

}

KLCellHit::KLCellHit( MICERun* r, const MiceModule* m, KLDigit* dig1, KLDigit* dig2 )
:theRun(r), theModule(m)
{
  miceMemory.addNew( Memory::KLCellHit );
  
  _digit1 = dig1;
  _digit2 = dig2;
  m_good = false;

  // check if this is a good cell hit

  int side1 = _digit1->GetSide();
  int side2 = _digit2->GetSide();

  int cell1 = _digit1->GetCell();
  int cell2 = _digit2->GetCell();

  int adc1 = _digit1->GetADC();
  int adc2 = _digit2->GetADC();

  //  cout << "Cell1: " << dec << cell1 << ", Cell2: " << cell2 << endl; 
  //cout << "Side1: " << side1 << ", Side2: " << side2 << endl; 
  //cout << "Adc1: " << adc1 << ", Adc2: " << adc2 << endl; 
  //cout << "----------------------------- " << endl; 

  if( cell1 == cell2 && side1 != side2 )
      { 
			m_good = true;
			m_cellNumber = cell1;
			m_adc = adc1 + adc2;
			m_adcProd = 2 * adc1 * adc2 / (adc1 + adc2);
      }
}

void KLCellHit::Print( bool verbose )
{
	if ( verbose ) {
	cout << "____________________________" << endl;
	cout << "  KLCellHit" << endl;
	cout << "  Cell Number : "<< m_cellNumber << endl;
	cout << "  ADC : " << m_adc << endl;
	cout << "  ADC Product: " << m_adcProd << endl;
	cout << "____________________________" << endl;
	}
	if ( !verbose ) {
		cout << "KLCellHit: " << m_cellNumber << ", ADC: " << m_adc << ", ADC Product: " << m_adcProd <<endl; 
	}
}

KLDigit* KLCellHit::digit( int const side ) const {
	KLDigit* Digit = 0;
	if ( digit1()->GetSide() == side ) Digit = digit1();
	if ( digit2()->GetSide() == side ) Digit = digit2();
	return Digit;
}
