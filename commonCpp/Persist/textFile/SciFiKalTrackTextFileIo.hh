// SciFiKalTrack.hh
//
// M.Ellis 27/8/2005


#ifndef PERSIST_SciFiKalTrackTEXTFIILEIO_H
#define PERSIST_SciFiKalTrackTEXTFILEIO_H

#include "TextFileIoBase.hh"
#include "Recon/SciFi/SciFiKalTrack.hh"
#include "Interface/Memory.hh"

class SciFiKalTrackTextFileIo : public TextFileIoBase
{
  public :

    SciFiKalTrackTextFileIo( SciFiKalTrack*, int );
    SciFiKalTrackTextFileIo( std::string );

    virtual ~SciFiKalTrackTextFileIo() { miceMemory.addDelete( Memory::SciFiKalTrackTextFileIo ); };

    std::string		storeObject();
    void		restoreObject( std::string );

    SciFiKalTrack*		theHit() const { return m_hit; };

    SciFiKalTrack*		makeSciFiKalTrack();

    void		completeRestoration();

  private :

    SciFiKalTrack*		m_hit;

    // ***** variables go here ******
};

#endif

