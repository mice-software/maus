/* 
*/

#include <iostream>
#include <fstream>
#include <algorithm>
#include "CLHEP/Vector/ThreeVector.h"
#include "VirtualRec.hh"
#include "Interface/dataCards.hh"
#include "Interface/MICEEvent.hh"
#include "Interface/MICERun.hh"

VirtualRec::VirtualRec( MICEEvent& e, MICERun* run ) : theEvent( e )
{
  m_run = run;
} 

void VirtualRec::Process( )
{
}

VirtualRec::~VirtualRec()
{
}
