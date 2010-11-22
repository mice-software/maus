/*
** MICE Time-of-Flight detector for the MICE proposal
** Steve Kahn - 14 Sep, 2002
*/
#ifndef _TOFSD_HH_
#define _TOFSD_HH_ 1

#include "DetModel/MAUSSD.hh"
#include "G4VSensitiveDetector.hh"
#include "Interface/TofHit.hh"
#include "Interface/Memory.hh"

#include <json/json.h>
#include <string>

class G4Step;
class G4HCofThisEvent;
class MICEEvent;
class MiceModule;
//class TofHit;

class TofSD : public MAUSSD {
 public:
  TofSD( MiceModule* , bool dEdxCut);

  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);

  //  bool isHit() { return _isHit; }
  //  Json::Value GetHit() { return _hit; }

  // private:
  //  Json::Value _hit;   // will contains the root value after parsing.
  //   bool _isHit;
  
  //  MiceModule*	_module;
  //  
  //  bool            _dEdxCut;
  
};

#endif
