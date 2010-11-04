// EmCalDigit.hh
//
// M.Ellis 27/8/2005


#ifndef PERSIST_EmCalDigitTEXTFIILEIO_H
#define PERSIST_EmCalDigitTEXTFILEIO_H

#include "TextFileIoBase.hh"
#include "Interface/EmCalDigit.hh"
#include "Interface/Memory.hh" 

class EmCalHit;
class EmCalHitTextFileIo;

class EmCalDigitTextFileIo : public TextFileIoBase
{
  public :

    EmCalDigitTextFileIo( EmCalDigit*, int );
    EmCalDigitTextFileIo( std::string );

    virtual ~EmCalDigitTextFileIo() { miceMemory.addDelete( Memory::EmCalDigitTextFileIo ); };

    std::string		storeObject();
    void		restoreObject( std::string );

    EmCalDigit*		theDigit() const { return m_digit; };

    EmCalDigit*		makeEmCalDigit();

    void		completeRestoration();

    void    setEmCalHits( std::vector<EmCalHitTextFileIo*>* );

  private :

    EmCalDigit*		m_digit;
    std::vector<EmCalHitTextFileIo*>* _hitIos;
    std::vector<int>  _hitIndices;

    // ***** variables go here ******
    std::vector< int > _tdcVector; // vector containing all tdc bits above threshold
    std::vector< EmCalHit* > _hitVector;
    int         _adc;
    int         _tdcStart;
    int         _tdcLength;
    int         _side;
    double      _edep;
};

#endif

