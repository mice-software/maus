/*
** MICE Time-of-Flight detector for the MICE proposal
** Steve Kahn - 14 Sep, 2002
*/
#ifndef _TOFSD_HH_
#define _TOFSD_HH_ 1

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

class TofSD : public G4VSensitiveDetector
{
  public:

      TofSD( MICEEvent*, MiceModule* , bool dEdxCut);

     ~TofSD();

      void Initialize(G4HCofThisEvent*);

      G4bool ProcessHits(G4Step*, G4TouchableHistory*);

      void EndOfEvent(G4HCofThisEvent*);

  std::string GetEvent() { return _event_document; }

  private:

	MICEEvent*	_event;

  std::string _event_document;
  Json::Value _root;   // will contains the root value after parsing.
  Json::Reader _reader;

	MiceModule*	_module;

	TofHit		_hit;
	bool            _dEdxCut;

};

#endif
