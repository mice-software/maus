/** @class  MAUSSteppingAction
 *  Geant4 calls this class per track step
 *
 *  @author Christopher Tunnell <c.tunnell1@physics.ox.ac.uk>
 *
 *  This will add the steps of tracks to the datastructure
 *
 *  Copyright 2010 c.tunnell1@physics.ox.ac.uk
 *
 */

#ifndef _COMPONENTS_MAP_MAUSSTEPPINGACTION_H_
#define _COMPONENTS_MAP_MAUSSTEPPINGACTION_H_

#include <json/json.h>
#include <G4Step.hh>
#include <G4StepPoint.hh>
#include <G4Track.hh>
#include <G4UserSteppingAction.hh>
#include <cmath>
#include <sstream>

namespace MAUS {

class MAUSSteppingAction : public G4UserSteppingAction {
 public:
  MAUSSteppingAction();

  void UserSteppingAction(const G4Step*);

  static MAUSSteppingAction* GetSteppingAction() {
    return _self;
  }

  Json::Value GetTrack() {return _track;}

  Json::Value GetTrack(Json::Value track) {_track = track;}

 private:
  static MAUSSteppingAction* _self;
  Json::Value _track;
};

}  //  ends MAUS namespace

#endif  //  _COMPONENTS_MAP_MAUSSTEPPINGACTION_H_
