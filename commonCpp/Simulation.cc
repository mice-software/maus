// @(#) $Id: Simulation.cc,v 1.61 2010-10-22 08:54:02 rogers Exp $ $Name:  $
// MICE main
// Yagmur Torun

#ifdef ROOTFLAG
#include "TROOT.h"
#include "TNtuple.h"
#include "TTree.h"
#include "TFile.h"
#include "TBranch.h"
#include "TH1.h"
#include "TH2.h"
#include "THistPainter.h"
#include "TCanvas.h"
#include "TApplication.h"
#include "TDrawPanelHist.h"
#endif

#include "Interface/Squeak.hh"

#include "gsl/gsl_ieee_utils.h"

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#include "G4UIXm.hh"
// new!!
#include "Simulation/MICEPhysicsList.hh"
#include "Simulation/MICEDetectorConstruction.hh"
#include "G4MuIonisation.hh"
#include "G4MultipleScattering.hh"
#include "Simulation/MICEPrimaryGeneratorAction.hh"
#include "Simulation/MICETrackingAction.hh"
#include "Simulation/MICESteppingAction.hh"
#include "Simulation/MICEStackingAction.hh"
#include "Simulation/MICEEventAction.hh"
#include "Simulation/MICERunAction.hh"
#include "Simulation/StripSpecialHits.hh"
#include "G4TransportationManager.hh"
#include "G4FieldManager.hh"
#include "G4ProcessTable.hh"
#include "G4Field.hh"

#include "Interface/dataCards.hh"
#include "globals.hh"
#include "Randomize.hh"
#include "BeamTools/BTPhaser.hh"

#include "Interface/MICEEvent.hh"
#include "Interface/MICERun.hh"
#include "Interface/SpecialHit.hh"
#include "Interface/ZustandVektor.hh"

#include "Config/MiceModule.hh"
#include "Interface/MiceMaterials.hh"

#include "Persist/common/PersistClasses.hh"
#include "Persist/textFile/TextFileWriter.hh"
#include "Persist/textFile/TextFileReader.hh"
#include "Persist/common/MiceEventManager.hh"

#include "Simulation/FillMaterials.hh"
#include "Interface/MiceMaterials.hh"
//#include "Applications/common/GlobalReader.hh"

#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

#ifdef G4VIS_USE
#include "MICEVisManager.hh"
#endif


int NumAllGridsMuonsDef;

// some dataCards

dataCards MyDataCards("Simulation");

static const char *cvs_id = "$Id: Simulation.cc,v 1.61 2010-10-22 08:54:02 rogers Exp $";
static const char *cvs_tag = "$Name:  $";

TextFileWriter* writer = NULL;
Reader* reader;

PersistClasses* pClasses;
PersistClasses* pClassesRF;

// The MICEEvent that is needed by the Simulation - this needs to be removed as a global variable ASAP!

MICEEvent simEvent;

// The MICEEvent that will be used for the generation of RF background

MICEEvent rfEvent;


int evtno = -1;

bool generateRF;

bool settingRF;
void writeEvent();




void  writeEvent()
{
  if(settingRF) return;
  ++evtno;

  if( evtno < 10 || ( evtno < 100 && evtno % 10 == 0 ) ||
      ( evtno < 1000 && evtno % 100 == 0 ) || evtno % 1000 == 0 )
    Squeak::mout(Squeak::info) << "Simulated event " << evtno << std::endl;

  // look for SpecialHits in the tracker reference planes
  // if they exist, make a ZustandVektor for each
  simEvent.rfData = BTPhaser::GetRFData();

  if( ! generateRF )
    for( unsigned int i = 0; i < simEvent.specialHits.size(); ++i )
      if( simEvent.specialHits[i]->GetStepType() == 1 )
        if( simEvent.specialHits[i]->GetStationNo() == 9001 ||
            simEvent.specialHits[i]->GetStationNo() == 9002 )
          simEvent.zustandVektors.push_back( new ZustandVektor( simEvent.specialHits[i] ) );

  // if we are generating RF hits, then only save the hits in the
  // tracker reference plane

  if( generateRF )
    stripSpecialHits( simEvent );


  writer->writeEvent();

  clearEvent( simEvent );
  clearEvent( rfEvent );

  if( reader )
    reader->readEvent();
}

//BUG!!!
//Hack for now - ReadLoop will read in persistency objects from filename
//classesToProcess should contain objects from PersistClasses::Blah
//if classes to Process is empty, will just get everything
//This is for use in MICEPrimaryGeneratorAction and will likely be removed for mice-2-4-0
std::vector<MICEEvent*> ReadLoop( std::string filename, std::vector<std::string> classesToProcess )
{
  MICEEvent dummy;
  std::vector<MICEEvent*> theEvents;
	PersistClasses* persistClasses = new PersistClasses( classesToProcess.size() == 0 );
	TextFileReader* textReader     = new TextFileReader(filename, dummy, NULL, *persistClasses); 
  for(unsigned int i=0; i<classesToProcess.size(); i++)
    persistClasses->setClassProcessing( classesToProcess[i] );
  persistClasses->dumpProcessing();
	while(textReader->IsOk())
	{
		textReader->readEvent();
    theEvents.push_back(deepCopyEvent(dummy));
    clearEvent(dummy);
	}
  delete theEvents.back(); //If text reader goes bad, last event is crud
  theEvents.pop_back();

	delete textReader;
	delete persistClasses;
  return theEvents;
}



