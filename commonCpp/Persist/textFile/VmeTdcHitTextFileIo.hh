// VmeTdcHit.hh
//
// M.Ellis 27/8/2005


#ifndef PERSIST_VmeTdcHitTEXTFIILEIO_H
#define PERSIST_VmeTdcHitTEXTFILEIO_H

#include "TextFileIoBase.hh"
#include "Interface/VmeTdcHit.hh"

#include "Interface/Memory.hh"

class VmeTdcHitTextFileIo : public TextFileIoBase
{
  public :

    VmeTdcHitTextFileIo( VmeTdcHit*, int );
    VmeTdcHitTextFileIo( std::string );

    virtual ~VmeTdcHitTextFileIo() { miceMemory.addDelete( Memory::VmeTdcHitTextFileIo ); };

    std::string		storeObject();
    void		restoreObject( std::string );

    VmeTdcHit*		theHit() const { return m_hit; };

    VmeTdcHit*		makeVmeTdcHit();

    void		completeRestoration();

  private :

    VmeTdcHit*		m_hit;
    int 		m_crate;
    int			m_board;
    std::string		m_module_type;
    int			m_channel;
    int	                m_tdc;
 
};

#endif

