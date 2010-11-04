
#ifndef PERSIST_EmCalHitTEXTFIILEIO_H
#define PERSIST_EmCalHitTEXTFILEIO_H

#include "TextFileIoBase.hh"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Matrix/SymMatrix.h"

#include "Interface/Memory.hh" 

class ZustandVektor;

class ZustandVektorTextFileIo : public TextFileIoBase
{
  public :

    ZustandVektorTextFileIo( ZustandVektor*, int );
    ZustandVektorTextFileIo( std::string );

    virtual ~ZustandVektorTextFileIo() { miceMemory.addDelete( Memory::ZustandVektorTextFileIo ); };

    std::string   storeObject();
    void    restoreObject( std::string );

    ZustandVektor*   theVector() const { return m_vector; };

    ZustandVektor*   makeZustandVektor();

    void    completeRestoration();

  private :

    ZustandVektor*   m_vector;
};

#endif

