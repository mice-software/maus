// VmeAdcHitTextFileIo.cc
//
// M.Ellis 27/8/2005

#include <sstream>

#include "VmeAdcHitTextFileIo.hh"

VmeAdcHitTextFileIo::VmeAdcHitTextFileIo( VmeAdcHit* hit, int index ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::VmeAdcHitTextFileIo ); 
	
  setTxtIoIndex( index );
  m_hit = hit;
}

VmeAdcHitTextFileIo::VmeAdcHitTextFileIo( std::string def ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::VmeAdcHitTextFileIo ); 
  
  restoreObject( def );
}

std::string	VmeAdcHitTextFileIo::storeObject()
{
  // fill the line with the information about the class here, don't forget the index!
    m_crate = m_hit->crate();
    m_board = m_hit->board();
    m_module_type = m_hit->moduleType();
    m_channel = m_hit->channel();
    m_adc = m_hit->adc();

    std::stringstream ss;
    ss << txtIoIndex() << " " << m_crate << " " << m_board << " " << m_channel << " " << m_adc << " " << m_module_type.c_str();
    return( ss.str() );
}

void		VmeAdcHitTextFileIo::restoreObject( std::string def )
{
  int myindex;

  std::istringstream ist( def.c_str() );

  ist >> myindex >> m_crate >> m_board >> m_channel >> m_adc >> m_module_type;

  setTxtIoIndex( myindex );
}

VmeAdcHit*	VmeAdcHitTextFileIo::makeVmeAdcHit()
{
  m_hit = new VmeAdcHit(m_crate, m_board, m_module_type, m_channel, m_adc);

  // possibly you have to use some "set" methods here...

  return m_hit;
}

void		VmeAdcHitTextFileIo::completeRestoration()
{
}
