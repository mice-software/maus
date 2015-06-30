// MAUS WARNING: THIS IS LEGACY CODE.
/*
** MICE Time-of-Flight detector for the MICE proposal
** 14 Sep, 2002
*/
#ifndef _KLSD_HH_
#define _KLSD_HH_ 1

#include <string>
#include "Geant4/G4VSensitiveDetector.hh"
#include "src/common_cpp/DetModel/MAUSSD.hh"
#include "src/common_cpp/DataStructure/Hit.hh"
#include "src/common_cpp/DataStructure/MCEvent.hh"

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
  bool isHit() {return _hits != NULL && _hits->size() > 0;}
  int GetNHits() {return _hits->size();}
  void ClearHits();
  void GetHits(MAUS::MCEvent* event) {event->SetKLHits(_hits);}
 private:
  std::vector<MAUS::KLHit>* _hits;
};

#endif
