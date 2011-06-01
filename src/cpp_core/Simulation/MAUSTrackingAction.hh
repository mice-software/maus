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
 *  Collect the steps from MAUSSteppingAction, then create
 *  tracks in the datastructure.
 *
 */

#ifndef _SRC_CPP_CORE_SIMULATION_MAUSTRACKINGACTION_HH_
#define _SRC_CPP_CORE_SIMULATION_MAUSTRACKINGACTION_HH_

#include <sstream>  //  combining strings and numbers

#include "json/json.h"  // for datastructure

#include "G4Track.hh"  //  arg to tracking action
#include "G4UserTrackingAction.hh"  //  inherit from

#include "src/cpp_core/Simulation/MAUSSteppingAction.hh"

namespace MAUS {

class MAUSTrackingAction : public G4UserTrackingAction {
 public:
    /** @brief Add a new track to the stepping action and fill with some
     *         track data
     */
    void PreUserTrackingAction(const G4Track*);

    /** @brief Put the final momentum of the track into the spill data
     */
    void PostUserTrackingAction(const G4Track*);
};

}  //  ends MAUS namespace

#endif  // _SRC_CPP_CORE_SIMULATION_MAUSTRACKINGACTION_HH_
