/* 
*/

#ifndef VIRTUALREC_HH
#define VIRTUALREC_HH

#include "CLHEP/Vector/ThreeVector.h"
#include <fstream>
#include <vector>

#include "Interface/MICEEvent.hh"
#include "Interface/MICERun.hh"

/**
* VirtualRec is a class to manage the reconstruction of the Virtual detector
*/

class VirtualRec
{
public:

	//! constructor
        VirtualRec( MICEEvent&, MICERun* );

	//! Default destructor
	~VirtualRec();

	//! Perform the processing for a single event
	void Process();

private:

        MICEEvent& 	theEvent;
	MICERun*	m_run;
};

#endif
