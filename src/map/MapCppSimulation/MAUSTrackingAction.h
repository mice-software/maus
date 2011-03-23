/** @class  MAUSTrackingAction
 *  Geant4 calls this class before/after tracks propogated
 *
 *  @author Christopher Tunnell <c.tunnell1@physics.ox.ac.uk>
 *
 *  Collect the steps from MAUSSteppingAction, then create
 *  tracks in the datastructure.
 *
 *  Copyright 2010 c.tunnell1@physics.ox.ac.uk
 *
 */

#ifndef _COMPONENTS_MAP_MAUSTRACKINGACTION_H_
#define _COMPONENTS_MAP_MAUSTRACKINGACTION_H_

#include <json/json.h>  // for datastructure
#include <G4Track.hh>  //  arg to tracking action
#include <G4UserTrackingAction.hh>  //  inherit from
#include <sstream>  //  combining strings and numbers

#include "MAUSSteppingAction.h"  // static method to grab steps


namespace MAUS {

class MAUSTrackingAction : public G4UserTrackingAction {
 public:
    void PreUserTrackingAction(const G4Track*);
    void PostUserTrackingAction(const G4Track*);
};

}  //  ends MAUS namespace

#endif  //  _COMPONENTS_MAP_MAUSTRACKINGACTION_H_
