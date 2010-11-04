// TofDigit.hh
//
// M.Ellis 27/8/2005


#ifndef PERSIST_TofDigitTEXTFIILEIO_H
#define PERSIST_TofDigitTEXTFILEIO_H

#include "TextFileIoBase.hh"
#include "Interface/RFData.hh"
#include "CLHEP/Vector/ThreeVector.h"

#include "Interface/Memory.hh"

class RFDataTextFileIo : public TextFileIoBase
{
  public :

    RFDataTextFileIo( RFData* data, int index );
    RFDataTextFileIo( std::string );

    virtual ~RFDataTextFileIo() { miceMemory.addDelete( Memory::RFDataTextFileIo ); };

    std::string		storeObject();
    void		restoreObject( std::string );

    RFData*		rfData() const { return _data; };

    RFData*		makeRFData();

    void		completeRestoration();

  private :

		//The digit
    RFData*		_data;

		//The data in the digit that are written to the file
    Hep3Vector _cavityPos;
    double     _phase;
    double     _peakField;
    double     _frequency;
    double     _phaseError;
    double     _energyError;

};

#endif

