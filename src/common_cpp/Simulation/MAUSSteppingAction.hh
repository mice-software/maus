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
#include <vector>

#include "json/json.h"

#include "Geant4/G4Step.hh"
#include "Geant4/G4StepPoint.hh"
#include "Geant4/G4Track.hh"
#include "Geant4/G4UserSteppingAction.hh"

#include "src/common_cpp/DataStructure/Step.hh"

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
   *  @returns the stepping data for the entire spill. MAUSSteppingAction still
   *  owns the memory for Steps
   */
  std::vector<Step>* GetSteps() const {return _steps;}

  /** @brief Return the steps for this spill
   *
   *  @returns the stepping data for the entire spill. Caller takes
   *  ownership of the memory for Steps
   */
  std::vector<Step>* TakeSteps();

  /** @brief Set the steps for this spill
   *
   *  @params steps vector of steps, usually empty; MAUSSteppingAction takes
   *  ownership of this memory
   */
  void SetSteps(std::vector<Step>* steps);

  /** @brief Set to true to store every step (rather verbose)
   */
  void SetWillKeepSteps(bool willKeepSteps) {_keepSteps = willKeepSteps;}

  /** @brief Return the value of the KeepSteps flag
   */
  bool GetWillKeepSteps() const {return _keepSteps;}

  /** @brief Convert from G4StepPoint to MAUS datastructure
   *
   *  @brief point Convert either the prestep or poststep of point
   *  @brief prestep Set to true to convert the PreStepPoint; set to false to
   *         convert the PostStepPoint
   */
  Step StepToMaus(const G4Step* point, bool prestep) const;


 private:
  std::vector<Step>* _steps;
  bool _keepSteps;
  int _maxNSteps;

  // disallow...
  MAUSSteppingAction& operator=(const MAUSSteppingAction&);
  MAUSSteppingAction(const MAUSSteppingAction&);
};

}  //  ends MAUS namespace

#endif  //  _SRC_CPP_CORE_SIMULATION_MAUSSTEPPINGACTION_HH_
