// A first draft at a function to provide access to any of the data readers in G4MICE from the Data cards
// 
// M. Wojcik 
// 

#ifndef GLOBALREADER_HH
#define GLOBALREADER_HH

#include <iostream>

#include "Interface/Reader.hh"
#include "Interface/dataCards.hh"
#include "Interface/MICEEvent.hh"
#include "Interface/MICERun.hh"
#include "Persist/common/PersistClasses.hh"
#include "Persist/textFile/TextFileReader.hh"

//Reader* makeReader( std::string fname, dataCards& cards, MICEEvent& theEvent, MICERun* run, std::string dataType, PersistClasses* pC );

/*Reader* makeReader( std::string fname, dataCards& cards, MICEEvent& theEvent, MICERun* run, PersistClasses* pC )
{
  std::string dataType = cards.fetchValueString( "DataType" ); 
  return( makeReader( fname, cards, theEvent, run, dataType, pC ) ); 
  }*/


/*Reader* makeReader( std::string fname, dataCards& cards, MICEEvent& theEvent, PersistClasses* pC )
{
  return makeReader( fname, cards, theEvent, NULL, pC );
  }*/

/*Reader* makeReader( std::string fname, dataCards& cards, MICEEvent& theEvent, std::string dataType, PersistClasses* pC )
{
  return makeReader( fname, cards, theEvent, NULL, dataType, pC );
  }*/


Reader* makeReader( std::string fname, dataCards& cards, MICEEvent& theEvent, MICERun* run, std::string dataType, PersistClasses* pC );

#endif
