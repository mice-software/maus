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

#ifndef _SRC_MAP_MAPCPPSTARTOFRUN_MAPCPPSTARTOFRUN_HH_
#define _SRC_MAP_MAPCPPSTARTOFRUN_MAPCPPSTARTOFRUN_HH_

#include <string>

namespace MAUS {

/** MapCppStartOfRun looks for the start of a run and calls RunActionManager
 *
 *  MapCppStartOfRun examines the spill_number data item. If it has changed,
 *  MapCppStartOfRun calls RunActionManager::StartOfRun to update the per run
 *  objects like field maps and calibrations.
 */
class MapCppStartOfRun {
  public:
    /** Constructor initialises first to true (to indicate first run) */
    MapCppStartOfRun();

    /** Destructor does nothing */
    ~MapCppStartOfRun();

    /** Does nothing and returns true */
    bool birth(std::string argJsonConfigDocument);

    /** Does nothing and returns true */
    bool death();

    /** Calls the StartOfRun
     *
     *  Initialises the RunHeader; calls StartOfRun to initialise per-run data;
     *  hands the RunHeader to Go for processing into the output stream.
     */
    std::string process(std::string document);

  private:
    int _last_run_number;
    bool _first;  // true for the first spill of the first run only
    std::string _classname;
};
}  // namespace MAUS

#endif  // _SRC_MAP_MAPCPPSTARTOFRUN_MAPCPPSTARTOFRUN_HH_

