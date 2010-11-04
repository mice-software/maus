// TextFileWriter.hh
//
// M.Ellis 22/8/2005

#ifndef PERSIST_TEXTFILEWRITER_H
#define PERSIST_TEXTFILEWRITER_H

#include <string>
#include <fstream>

#include "zlib.h"

#include "Interface/MICEEvent.hh"

#include "Persist/common/PersistClasses.hh"

#include "Interface/Memory.hh" 

class TextFileWriter
{
  public :

    TextFileWriter( std::string, MICEEvent&, PersistClasses& );
    
    ~TextFileWriter() 	{ miceMemory.addDelete( Memory::TextFileWriter ); }; 
    
    void	setMaximumFileSize( int max )		{ maxWrite = max; };

    void	writeEvent();

    void	closeOutputFile();

  private :

    void	openNewFile();

    template <class t>
    void       writeIo(t);

    int			fileNum;
    int			maxWrite;
    std::string		fileName;
    gzFile	 	outf;
    MICEEvent&	   	event;
    PersistClasses&	persist;
};

#endif

