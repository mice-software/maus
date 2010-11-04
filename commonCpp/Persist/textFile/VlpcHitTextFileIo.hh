// VlpcHitTextfileIo.hh
//
// M.Ellis 22/8/2005


#ifndef PERSIST_VLPCHITTEXTFILEIO_H
#define PERSIST_VLPCHITTEXTFILEIO_H

#include "TextFileIoBase.hh"
#include "Interface/VlpcHit.hh"
#include "Interface/Memory.hh" 

class VlpcHitTextFileIo : public TextFileIoBase
{
  public :

    VlpcHitTextFileIo( VlpcHit*, int );
    VlpcHitTextFileIo( std::string );

    virtual ~VlpcHitTextFileIo() { miceMemory.addDelete( Memory::VlpcHitTextFileIo ); };

    std::string		storeObject();
    void		restoreObject( std::string );

    VlpcHit*		theHit() const { return m_hit; };

    VlpcHit*		makeVlpcHit();

    void		completeRestoration();

  private :

    VlpcHit*		m_hit;

    int			m_cryo;
    int			m_cass;
    int			m_afe;
    int			m_mcm;
    int			m_chan;
    double		m_adc;
    bool                m_discrim;
    double              m_tdc;
    int			m_trig_time;
    int			m_bunch_num;
};

#endif

