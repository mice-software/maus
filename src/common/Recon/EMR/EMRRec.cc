/* 
*/

#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include "CLHEP/Vector/ThreeVector.h"
#include "Recon/EMR/EMRRec.hh"
#include "Interface/dataCards.hh"
#include "Interface/MICEEvent.hh"
#include "Interface/MICERun.hh"

#include "Interface/EMRDigit.hh"

EMRRec::EMRRec( MICEEvent& e, MICERun* run ) : theEvent( e )
{
  m_run = run;
} 

void EMRRec::Process( )
{
}

EMRRec::~EMRRec()
{
}
