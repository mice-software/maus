/* 
*/

#ifndef EMRREC_HH
#define EMRREC_HH

#include "CLHEP/Vector/ThreeVector.h"
#include <fstream>
#include <vector>

#include "Interface/MICEEvent.hh"
#include "Interface/MICERun.hh"

/**
* EMRRec is a class to manage the reconstruction of the EMR detector
*/

class EMRRec
{
public:

	//! constructor
        EMRRec( MICEEvent&, MICERun* );

	//! Default destructor
	~EMRRec();

	//! Perform the processing for a single event
	void Process();

private:

        MICEEvent& 	theEvent;
	MICERun*	m_run;
};

#endif
