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

//                Make into a proper singleton class

#ifndef _SRC_MAP_MAUSSTEPPINGACTION_HH_
#define _SRC_MAP_MAUSSTEPPINGACTION_HH_

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
  void UserSteppingAction(const G4Step*);

  Json::Value GetTracks() const {return _tracks;}
  void SetTracks(Json::Value tracks) {_tracks = tracks;}

  void SetKeepTracks(bool willKeepTracks) {_keepTracks = willKeepTracks;}
  bool GetKeepTracks() const {return _keepTracks;}

  Json::Value StepPointToJson(const G4Step* point, bool prestep) const;

 private:
  friend class MAUSGeant4Manager;

  MAUSSteppingAction();

  Json::Value _tracks;
  bool _keepTracks;
  int _maxNSteps;
  int _currentNSteps;
};

}  //  ends MAUS namespace

#endif  //  _SRC_COMMON_MAUSSTEPPINGACTION_HH_
