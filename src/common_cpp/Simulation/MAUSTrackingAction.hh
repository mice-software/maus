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

#ifndef _SRC_CPP_CORE_SIMULATION_MAUSTRACKINGACTION_HH_
#define _SRC_CPP_CORE_SIMULATION_MAUSTRACKINGACTION_HH_

#include <string>
#include <sstream>  //  combining strings and numbers

#include "json/json.h"  // for datastructure

#include "Geant4/G4Track.hh"  //  arg to tracking action
#include "Geant4/G4UserTrackingAction.hh"  //  inherit from

#include "src/common_cpp/DataStructure/Track.hh"

#include "src/common_cpp/Simulation/MAUSSteppingAction.hh"

namespace MAUS {

/** @class  MAUSTrackingAction
 *  Geant4 calls this class before/after tracks propogated
 *
 *  Collect the steps from MAUSSteppingAction, then create
 *  tracks in the datastructure.
 */
class MAUSTrackingAction : public G4UserTrackingAction {
 public:
    /** @brief constructor
     */
    MAUSTrackingAction();

    /** @brief Add a new track to the stepping action and fill with some
     *         track data
     */
    void PreUserTrackingAction(const G4Track*);

    /** @brief Put the final momentum of the track into the spill data
     */
    void PostUserTrackingAction(const G4Track*);

    /** @brief Set the tracks; MAUSTrackingAction takes ownership of the memory
     */
    void SetTracks(std::vector<Track>* tracks);

    /** @brief Get the tracks; MAUSTrackingAction still owns the memory
     */
    std::vector<Track>* GetTracks() const {return _tracks;}

    /** @brief Get the tracks; caller takes ownership of the memory
     */
    std::vector<Track>* TakeTracks();

    /** @brief Choose whether to store tracks
     *
     *  @params willKeepTracks if true will keep initial and final position and
     *          momentum of tracks and optionally steps, according to the flag
     *          in MAUSSteppingAction. If false, will keep neither tracks nor
     *          steps.
     */
    void SetWillKeepTracks(bool willKeepTracks) {_keepTracks = willKeepTracks;}

    /** @brief Returns true is tracks are stored.
     */
    bool GetWillKeepTracks() {return _keepTracks;}

    /** @brief If MAUS kills a particle, give the reason here
     */
    void SetKillReason(const G4Track* aTrack, std::string reason);

 private:
    std::vector<Track>* _tracks;
    bool _keepTracks;
    MAUSSteppingAction* _stepping;
};

}  //  ends MAUS namespace

#endif  // _SRC_CPP_CORE_SIMULATION_MAUSTRACKINGACTION_HH_
