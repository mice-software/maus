// TextFileReader.hh
//
// M.Ellis 22/8/2005

#ifndef PERSIST_TEXTFILEREADER_H
#define PERSIST_TEXTFILEREADER_H

#include <string>
#include <fstream>

#include "zlib.h"

#include "Interface/Reader.hh"

#include "Interface/MICEEvent.hh"
#include "Interface/MICERun.hh"

#include "Persist/common/PersistClasses.hh"

#include "Interface/Memory.hh" 

class TextFileReader : public Reader
{
  public :

    TextFileReader( std::string, MICEEvent&, MICERun*, PersistClasses& );

    ~TextFileReader()		{ miceMemory.addDelete( Memory::TextFileReader ); };

    bool	readEvent();
    
    bool        IsOk() const;  
 
    void	closeInputFile();

    void	skipEvents( int );

  private :

    gzFile	 	inf;
    MICEEvent&	   	event;
    MICERun*		run;
    PersistClasses&	persist;
    bool                isOk;

    std::string		readOneLine( gzFile );
};

#endif

