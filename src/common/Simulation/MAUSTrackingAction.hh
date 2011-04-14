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

#ifndef _SRC_COMMON_SIMULATION_MAUSTRACKINGACTION_H_
#define _SRC_COMMON_SIMULATION_MAUSTRACKINGACTION_H_

#include <sstream>  //  combining strings and numbers

#include <json/json.h>  // for datastructure

#include <G4Track.hh>  //  arg to tracking action
#include <G4UserTrackingAction.hh>  //  inherit from

#include "MAUSSteppingAction.hh"  // static method to grab steps


namespace MAUS {

class MAUSTrackingAction : public G4UserTrackingAction {
 public:
    void PreUserTrackingAction(const G4Track*);
    void PostUserTrackingAction(const G4Track*);
};

}  //  ends MAUS namespace

#endif  // _SRC_COMMON_SIMULATION_MAUSTRACKINGACTION_H_
