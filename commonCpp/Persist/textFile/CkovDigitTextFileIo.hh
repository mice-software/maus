// CkovDigit.hh
//
// M.Ellis 7/10/2006


#ifndef PERSIST_CkovDigitTEXTFIILEIO_H
#define PERSIST_CkovDigitTEXTFILEIO_H

#include "TextFileIoBase.hh"
#include "Interface/CkovDigit.hh"

#include "Interface/Memory.hh" 

class CkovHit;
class CkovHitTextFileIo;
class VmeAdcHitTextFileIo;
class VmefAdcHitTextFileIo;
class VmeTdcHitTextFileIo;

class CkovDigitTextFileIo : public TextFileIoBase
{
  public :

    CkovDigitTextFileIo( CkovDigit*, int );
    CkovDigitTextFileIo( std::string );

    virtual ~CkovDigitTextFileIo() { miceMemory.addDelete( Memory::CkovDigitTextFileIo ); };

    std::string		storeObject();
    void		restoreObject( std::string );

    CkovDigit*		theDigit() const { return m_digit; };

    CkovDigit*		makeCkovDigit();

    void	completeRestoration();

    void    	setCkovHits( std::vector<CkovHitTextFileIo*>* );
    void	setVmeAdcHits(std::vector<VmeAdcHitTextFileIo*>& hits)		{ m_vmeAdcHits =  & hits; };
    void	setVmefAdcHits(std::vector<VmefAdcHitTextFileIo*>& hits)	{ m_vmefAdcHits =  & hits; };	
    void	setVmeTdcHits(std::vector<VmeTdcHitTextFileIo*>& hits)		{ m_vmeTdcHits = & hits; };

  private :

	//Indices for the digit member data
	//(these also are written in the file)
    int m_adc_index;
    int m_fadc_index;
    int m_tdc_index;
	
    CkovDigit*	m_digit;
    std::vector<CkovHitTextFileIo*>* _hitIos;
    std::vector<int>  _hitIndices;

    std::vector<VmeAdcHitTextFileIo*>*  m_vmeAdcHits;
    std::vector<VmefAdcHitTextFileIo*>* m_vmefAdcHits;
    std::vector<VmeTdcHitTextFileIo*>*  m_vmeTdcHits;
	
    VmeAdcHit  *	m_adcHit;
    VmefAdcHit *	m_fadcHit;
    VmeTdcHit  *	m_tdcHit;

    // ***** variables go here ******
    std::vector<CkovHit*> _hitVector;
    int         _tdc;
    int         _adc;
    double      _pe;
    int         _pmt;
    int         _plane;
    bool 		_good;
};

#endif

