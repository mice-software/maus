// KLCellHit.hh
//
// A class representing a cell hit in KL


#ifndef INTERFACE_KLCELLHIT_H
#define INTERFACE_KLCELLHIT_H

#include "Interface/Memory.hh"

class VmefAdcHit;
class KLDigit;
class MICERun;
class MiceModule;

class KLCellHit
{
  public :

	KLCellHit()			{};
	KLCellHit( KLDigit*, KLDigit* );
	KLCellHit( MICERun*, const MiceModule*, KLDigit* );
	KLCellHit( MICERun*, const MiceModule*, KLDigit*, KLDigit* );
	~KLCellHit() 		{ miceMemory.addDelete( Memory::KLCellHit ); };

	KLDigit*	digit1() const	 		{ return _digit1; }
	KLDigit*	digit2() const   		{ return _digit2; }
	KLDigit*	digit( int const side ) const;

	bool		isGood() const				{ return m_good; };
	int		cellNumber() const		{ return m_cellNumber; };
   double	adc() const             { return m_adc; };
   double	adcProd() const         { return m_adcProd; };

	void		SetCell(int cellNumber)			{ m_cellNumber = cellNumber; };
	void 		SetGood( bool g )		      	{ m_good = g; };
	void 		SetDigit1( KLDigit* Digit)		{ _digit1 = Digit; };
	void 		SetDigit2( KLDigit* Digit)		{ _digit2 = Digit; };

	void 		Print( bool verbose = true );

  private :

        int 		m_cellNumber;
        int 		m_adc;
	     int 		m_adcProd;
	     bool		m_good;

	     KLDigit*   _digit1;
	     KLDigit*   _digit2;

	     MICERun* theRun;
	     const MiceModule* theModule;

};

#endif

