/* 
*/

#ifndef CKOVREC_HH
#define CKOVREC_HH

#include "CLHEP/Vector/ThreeVector.h"
#include <fstream>
#include <vector>

#include "Interface/MICEEvent.hh"
#include "Interface/MICERun.hh"

#include "CkovChannelMap.hh"

#define REFERENCE_VALUE_CRATE 1
#define REFERENCE_VALUE_BOARD 2
#define REFERENCE_VALUE_CHANNEL 0

/**
* CkovRec is a class to manage the reconstruction of the CKOV detector
*/

class CkovRec
{
public:

	//! constructor
        CkovRec( MICEEvent&, MICERun* );

	//! Default destructor
	~CkovRec();

	//! Perform the processing for a single event
	void Process();
  void MakeDigits();
private:

        MICEEvent& 	theEvent;
	MICERun*	m_run;
  CkovChannelMap * chMap; 
};

#endif
