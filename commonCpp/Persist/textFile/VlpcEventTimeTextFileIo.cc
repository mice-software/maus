// VlpcEventTimeTextFileIo.cc
//
// M.Ellis 27/8/2005

#include <sstream>

#include "VlpcEventTimeTextFileIo.hh"

VlpcEventTimeTextFileIo::VlpcEventTimeTextFileIo( VlpcEventTime* hit, int index ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::VlpcEventTimeTextFileIo );
	
  setTxtIoIndex( index );
  m_hit = hit;
}

VlpcEventTimeTextFileIo::VlpcEventTimeTextFileIo( std::string def ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::VlpcEventTimeTextFileIo );
  
  restoreObject( def );
}

std::string	VlpcEventTimeTextFileIo::storeObject()
{
  // fill the line with the information about the class here, don't forget the index!

  m_time = m_hit->time();
  
  std::stringstream ss;
  ss<< txtIoIndex() << " " << m_time;
  return( ss.str() );
}

void		VlpcEventTimeTextFileIo::restoreObject( std::string def )
{
  int myindex;

  std::istringstream ist( def.c_str() );

  ist >> myindex >> m_time; //  >> and many more variable here probably...

  setTxtIoIndex( myindex );
}

VlpcEventTime*	VlpcEventTimeTextFileIo::makeVlpcEventTime()
{
  m_hit = new VlpcEventTime(m_time);

  // possibly you have to use some "set" methods here...

  return m_hit;
}

void		VlpcEventTimeTextFileIo::completeRestoration()
{
}
