// SciFiDigitTextfileIo.hh
//
// M.Ellis 22/8/2005


#ifndef PERSIST_SCIFIDIGITTEXTFILEIO_H
#define PERSIST_SCIFIDIGITTEXTFILEIO_H

#include <vector>

#include "TextFileIoBase.hh"
#include "Interface/SciFiHit.hh"
#include "Interface/SciFiDigit.hh"
#include "Interface/Memory.hh" 

class SciFiHitTextFileIo;

class SciFiDigitTextFileIo : public TextFileIoBase
{
  public :

    SciFiDigitTextFileIo( SciFiDigit*, int );
    SciFiDigitTextFileIo( std::string );

    virtual ~SciFiDigitTextFileIo() { miceMemory.addDelete( Memory::SciFiDigitTextFileIo ); };

    std::string		storeObject();
    void		restoreObject( std::string );

    SciFiDigit*		theDigit() const { return m_digit; };

    SciFiDigit*		makeSciFiDigit();

    void		completeRestoration();

    void		setSciFiHits( std::vector<SciFiHitTextFileIo*>& );

  private :

    SciFiDigit*		m_digit;

    int         	trackerNo;
    int         	stationNo;
    int         	planeNo;
    int         	fiberNo;
    int			adcCounts;
    int			tdcCounts;
    SciFiHit*		m_mc_hit;
    bool		used;
    double		m_pe;

    int					mchit_index;
    std::vector<SciFiHitTextFileIo*>*	m_mchits;
};

#endif

