/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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

#ifndef _SRC_COMMON_CPP_GLOBALSHANDLING_RUNACTIONMANAGER_HH_
#define _SRC_COMMON_CPP_GLOBALSHANDLING_RUNACTIONMANAGER_HH_

#include <vector>

namespace MAUS {

class RunActionBase;
class RunHeader;
class RunFooter;

/** @class RunActionManager is responsible for objects that need to be
 *  updated every data run.
 *
 *  RunActionManager maintains a list of objects (RunActions) that need
 *  to be updated every run (interface class is RunActionBase). Get/Set
 *  methods are provided for some specific items.
 *
 *  RunActionManager has two functions - StartOfRun calls each of the
 *  RunActionBase StartOfRun methods in turn, while EndOfRun calls
 *  each of the RunActionBase EndOfRun methods in turn.
 *
 *  The only communication between worker nodes in MAUS is the Spill, so MAUS
 *  calls EndOfRun then StartOfRun when it detects a change in the spill
 *  number. An additional call to EndOfRun is made before exiting.
 */

class RunActionManager {
  public:
    /** Initialises everything to NULL
     */
    RunActionManager();

    /** Deletes all RunActionBase objects in the run_data vector (should be
     *  everything then)
     */
    ~RunActionManager();

    /** Calls the StartOfRun method on each of the RunActions
     *
     *  StartOfRun methods are called in the order items are loaded into the
     *  _run_actions vector.
     */
    void StartOfRun(RunHeader* run_header);

    /** Calls the EndOfRun method on each of the RunActions.
     *
     *  EndOfRun methods are called in the order items are loaded into the
     *  _run_actions vector.
     */
    void EndOfRun(RunFooter* run_footer);

    /** Replace current_item with new_item in run_data vector.
     *
     *  If current_item is not in the run_data vector, throw a
     *  Squeal. If new_item is already in the run_data vector or is NULL, throw
     *  a Squeal.
     *
     *  Memory allocated to current_item is still allocated, but no longer owned
     *  by RunActionManager (i.e. now owned by caller).
     */
    void Swap(RunActionBase* current_item, RunActionBase* new_item);

    /** Add item to the RunActionManager
     *
     *  RunActionManager now owns this memory (will clean it on delete). Throw
     *  a Squeal if item is already in the run_actions or item is NULL.
     */
    void PushBack(RunActionBase* item);

  private:
    std::vector<RunActionBase*> _run_actions;

    RunActionManager(const RunActionManager& run);
    RunActionManager& operator=(const RunActionManager& run);
};
}

#endif
