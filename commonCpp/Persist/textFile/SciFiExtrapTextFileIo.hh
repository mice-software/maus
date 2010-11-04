// SciFiExtrap.hh
//
// M.Ellis 27/8/2005


#ifndef PERSIST_SciFiExtrapTEXTFIILEIO_H
#define PERSIST_SciFiExtrapTEXTFILEIO_H

#include "TextFileIoBase.hh"
#include "Recon/SciFi/SciFiExtrap.hh"
#include "Interface/Memory.hh"

class SciFiExtrapTextFileIo : public TextFileIoBase
{
  public :

    SciFiExtrapTextFileIo( SciFiExtrap*, int );
    SciFiExtrapTextFileIo( std::string );

    virtual ~SciFiExtrapTextFileIo() { miceMemory.addDelete( Memory::SciFiExtrapTextFileIo ); };

    std::string		storeObject();
    void		restoreObject( std::string );

    SciFiExtrap*		theHit() const { return m_hit; };

    SciFiExtrap*		makeSciFiExtrap();

    void		completeRestoration();

  private :

    SciFiExtrap*		m_hit;

    // ***** variables go here ******
};

#endif

