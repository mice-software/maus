/* 
*/

#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include "CLHEP/Vector/ThreeVector.h"
#include "Recon/Ckov/CkovRec.hh"
#include "Interface/dataCards.hh"
#include "Interface/MICEEvent.hh"
#include "Interface/MICERun.hh"

#include "Interface/CkovDigit.hh"

CkovRec::CkovRec( MICEEvent& e, MICERun* run ) : theEvent( e )
{
  m_run = run;
  char* MICEFILES = getenv( "MICEFILES" );
  std::string ChannelMapFile = std::string( MICEFILES ) + "/Cabling/Ckov/" + "ckovcabling.txt";
  chMap = new CkovChannelMap( ChannelMapFile );
} 

void CkovRec::Process( )
{
  if( ( theEvent.ckovDigits.size() == 0 ) && ( theEvent.vmefAdcHits.size() != 0 ) ){
	MakeDigits();

  } else { std::cout << "CkovRec: No fADC hits in the event. Cannot create Ckov Digits!" << std::endl; }
}

CkovRec::~CkovRec()
{
}

// Create CkovDigits from TDC and fADC hits
void CkovRec::MakeDigits(){
  int tdcHits =        theEvent.vmeTdcHits.size(); 
  int fadcHits =       theEvent.vmefAdcHits.size();      
  int refValue = 0;
  int tdcCount = 0;
  VmeTdcHit * tdchit;
  VmefAdcHit * fadchit;

  //Get the TDC reference value
  int i = 0;
  while( i < tdcHits ){
    tdchit = theEvent.vmeTdcHits[i];
    if( ( tdchit->crate() == REFERENCE_VALUE_CRATE ) && 
	( tdchit->board() == REFERENCE_VALUE_BOARD ) && 
	( tdchit->channel() == REFERENCE_VALUE_CHANNEL ) ){
      refValue = tdchit->tdc();
      std::cout << "REF VALUE = " <<  refValue << std::endl;
      i = tdcHits;
    } else ++i;
  }
  for( i = 0; i < fadcHits; ++i ) {
    fadchit = theEvent.vmefAdcHits[ i ];
    if( chMap->isThisCkovFadc( fadchit ) ){
      int plane, pmt, pmtNum;
      pmtNum = chMap->findThePosition( fadchit, plane, pmt );
      if( pmtNum == 99 || pmtNum == -99 ) continue;
      std::vector< int > tdcMap;
      tdcMap.resize( tdcHits, 0 );
      /// Check if the hit is unique
      for( unsigned int j=0; j < theEvent.ckovDigits.size(); ++j )
	if( pmtNum == chMap->findThePosition( theEvent.ckovDigits[ j ]->vmefAdcHit() ) ){
	  std::cout << "NOT NEW!!!" << std::endl;
	  fadchit->Print();
	  theEvent.ckovDigits[ j ]->vmefAdcHit()->Print();
	}
      tdcCount = 0;                   
      tdchit = NULL;
      while( ( tdcCount < tdcHits ) && (!tdchit) ){
	if( pmtNum == chMap->findThePosition( theEvent.vmeTdcHits[ tdcCount ] ) ) {
	  if( tdcMap[ tdcCount ] == 0 ){
	    tdchit = theEvent.vmeTdcHits[ tdcCount ];
	    tdcMap[ tdcCount ] = i;
	  } else { 
	    std::cout << "This TDC hit was already used with this fAdc hit: " << std::endl;
	    theEvent.ckovDigits[ tdcMap[ tdcCount ] ]->vmefAdcHit()->Print();
	    std::cout << "Current fAADC hit is: " << std::endl;
	    fadchit->Print();
	  }
	}
	++tdcCount;
      }
      CkovDigit* ckovDigit;
      if( tdchit ) ckovDigit = new CkovDigit( fadchit, tdchit, pmtNum, plane );
      else ckovDigit = new CkovDigit( fadchit, pmtNum, plane );
      theEvent.ckovDigits.push_back( ckovDigit );
    }
  }
  

}
