// SciFiTrack.hh
//
// M.Ellis 27/8/2005


#ifndef PERSIST_SciFiTrackTEXTFIILEIO_H
#define PERSIST_SciFiTrackTEXTFILEIO_H

#include "TextFileIoBase.hh"
#include "Recon/SciFi/SciFiTrack.hh"
#include "Interface/Memory.hh" 

class SciFiTrackTextFileIo : public TextFileIoBase
{
  public :

    SciFiTrackTextFileIo( SciFiTrack*, int );
    SciFiTrackTextFileIo( std::string );

    virtual ~SciFiTrackTextFileIo() { miceMemory.addDelete( Memory::SciFiTrackTextFileIo ); };

    std::string		storeObject();
    void		restoreObject( std::string );

    SciFiTrack*		theHit() const { return m_hit; };

    SciFiTrack*		makeSciFiTrack();

    void		completeRestoration();

  private :

    SciFiTrack*		m_hit;

    // ***** variables go here ******
};

#endif

