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

#ifndef _SRC_COMMON_CPP_GLOBALSHANDLING_PERRUNDATAMANAGER_HH_
#define _SRC_COMMON_CPP_GLOBALSHANDLING_PERRUNDATAMANAGER_HH_

#include <vector>

namespace MAUS {

class DataRunActionBase;
class RunHeader;
class RunFooter;

/** @class DataRunActionManager is responsible for objects that need to be
 *  updated every run.
 *
 *  DataRunActionManager maintains a list of objects (DataRunActions) that need
 *  to be updated every run (interface class is DataRunActionBase). Get/Set
 *  methods are provided for some specific items.
 *
 *  DataRunActionManager has two functions - StartOfRun calls each of the
 *  DataRunActionBase StartOfRun methods in turn, while EndOfRun calls
 *  each of the DataRunActionBase EndOfDataRun methods in turn.
 *
 *  The only communication between worker nodes in MAUS is the Spill, so MAUS
 *  calls EndOfDataRun then StartOfRun when it detects a change in the spill
 *  number. An additional call to EndOfRun is made before exiting.
 */

class DataRunActionManager {
  public:
    /** Initialises everything to NULL
     */
    DataRunActionManager();

    /** Deletes all DataRunActionBase objects in the run_data vector (should be
     *  everything then)
     */
    ~DataRunActionManager();

    /** Calls the StartOfRun method on each of the DataRunActions
     *
     *  StartOfRun methods are called in the order items are loaded into the
     *  _run_actions vector.
     */
    void StartOfRun(RunHeader* run_header);

    /** Calls the EndOfRun method on each of the DataRunActions.
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
     *  by DataRunActionManager (i.e. now owned by caller).
     */
    void Swap(DataRunActionBase* current_item, DataRunActionBase* new_item);

    /** Add item to the PerRunDataManager
     *
     *  PerRunDataManager now owns this memory (will clean it on delete). Throw
     *  a Squeal if item is already in the run_actions or item is NULL.
     */
    void PushBack(DataRunActionBase* item);

  private:
    std::vector<DataRunActionBase*> _run_actions;

    DataRunActionManager(const DataRunActionManager& run);
    DataRunActionManager& operator=(const DataRunActionManager& run);
};
}

#endif
