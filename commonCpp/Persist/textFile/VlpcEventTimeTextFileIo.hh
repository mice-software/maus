// VlpcEventTime.hh
//
// M.Ellis 27/8/2005


#ifndef PERSIST_VlpcEventTimeTEXTFIILEIO_H
#define PERSIST_VlpcEventTimeTEXTFILEIO_H

#include "TextFileIoBase.hh"
#include "Interface/VlpcEventTime.hh"

#include "Interface/Memory.hh"

class VlpcEventTimeTextFileIo : public TextFileIoBase
{
  public :

    VlpcEventTimeTextFileIo( VlpcEventTime*, int );
    VlpcEventTimeTextFileIo( std::string );

    virtual ~VlpcEventTimeTextFileIo() { miceMemory.addDelete( Memory::VlpcEventTimeTextFileIo ); };

    std::string		storeObject();
    void		restoreObject( std::string );

    VlpcEventTime*		theHit() const { return m_hit; };

    VlpcEventTime*		makeVlpcEventTime();

    void		completeRestoration();

  private :

    VlpcEventTime*		m_hit;

    // ***** variables go here ******

   double m_time;

};

#endif

