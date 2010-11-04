// EmCalTrack.hh
//
// M.Ellis 27/8/2005


#ifndef PERSIST_EmCalTrackTEXTFIILEIO_H
#define PERSIST_EmCalTrackTEXTFILEIO_H

#include "TextFileIoBase.hh"
#include "Interface/EmCalTrack.hh"
#include "Interface/Memory.hh" 

class EmCalDigit;
class EmCalDigitTextFileIo;

class EmCalTrackTextFileIo : public TextFileIoBase
{
  public :

    EmCalTrackTextFileIo( EmCalTrack*, int );
    EmCalTrackTextFileIo( std::string );

    virtual ~EmCalTrackTextFileIo() { miceMemory.addDelete( Memory::EmCalTrackTextFileIo ); };

    std::string   storeObject();
    void    restoreObject( std::string );

    EmCalTrack*   theTrack() const { return m_track; };

    EmCalTrack*   makeEmCalTrack();

    void    completeRestoration();
    void    setEmCalDigits( std::vector<EmCalDigitTextFileIo*>* );

  private :

    EmCalTrack*   m_track;
    
    std::vector<EmCalDigitTextFileIo*>* _digitIos;
    std::vector<int>  _digitIndices;
    std::vector< EmCalDigit* > _digitVector;

    // ***** variables go here ******
    int _tdcPeaks;
    double _totalProdADC;
    double _barycenter;
    int _maxADClay;
    int _range;
    int _rangeH;
    double _holesQ; 
    double _highQ;
    int _maxLSubR; 
    std::vector< double > _adcProd;
    std::vector< double > _adcAdj;
    std::vector< double > _adcQ;
    std::vector< int > _n;
    std::vector< int > _nH;
};

#endif

