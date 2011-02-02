/*
** MICE Time-of-Flight detector for the MICE proposal
** Steve Kahn - 14 Sep, 2002
*/
#ifndef _TOFSD_HH_
#define _TOFSD_HH_ 1

#include "DetModel/MAUSSD.h"
#include "G4VSensitiveDetector.hh"
#include "Interface/TofHit.hh"
#include "Interface/Memory.hh"

#include <json/json.h>
#include <string>

using namespace MAUS;

class G4Step;
class G4HCofThisEvent;
class MICEEvent;
class MiceModule;

class TofSD : public MAUSSD {
 public:
  TofSD( MiceModule* , bool dEdxCut);

  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);
  
};

#endif
