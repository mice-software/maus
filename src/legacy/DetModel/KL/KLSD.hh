// MAUS WARNING: THIS IS LEGACY CODE.
/*
** Modified example 1 from the GEANT4 distribution to simulate the
** MICE scintillating fiber tracker for the MICE proposal
** August 21, 2002
*/
#ifndef KLSD_h
#define KLSD_h 1

#include "Geant4/G4VSensitiveDetector.hh"
#include "Interface/KLHit.hh"

class G4Step;
class G4HCofThisEvent;
class MICEEvent;
class MiceModule;

/**
* KLSD is the sensitive detector for GEANT4 that handles the 
* KL calorimeter simulation.
**/

class KLSD : public G4VSensitiveDetector
{
  public:

      //! Constructor
      KLSD(  MICEEvent*, MiceModule* );

      //! Initialise this this instance with the G4 event information
      void Initialize( G4HCofThisEvent* );

      //! process all hits in this detector
      G4bool ProcessHits( G4Step*, G4TouchableHistory* );

      //! code that is called at the end of processing of an event
      void EndOfEvent( G4HCofThisEvent* );

  private:

        MICEEvent*      _event;

        MiceModule*     _module;
};

#endif
