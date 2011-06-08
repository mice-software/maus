/* 
*/

#include <string.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include "CLHEP/Vector/ThreeVector.h"
#include "KLRec.hh"
#include "Interface/dataCards.hh"
#include "Interface/MICEEvent.hh"
#include "Config/MiceModule.hh"
#include "Interface/MICERun.hh"

#include "Interface/KLDigit.hh"
#include "KLCellHit.hh"
#include "KLChannelMap.hh"

KLRec::KLRec( MICEEvent& e, MICERun* run ) : theEvent( e )
{
  m_run = run;
} 

KLRec::~KLRec()
{
}

void KLRec::Process( )
{
	char* MICEFILES = getenv( "MICEFILES" );
	// the full name of the input file:
	std::string fnam = std::string( MICEFILES ) + "/Cabling/KL/" + "klcabling.txt";
	KLChannelMap *klmap = new KLChannelMap(fnam);
	//klmap->Print();

	int fadcHits = theEvent.vmefAdcHits.size();
	for(int k=0; k<fadcHits;k++)
	{
	   VmefAdcHit* fadchit = theEvent.vmefAdcHits[k];
	   if( klmap->isThisKLFadc(fadchit) )
	   {
	     int  cell, pmt, pmtNum;
        pmtNum = klmap->findThePosition(fadchit, cell, pmt);

		  // create a KLDigit
	     KLDigit* klDigit = new KLDigit(fadchit, pmt, cell);

	     //klDigit->Print();
	     theEvent.klDigits.push_back( klDigit );
	   }
	}
	
	if(theEvent.klDigits.size() > 1) makeCellHits();

}


void KLRec::makeCellHits()
{
  std::vector<const MiceModule*> klModules = m_run->miceModule->findModulesByPropertyExists( "Int", "Cell" );

//  for( unsigned int i = 0; i < klModules.size(); ++i )
  for( unsigned int i = 0; i < 21; ++i )
    {
     if( klModules[i]->propertyExistsThis( "numPMTs", "int" ) )
        {
          int cell = klModules[i]->propertyInt( "Cell" );
          int numPmts = klModules[i]->propertyInt( "numPMTs" );

          if( numPmts == 2 )
            {
              for( unsigned int a = 0; a < theEvent.klDigits.size(); ++a )
                if( theEvent.klDigits[a]->GetCell()  == cell )
                  for( unsigned int b = a + 1; b < theEvent.klDigits.size(); ++b )
                    if( theEvent.klDigits[b]->GetCell()  == cell &&
                        theEvent.klDigits[a]->GetSide() != theEvent.klDigits[b]->GetSide())
                      {

                        KLCellHit* hit = new KLCellHit(m_run, klModules[i], theEvent.klDigits[a], theEvent.klDigits[b] );


                       if( hit->isGood() )
                        {
                           hit->digit1()->setCellHit( hit );
                           hit->digit2()->setCellHit( hit );
                           theEvent.klCellHits.push_back( hit );
									//hit->Print(1);
                         }
                       else delete hit;
                     }
            }
            else if( numPmts == 1 )
            {
              for( unsigned int a = 0; a < theEvent.klDigits.size(); ++a )
                if( theEvent.klDigits[a]->GetCell()  == cell )
                  {
                    KLCellHit* hit = new KLCellHit(m_run, klModules[i], theEvent.klDigits[a] );
                    if( hit->isGood() )
                      {
                        hit->digit1()->setCellHit( hit );
                        theEvent.klCellHits.push_back( hit );
                      }
                  }
                  }
        }
    }
}
