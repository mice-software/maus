/* 
Global reconstruction/matching
*/

#ifndef GLOBALREC_HH
#define GLOBALREC_HH

#include "CLHEP/Vector/ThreeVector.h"
#include <fstream>
#include <vector>

#include "Interface/MICEEvent.hh"
#include "Interface/MICERun.hh"
#include "Interface/dataCards.hh"

/**
* GlobalRec is a class to manage the global reconstruction and matching
*/

class GlobalRec
{
public:

	//! Constructor 
	GlobalRec( MICEEvent&, MICERun* );
	      
	//! Default destructor
	~GlobalRec();

	//! Perform the processing for a single event
	void Process();

private:

  MICEEvent&  	theEvent;
  MICERun*	m_run;
};

#endif
