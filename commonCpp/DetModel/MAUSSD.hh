/** @class MAUSSD
 *  Generic class for MICE special detectors within the MC.
 *
 *  @author Christopher Tunnell <c.tunnell1@physics.ox.ac.uk>
 *
 *  Generic class for MICE special detectors.  Each special
 *  detector will produce a JSON::Value hit and also set
 *  a boolean flag as whether it was hit.
 *
 *  Copyright 2010 c.tunnell1@physics.ox.ac.uk
 *
 */
#ifndef _COMMONCPP_DETMODEL_MAUSSD_H_
#define _COMMONCPP_DETMODEL_MAUSSD_H_

#include <vector>
#include <json/json.h>   
#include <G4VSensitiveDetector.hh>
#include "Config/MiceModule.hh"

class MAUSSD : public G4VSensitiveDetector
{
 public:
  MAUSSD( MiceModule* , bool dEdxCut);

  virtual void Initialize(G4HCofThisEvent*) { /* do nothing */ }
  virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*) { return false; }
  virtual void EndOfEvent(G4HCofThisEvent*) {}

  bool isHit() { return _isHit; }
  int GetNHits() { return _hits.size(); }
  std::vector<Json::Value> GetHits() { return _hits; }

 protected:
  std::vector<Json::Value> _hits;  
  bool _isHit;
  
  MiceModule*	_module;
  
  bool            _dEdxCut;
};

#endif  //  _COMMONCPP_DETMODEL_MAUSSD_H_
