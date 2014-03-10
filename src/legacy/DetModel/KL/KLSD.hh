// MAUS WARNING: THIS IS LEGACY CODE.
/*
** MICE Time-of-Flight detector for the MICE proposal
** 14 Sep, 2002
*/
#ifndef _KLSD_HH_
#define _KLSD_HH_ 1

#include "DetModel/MAUSSD.hh"
#include "Geant4/G4VSensitiveDetector.hh"
#include <json/json.h>
#include <string>

class G4Step;
class G4HCofThisEvent;
class MICEEvent;
class MiceModule;

class KLSD : public MAUS::MAUSSD {
 public:
  KLSD( MiceModule* );

  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);
  
};

#endif
