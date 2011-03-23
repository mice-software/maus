/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/** @class  MAUSSteppingAction
 *  Geant4 calls this class per track step
 *
 *  @author Christopher Tunnell <c.tunnell1@physics.ox.ac.uk>
 *  @author Chris Rogers <chris.rogers@stfc.ac.uk>
 *
 *  This class adds steps to the mc/particle/track branch. Also kills any 
 *  particles that take more than MaxNSteps (suspect looping in this case).
 *
 *  Copyright 2010-2011 c.tunnell1@physics.ox.ac.uk, chris.rogers@stfc.ac.uk
 *
 */

// TODO (Rogers): BUG _maxNSteps is hardcoded at 100000
//                Make into a proper singleton class

#ifndef _COMPONENTS_MAP_MAUSSTEPPINGACTION_H_
#define _COMPONENTS_MAP_MAUSSTEPPINGACTION_H_

#include <cmath>
#include <sstream>

#include <json/json.h>

#include <G4Step.hh>
#include <G4StepPoint.hh>
#include <G4Track.hh>
#include <G4UserSteppingAction.hh>

namespace MAUS {

class MAUSSteppingAction : public G4UserSteppingAction {
 public:
  MAUSSteppingAction();

  void UserSteppingAction(const G4Step*);

  static MAUSSteppingAction* GetSteppingAction() {
    return _self;
  }

  Json::Value GetTracks() {return _tracks;}

  void SetTracks(Json::Value tracks) {_tracks = tracks;}

 private:
  static MAUSSteppingAction* _self;
  Json::Value _tracks;
  int _maxNSteps;
};

}  //  ends MAUS namespace

#endif  //  _COMPONENTS_MAP_MAUSSTEPPINGACTION_H_
