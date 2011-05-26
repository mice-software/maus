/* 
** Written for the MICE proposal simulation
** Ed McKigney - Aug 21 2002
*/

#ifndef SCIFIREC_HH
#define SCIFIREC_HH

#include "CLHEP/Vector/ThreeVector.h"
#include <fstream>
#include <vector>

#include "Interface/MICEEvent.hh"
#include "Interface/MICERun.hh"

#include "Interface/SciFiDigit.hh"
#include "Calib/SciFiReconstructionParams.hh"
#include "Recon/SciFi/SciFiTrack.hh"
#include "Recon/SciFi/SciFiKalTrack.hh"
#include "Recon/SciFi/SciFiSpacePoint.hh"

// This is needed to use the Kalman Filter Package
#include "Recon/SciFi/SetupKalmanRec.hh"

class VlpcCable;
class VlpcCalib;
class SciFiBadChans;

/**
* SciFiRec is a class to manage the reconstruction of the Scintillating Fibre tracker.
*/

class SciFiRec
{
public:

	//! constructor
        SciFiRec( MICEEvent&, MICERun* );

	//! Default destructor
	~SciFiRec();

	//! Perform the processing for a single event
	void Process();

private:

	// Utility
	int active;

	// calibration and cabling info for real data
        VlpcCalib*		m_calib;
	VlpcCable*		m_cable;

        MICEEvent& 	theEvent;
	MICERun*	m_run;
};

#endif
