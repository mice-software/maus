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

#ifndef _SRC_CPP_CORE_SIMULATION_MAUSSTEPPINGACTION_HH_
#define _SRC_CPP_CORE_SIMULATION_MAUSSTEPPINGACTION_HH_

#include <cmath>
#include <sstream>

#include "json/json.h"

#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4Track.hh"
#include "G4UserSteppingAction.hh"

namespace MAUS {

/** @class  MAUSSteppingAction
 *  Geant4 calls this class per track step
 *
 *  This class adds steps to the mc/particle/track branch. Also kills any 
 *  particles that take more than MaxNSteps (suspect looping in this case).
 *
 */
class MAUSSteppingAction : public G4UserSteppingAction {
 public:
  MAUSSteppingAction();

  /** @brief Cuts on step number and put steps into the track if requested
   *
   *  Called by Geant4 on every step. This will kill particles if they exceed
   *  MaxNumberOfSteps (set status to fStopAndKill). If KeepStep flag is set,
   *  stores each step in "steps" branch of the track.
   *
   *  @params step the geant4 step that will be acted on
   */
  void UserSteppingAction(const G4Step* step);

  /** @brief Return the steps for this spill
   *
   *  The track data for each spill are stored on the stepping action.
   *
   *  @returns the track data for the entire spill
   */
  Json::Value GetSteps() const {return _steps;}

  /** @brief Set the steps for this spill
   *
   *  @params steps Json value of array type
   */
  void SetSteps(Json::Value steps);

  /** @brief Set to true to store every step (rather verbose)
   */
  void SetWillKeepSteps(bool willKeepSteps) {_keepSteps = willKeepSteps;}

  /** @brief Return the value of the KeepSteps flag
   */
  bool GetWillKeepSteps() const {return _keepSteps;}

  /** @brief Convert from G4StepPoint to Json
   *
   *  @brief point Convert either the prestep or poststep of point
   *  @brief prestep Set to true to convert the PreStepPoint; set to false to
   *         convert the PostStepPoint
   */
  Json::Value StepToJson(const G4Step* point, bool prestep) const;


 private:
  Json::Value _steps;
  bool _keepSteps;
  int _maxNSteps;
};

}  //  ends MAUS namespace

#endif  //  _SRC_CPP_CORE_SIMULATION_MAUSSTEPPINGACTION_HH_
