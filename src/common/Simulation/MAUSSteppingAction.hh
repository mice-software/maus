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
 *  This class adds steps to the mc/particle/track branch. Also kills any 
 *  particles that take more than MaxNSteps (suspect looping in this case).
 *
 */

// TODO(Rogers): The tracks should be stored on the run action (and cleared at
//               runaction time)

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
  /** @brief Cuts on step number and put steps into the track if requested
   *
   *  Called by Geant4 on every step. This will kill particles if they exceed
   *  MaxNumberOfSteps (set status to fStopAndKill). If KeepTracks flag is set,
   *  stores each step in "steps" branch of the track.
   *
   *  @params step the geant4 step that will be acted on
   */
  void UserSteppingAction(const G4Step* step);

  /** @brief Return the tracks for this spill
   *
   *  The track data for each spill are stored on the stepping action.
   *
   *  @returns the track data for the entire spill
   */
  Json::Value GetTracks() const {return _tracks;}

  /** @brief Set the tracks for this spill
   */
  void SetTracks(Json::Value tracks) {_tracks = tracks;}

  /** @brief Set to true to store every step (rather verbose)
   */
  void SetKeepTracks(bool willKeepTracks) {_keepTracks = willKeepTracks;}

  /** @brief Return the value of the KeepTracks flag
   */
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
