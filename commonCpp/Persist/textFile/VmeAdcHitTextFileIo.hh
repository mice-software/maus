// VmeAdcHit.hh
//
// M.Ellis 27/8/2005


#ifndef PERSIST_VmeAdcHitTEXTFIILEIO_H
#define PERSIST_VmeAdcHitTEXTFILEIO_H

#include "TextFileIoBase.hh"
#include "Interface/VmeAdcHit.hh"

#include "Interface/Memory.hh"

class VmeAdcHitTextFileIo : public TextFileIoBase
{
  public :

    VmeAdcHitTextFileIo( VmeAdcHit*, int );
    VmeAdcHitTextFileIo( std::string );

    virtual ~VmeAdcHitTextFileIo() { miceMemory.addDelete( Memory::VmeAdcHitTextFileIo ); };

    std::string		storeObject();
    void		restoreObject( std::string );

    VmeAdcHit*		theHit() const { return m_hit; };

    VmeAdcHit*		makeVmeAdcHit();

    void		completeRestoration();

  private :

    VmeAdcHit*		m_hit;
    int 		m_crate;
    int			m_board;
    std::string		m_module_type;
    int			m_channel;
    int	                m_adc;

    // ***** variables go here ******
};

#endif

