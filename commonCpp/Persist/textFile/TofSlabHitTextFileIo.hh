// TofSlabHit.hh
//
// M.Ellis 27/8/2005


#ifndef PERSIST_TofSlabHitTEXTFILEIO_H
#define PERSIST_TofSlabHitTEXTFILEIO_H

#include <iostream>

#include "TextFileIoBase.hh"
#include "Recon/TOF/TofSlabHit.hh"

#include "Interface/Memory.hh"

class VmeAdcHitTextFileIo;
class VmeTdcHitTextFileIo;
class TofDigitTextFileIo;

class TofSlabHitTextFileIo : public TextFileIoBase
{
  public :

    TofSlabHitTextFileIo( TofSlabHit*, int );
    TofSlabHitTextFileIo( std::string );

    virtual ~TofSlabHitTextFileIo() { miceMemory.addDelete( Memory::TofSlabHitTextFileIo ); };

    std::string		storeObject();
    void		restoreObject( std::string );

    TofSlabHit*		theHit() const { return m_hit; };

    TofSlabHit*		makeTofSlabHit();

	void	setTofDigits(std::vector<TofDigitTextFileIo*>& hits)	{ m_tofDigits = & hits; };

    void	completeRestoration();

  private :

    TofSlabHit*		m_hit;

    int		m_station;
    int 	m_plane;
    int 	m_slabNumber;
	double	r_time;      //row time
	double	c_time;      //corrected time	

	TofDigit*   m_digit1;
	TofDigit*   m_digit2;

	int m_digit1_index;
	int m_digit2_index;

  std::vector<TofDigitTextFileIo*>* m_tofDigits;

};

#endif

