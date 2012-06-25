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

#include <algorithm>

#include "src/legacy/Interface/Squeal.hh"

#include "src/common_cpp/DataStructure/RunHeader.hh"
#include "src/common_cpp/DataStructure/RunFooter.hh"

#include "src/common_cpp/GlobalsHandling/PerRunDataBase.hh"
#include "src/common_cpp/GlobalsHandling/PerRunDataManager.hh"

namespace MAUS {

DataRunActionManager::DataRunActionManager() : _run_actions() {
}

DataRunActionManager::~DataRunActionManager() {
    for (size_t i = 0; i < _run_actions.size(); ++i) {
        delete _run_actions[i];
    }
}

void DataRunActionManager::StartOfRun(RunHeader* run_header) {
    for (size_t i = 0; i < _run_actions.size(); ++i) {
       _run_actions[i]->StartOfRun(run_header);
    }
}

void DataRunActionManager::EndOfRun(RunFooter* run_footer) {
    for (size_t i = 0; i < _run_actions.size(); ++i) {
       _run_actions[i]->EndOfRun(run_footer);
    }
}

void DataRunActionManager::PushBack(DataRunActionBase* datum) {
    if (datum == NULL) {
        throw(Squeal(Squeal::recoverable,
                     "Trying to append a NULL to the DataRunActionManager",
                     "DataRunActionManager::PushBack"));
    } else if (std::find(_run_actions.begin(), _run_actions.end(), datum) !=
                                                           _run_actions.end()) {
        throw(Squeal(Squeal::recoverable,
                 "Trying to append same item twice to the DataRunActionManager",
                 "DataRunActionManager::PushBack"));
    } else {
        _run_actions.push_back(datum);
    }
}

void DataRunActionManager::Swap(DataRunActionBase* current_item,
                                DataRunActionBase* new_item) {
    typedef std::vector<DataRunActionBase*>::iterator RunDataIt;
    RunDataIt place = std::find
                       (_run_actions.begin(), _run_actions.end(), current_item);
    if (place == _run_actions.end()) {
        throw(Squeal(Squeal::recoverable,
                     "Failed to find item in per run data list",
                     "DataRunActionManager::Swap"));
    }
    RunDataIt check = std::find
                           (_run_actions.begin(), _run_actions.end(), new_item);
    if (check != _run_actions.end()) {
        throw(Squeal(Squeal::recoverable,
                     "New item already in per run data list",
                     "DataRunActionManager::Swap"));
    }
    *place = new_item;
}
}

