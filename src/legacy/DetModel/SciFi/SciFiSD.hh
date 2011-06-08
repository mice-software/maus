// MAUS WARNING: THIS IS LEGACY CODE.
#ifndef SciFiSD_h
#define SciFiSD_h 1

#include <json/json.h>
#include "DetModel/MAUSSD.hh"

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
