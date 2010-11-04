// SciFiKalTrackTextFileIo.cc
//
// M.Ellis 27/8/2005

#include <sstream>

#include "SciFiKalTrackTextFileIo.hh"

SciFiKalTrackTextFileIo::SciFiKalTrackTextFileIo( SciFiKalTrack* hit, int index ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::SciFiKalTrackTextFileIo );
	
  setTxtIoIndex( index );
  m_hit = hit;
}

SciFiKalTrackTextFileIo::SciFiKalTrackTextFileIo( std::string def ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::SciFiKalTrackTextFileIo );
  
  restoreObject( def );
}

std::string	SciFiKalTrackTextFileIo::storeObject()
{
  // fill the line with the information about the class here, don't forget the index!

  std::stringstream ss;

  ss << txtIoIndex();

  return( ss.str() );
}

void		SciFiKalTrackTextFileIo::restoreObject( std::string def )
{
  int myindex;

  std::istringstream ist( def.c_str() );

  ist >> myindex; //  >> and many more variable here probably...

  setTxtIoIndex( myindex );
}

SciFiKalTrack*	SciFiKalTrackTextFileIo::makeSciFiKalTrack()
{
  //m_hit = new SciFiKalTrack();

  // possibly you have to use some "set" methods here...

  return m_hit;
}

void		SciFiKalTrackTextFileIo::completeRestoration()
{
}
