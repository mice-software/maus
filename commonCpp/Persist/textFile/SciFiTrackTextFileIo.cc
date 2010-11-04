// SciFiTrackTextFileIo.cc
//
// M.Ellis 27/8/2005

#include <sstream>

#include "SciFiTrackTextFileIo.hh"

SciFiTrackTextFileIo::SciFiTrackTextFileIo( SciFiTrack* hit, int index ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::SciFiTrackTextFileIo );
	
  setTxtIoIndex( index );
  m_hit = hit;
}

SciFiTrackTextFileIo::SciFiTrackTextFileIo( std::string def ) : TextFileIoBase()
{										
  miceMemory.addNew( Memory::SciFiTrackTextFileIo );
  
  restoreObject( def );
}

std::string	SciFiTrackTextFileIo::storeObject()
{
  // fill the line with the information about the class here, don't forget the index!

  std::stringstream ss;

  ss << txtIoIndex();

  return( ss.str() );
}

void		SciFiTrackTextFileIo::restoreObject( std::string def )
{
  int myindex;

  std::istringstream ist( def.c_str() );

  ist >> myindex; //  >> and many more variable here probably...

  setTxtIoIndex( myindex );
}

SciFiTrack*	SciFiTrackTextFileIo::makeSciFiTrack()
{
  m_hit = new SciFiTrack();

  // possibly you have to use some "set" methods here...

  return m_hit;
}

void		SciFiTrackTextFileIo::completeRestoration()
{
}
