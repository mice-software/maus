/* 
*/

#ifndef KLREC_HH
#define KLREC_HH

#include "CLHEP/Vector/ThreeVector.h"
#include <fstream>
#include <vector>

#include "Interface/MICEEvent.hh"
#include "Interface/MICERun.hh"

/**
* KLRec is a class to manage the reconstruction of the KL detector
*/

class KLRec
{
public:

	//! constructor
        KLRec( MICEEvent&, MICERun* );

	//! Default destructor
	~KLRec();

	//! Perform the processing for a single event
	void Process();

private:

        MICEEvent& 	theEvent;
	     MICERun*	m_run;

		  void makeCellHits();

};

#endif
