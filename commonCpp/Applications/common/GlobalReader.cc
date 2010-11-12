// A first draft at a function to provide access to any of the data readers in G4MICE from the Data cards
// 
// M. Wojcik 
// 

#include "GlobalReader.hh"

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


Reader* makeReader( std::string fname, dataCards& cards, MICEEvent& theEvent, MICERun* run, std::string dataType, PersistClasses* pC ){
  Reader* reader = NULL;

  if( ! pC )
    pC = new PersistClasses( true );

  std::string inFileName = MyDataCards.fetchValueString("InputFile");

  if( fname != "" )
    inFileName = fname;

  std::cout << fname << " " << inFileName << std::endl;

  if( dataType == "TextFile" )
    reader = new TextFileReader( inFileName, theEvent, run, *pC );
  //  else if( dataType == "DATE" )
  //    reader = new MiceDAQReader( inFileName, theEvent );

  return reader;
}
