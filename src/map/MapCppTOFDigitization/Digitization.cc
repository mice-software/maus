/*
** Written for the MICE proposal simulation
** Ed McKigney - Aug 21 2002
*/

#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include "gsl/gsl_ieee_utils.h"
#include "SciFiDeadChanList.hh"
#include "SciFiDigitizationParams.hh"
#include "SciFiDigits.hh"
#include "SciFiDigit.hh"
#include "TofDigit.hh"
#include "EmCalDigit.hh"
#include "CkovDigit.hh"

#include "CLHEP/Vector/ThreeVector.h"
#include "MICEEvent.hh"
#include "MICERun.hh"
#include "Event.hh"

#include "dataCards.hh"

#include "PersistClasses.hh"
#include "TextFileWriter.hh"
#include "TextFileReader.hh"
#include "MiceEventManager.hh"

#include "MiceModule.hh"
#include "GlobalReader.hh"

using std::cout;
using std::endl;

dataCards MyDataCards("Digitization");
SciFiDeadChanList dcl;

TextFileWriter* writer;
Reader* reader;

PersistClasses* pClassesRead;
PersistClasses* pClassesWrite;



void 	wipeDigits( MICEEvent& );

int main(int argc, char **argv)
{
  if (argc < 2 )
  {
    std::cout << "Usage: " << argv[0] << " " << "cardsFile (overwrite)" << std::endl;
    std::cout << "If the option overwrite is added then any existing digitised classes in the input file will be ignored, otherwise" << std::endl;
    std::cout << "The simulated hits will be used to generate Digits that will be added to whatever already exists" << std::endl;
    std::exit(1);
  }

  bool overwrite = true;

/*
  if( argc == 3 && ! strcmp( argv[2], "overwrite" ) )
    overwrite = true;
*/

  if (MyDataCards.readKeys(argv[1]) == 0)
  {
    std::string cardName(argv[1]);
    cout << "dataCard File " << cardName << " not found" << endl;
    std::exit(1);
  }

  // the MICE Event
  MICEEvent theEvent;

  // the MICE Run
  MICERun theRun;

  theRun.DataCards = & MyDataCards;

  // the MiceModel

  MiceModule* model = new MiceModule( MyDataCards.fetchValueString( "MiceModel" ) );

  theRun.miceModule = model;

  // setup persistency

  pClassesRead = new PersistClasses( true );
  pClassesWrite = new PersistClasses( true );

  bool readMany = false;
  std::string infName = MyDataCards.fetchValueString( "InputFile" );
  int fileNum = 2;

//   if( argc > 2 && ! overwrite )
  if( argc > 2)
  {
    readMany = true;
    infName = std::string( argv[ fileNum ] );
  }
  else if( argc > 3 )
  { 
    readMany = true;
    fileNum++;
    infName = std::string( argv[ fileNum ] );
  }

  reader = makeReader( infName, MyDataCards, theEvent, pClassesRead );
  writer = new TextFileWriter( MyDataCards.fetchValueString( "OutputFile" ), theEvent, *pClassesWrite );

  if( reader->IsOk() == false )
    exit(1);

  int count = 0;
  int done = 0;

  while( ! done && count < MyDataCards.fetchValueInt( "numEvts" ))
  {
    // read in an event

    std::cout << "Count = " << count << std::endl;

    done = ! reader->readEvent();

    if( ! done )
    {
      Event ev( theEvent, theRun  );

      if( overwrite ) // first need to delete any existing Digits!
        wipeDigits( theEvent );

      ev.Process();

      writer->writeEvent();

      clearEvent( theEvent );
    }

    count++;

    if (!(count%10))
        cout<<"Processed "<<count<<" events"<<endl;

    if( done && readMany && fileNum < ( argc - 1 ) )
    {
      delete reader;
      fileNum++;
      infName = std::string( argv[ fileNum ] );
      reader = makeReader( "", MyDataCards, theEvent, pClassesRead );
      done = 0;
   }
  }

  reader->closeInputFile();
  writer->closeOutputFile();
}

void	wipeDigits( MICEEvent& event )
{
  for( unsigned int i = 0; i < event.sciFiDigits.size(); ++i )
    delete event.sciFiDigits[i];
  event.sciFiDigits.clear();

  for( unsigned int i = 0; i < event.tofDigits.size(); ++i )
    delete event.tofDigits[i];
  event.tofDigits.clear();

  for( unsigned int i = 0; i < event.emCalDigits.size(); ++i )
    delete event.emCalDigits[i];
  event.emCalDigits.clear();

  for( unsigned int i = 0; i < event.ckovDigits.size(); ++i )
    delete event.ckovDigits[i];
  event.ckovDigits.clear();
}

