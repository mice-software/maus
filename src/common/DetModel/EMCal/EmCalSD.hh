/*
** Modified example 1 from the GEANT4 distribution to simulate the
** MICE scintillating fiber tracker for the MICE proposal
** Ed McKigney - August 21, 2002
*/
#ifndef EmCalSD_h
#define EmCalSD_h 1

#include "G4VSensitiveDetector.hh"
#include "Interface/EmCalHit.hh"

#include "Interface/Memory.hh" 

class G4Step;
class G4HCofThisEvent;
class MICEEvent;
class MiceModule;

/**
* EmCalSD is the sensitive detector for GEANT4 that handles the 
* calorimeter simulation.
**/

class EmCalSD : public G4VSensitiveDetector
{
  public:

      //! Constructor
      EmCalSD(  MICEEvent*, MiceModule* );

      //! Default destructor
     ~EmCalSD();

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
