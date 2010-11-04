// VlpcHitTextFileIo.cc
//
// M.Ellis 22/8/2005

#include <sstream>

#include "VlpcHitTextFileIo.hh"

VlpcHitTextFileIo::VlpcHitTextFileIo( VlpcHit* hit, int index ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::VlpcHitTextFileIo );
	
  setTxtIoIndex( index );
  m_hit = hit;
}

VlpcHitTextFileIo::VlpcHitTextFileIo( std::string def ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::VlpcHitTextFileIo );
  
  restoreObject( def );
}

std::string	VlpcHitTextFileIo::storeObject()
{
  m_cryo = m_hit->cryo();
  m_cass = m_hit->cassette();
  m_afe = m_hit->afe();
  m_mcm = m_hit->mcm();
  m_chan = m_hit->chan();
  m_adc = m_hit->adc();
  m_discrim = m_hit->discrim();
  m_tdc = m_hit->tdc();  
  m_trig_time = m_hit->triggerTime();
  m_bunch_num = m_hit->bunchNumber();

  std::stringstream ss;
  ss << txtIoIndex() << " " << m_cryo << " " << m_cass << " " <<  m_afe << "  "<< m_mcm << " " << m_chan << " " << m_adc << " " << m_discrim << " " << m_tdc << " " << m_trig_time << " " << m_bunch_num;
  return( ss.str() );
}

void		VlpcHitTextFileIo::restoreObject( std::string def )
{
  int myindex;

  std::istringstream ist( def.c_str() );

  ist >> myindex >> m_cryo >> m_cass >>  m_afe >> m_mcm >> m_chan >> m_adc >> m_discrim >> m_tdc >> m_trig_time >> m_bunch_num;

  setTxtIoIndex( myindex );
}

VlpcHit*	VlpcHitTextFileIo::makeVlpcHit()
{
  m_hit = new VlpcHit( m_cryo, m_cass, m_afe, m_mcm, m_chan, m_adc, m_discrim, m_tdc, m_trig_time, m_bunch_num );

  return m_hit;
}

void		VlpcHitTextFileIo::completeRestoration()
{
}
