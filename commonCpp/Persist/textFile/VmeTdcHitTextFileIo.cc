// VmeTdcHitTextFileIo.cc
//
// M.Ellis 27/8/2005

#include <sstream>

#include "VmeTdcHitTextFileIo.hh"

VmeTdcHitTextFileIo::VmeTdcHitTextFileIo( VmeTdcHit* hit, int index ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::VmeTdcHitTextFileIo );
	
  setTxtIoIndex( index );
  m_hit = hit;
}

VmeTdcHitTextFileIo::VmeTdcHitTextFileIo( std::string def ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::VmeTdcHitTextFileIo );
  
  restoreObject( def );
}

std::string	VmeTdcHitTextFileIo::storeObject()
{
  // fill the line with the information about the class here, don't forget the index!

   m_crate = m_hit->crate();
    m_board = m_hit->board();
    m_module_type = m_hit->moduleType();
    m_channel = m_hit->channel();
    m_tdc = m_hit->tdc();

  std::stringstream ss;
  ss << txtIoIndex() << " " << m_crate << " " << m_board << " " << m_channel << " " << m_tdc << " " << m_module_type.c_str();
  return( ss.str() );
}

void		VmeTdcHitTextFileIo::restoreObject( std::string def )
{
  int myindex;

  std::istringstream ist( def.c_str() );

 ist >> myindex >> m_crate >> m_board >> m_channel >> m_tdc >> m_module_type;


  setTxtIoIndex( myindex );
}

VmeTdcHit*	VmeTdcHitTextFileIo::makeVmeTdcHit()
{

  m_hit = new VmeTdcHit(m_crate, m_board, m_module_type, m_channel, m_tdc);
 
  // possibly you have to use some "set" methods here...

  return m_hit;
}

void		VmeTdcHitTextFileIo::completeRestoration()
{
}
