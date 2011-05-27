/* CKOVSD.hh
*/
#ifndef CKOVSD_h
#define CKOVSD_h 1

#include "G4VSensitiveDetector.hh"
#include "Interface/CkovHit.hh"
#include "CLHEP/Vector/ThreeVector.h"

class G4Step;
class G4HCofThisEvent;
class MICEEvent;
class MiceModule;

/**
* CKOVSD is the sensitive detector for GEANT4 that handles the CKOV detector
**/

class CkovSD : public G4VSensitiveDetector
{
  public:

      //! Constructor
      CkovSD(  MICEEvent*, MiceModule*, bool dEdxCut );

      //! Initialise this this instance with the G4 event information
      void Initialize( G4HCofThisEvent* );

      //! process all hits in this detector
      G4bool ProcessHits( G4Step*, G4TouchableHistory* );

      //! code that is called at the end of processing of an event
      void EndOfEvent( G4HCofThisEvent* );
      void ProcessAngles( CkovHit*, Hep3Vector, Hep3Vector, Hep3Vector );

  private:

        MICEEvent*      _event;
        MiceModule*     _module;
        bool            _dEdxCut;
};

#endif
