/* 
*/

#include <string.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "CLHEP/Vector/ThreeVector.h"
#include "EmCalRec.hh"
#include "Interface/dataCards.hh"
#include "Interface/MICEEvent.hh"
#include "Interface/MICERun.hh"
#include "Interface/EmCalTrack.hh"
#include "Interface/EmCalDigit.hh"
#include "Interface/EmCalHit.hh"
#include "Config/PidFits.hh"
#include "EmCalChannelMap.hh"
#include "Interface/VmefAdcHit.hh"

EmCalRec::EmCalRec( MICEEvent& e, MICERun* run ) : theEvent( e )
{
  m_run = run;
  PidFits* fits = PidFits::getInstance();
  m_cellLength = fits->getEmCalCellLengths();
  m_layers = fits->getNumCalLayers();
  m_highLvlTriggerSW = fits->getAverageADC(0.15); // [adc counts/mm]
  m_highLvlTriggerKL = fits->getAverageADC(0.0725); // [adc counts/mm]
} 

EmCalRec::~EmCalRec()
{
  for( unsigned int i = 0; i < theEvent.emCalTracks.size(); ++i )
    if (theEvent.emCalTracks[i]) delete theEvent.emCalTracks[i];

  theEvent.emCalTracks.clear();
}

void EmCalRec::Process( )
{
  if(theEvent.vmefAdcHits.size()==0){
    EmCalTrack* aTrack = 
       new EmCalTrack(theEvent.emCalDigits, m_cellLength, m_highLvlTriggerSW, m_highLvlTriggerKL);
    theEvent.emCalTracks.push_back(aTrack);
  }
  else{
	char* MICEFILES = getenv( "MICEFILES" );
	// the full name of the input file:
	std::string fnam = std::string( MICEFILES ) + "/Cabling/EmCal/" + "emcalcabling.txt";   

	EmCalChannelMap * emCalmap = new EmCalChannelMap(fnam);
//    	emCalmap->Print();
	int fadcHits = theEvent.vmefAdcHits.size();      
	for(int k=0; k<fadcHits;k++)
	{
	   VmefAdcHit* fadchit = theEvent.vmefAdcHits[k];
	   if( emCalmap->isThisEmCalFadc(fadchit) )
	   {
	         int  cell, pmt, pmtNum;
        	 pmtNum = emCalmap->findThePosition(fadchit, cell, pmt);
	      	// create a EmCalDigit
	      	EmCalDigit* emCalDigit = new EmCalDigit(fadchit, pmt, cell, NULL, 0);
	       //emCalDigit->Print();
	      	theEvent.emCalDigits.push_back( emCalDigit );
	   }

	}
}

}
