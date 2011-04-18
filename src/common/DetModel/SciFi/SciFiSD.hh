/*
** Modified example 1 from the GEANT4 distribution to simulate the
** MICE scintillating fiber tracker for the MICE proposal
** Ed McKigney - August 21, 2002
*/
#ifndef SciFiSD_h
#define SciFiSD_h 1

#include <json/json.h>
#include "DetModel/MAUSSD.h"
#include "Interface/SciFiHit.hh"

using namespace MAUS;

/**
* SciFiSD is the sensitive detector for GEANT4 that handles the Scintillating
* Fibre tracker simulation.
**/

class SciFiSD : public MAUSSD
{
  public:

      //! Constructor
  SciFiSD(  MiceModule*);

      //! process all hits in this detector
      G4bool ProcessHits( G4Step*, G4TouchableHistory* );

      //! code that is called at the end of processing of an event
      void EndOfEvent( G4HCofThisEvent* );


};

#endif
