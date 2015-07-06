// MAUS WARNING: THIS IS LEGACY CODE.
/*
** MICE Time-of-Flight detector for the MICE proposal
** 14 Sep, 2002
*/
#ifndef _TOFSD_HH_
#define _TOFSD_HH_ 1

#include <string>
#include "Geant4/G4VSensitiveDetector.hh"
#include "DetModel/MAUSSD.hh"
#include "src/common_cpp/DataStructure/Hit.hh"

class G4Step;
class G4HCofThisEvent;
class MICEEvent;
class MiceModule;

class TofSD : public MAUS::MAUSSD {
 public:
  TofSD( MiceModule* );

  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);
  bool isHit() {return _hits != NULL && _hits->size() > 0;}
  int GetNHits() {return _hits->size();}
  void ClearHits();
  void TakeHits(MAUS::MCEvent* event);
 private:
  std::vector<MAUS::TOFHit>* _hits;
  
};

#endif
