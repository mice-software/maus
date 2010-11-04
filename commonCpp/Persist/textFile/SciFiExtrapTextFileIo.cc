// SciFiExtrapTextFileIo.cc
//
// M.Ellis 27/8/2005

#include <sstream>

#include "SciFiExtrapTextFileIo.hh"

SciFiExtrapTextFileIo::SciFiExtrapTextFileIo( SciFiExtrap* hit, int index ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::SciFiExtrapTextFileIo );
	
  setTxtIoIndex( index );
  m_hit = hit;
}

SciFiExtrapTextFileIo::SciFiExtrapTextFileIo( std::string def ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::SciFiExtrapTextFileIo );
  
  restoreObject( def );
}

std::string	SciFiExtrapTextFileIo::storeObject()
{
  // fill the line with the information about the class here, don't forget the index!

  std::stringstream ss;

  ss << txtIoIndex();

  return( ss.str() );
}

void		SciFiExtrapTextFileIo::restoreObject( std::string def )
{
  int myindex;

  std::istringstream ist( def.c_str() );

  ist >> myindex; //  >> and many more variable here probably...

  setTxtIoIndex( myindex );
}

SciFiExtrap*	SciFiExtrapTextFileIo::makeSciFiExtrap()
{
  m_hit = new SciFiExtrap();

  // possibly you have to use some "set" methods here...

  return m_hit;
}

void		SciFiExtrapTextFileIo::completeRestoration()
{
}
