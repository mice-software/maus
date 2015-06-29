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


/** @class Reducer used to plot to tracker data */

#ifndef _REDUCECPPRECONTESTING_H
#define _REDUCECPPRECONTESTING_H

// C++ includes
#include <string>
#include <vector>
#include <map>

#include "json/json.h"

// MAUS includes
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/Plotting/SciFi/TrackerDataManager.hh"
#include "src/common_cpp/Plotting/SciFi/TrackerDataPlotterBase.hh"
#include "src/common_cpp/Plotting/SciFi/TrackerDataPlotterInfoBox.hh"
#include "src/common_cpp/Plotting/SciFi/TrackerDataPlotterXYZ.hh"
#include "src/common_cpp/DataStructure/MCEvent.hh"
#include "TNtuple.h"

namespace MAUS {

class ReduceCppReconTesting {

 public:

  /** @brief Sets up the worker
   *  @param argJsonConfigDocument a JSON document with the configuration.
   */
  bool birth(std::string aJsonConfigDocument);

  /** @brief Shutdowns the worker, does nothing */
  bool death();

  /** @brief Process JSON document */
  std::string process(std::string document);

  /** @brief Return the spill object */
  MAUS::Spill* get_spill() { return _spill; }

  //~ /** @brief Takes json data and returns a Spill
  //~ *   @param json_data a string holding spill's worth of data in json format
  //~ */
  //~ bool read_in_json(std::string aJsonData);

 private:

  /**
   * @brief Checks whether the given TrackPoint belongs to a detector given by
   * the range of enums between min and max
   * 
   * @param track_point The TrackPoint to be checked
   * @param min the lower end of the range of enum values to check against
   * @param max the higher end of the range of enum values to check against
   */
  bool checkDetector(const MAUS::DataStructure::Global::TrackPoint* track_point,
                     int min, int max);

  /**
   * @brief Checks whether the given SpacePoint belongs to a detector given by
   * the range of enums between min and max
   * 
   * @param track_point The TrackPoint to be checked
   * @param min the lower end of the range of enum values to check against
   * @param max the higher end of the range of enum values to check against
   */
  bool checkDetector(const MAUS::DataStructure::Global::SpacePoint* space_point,
                     int min, int max);

  std::map<std::string, TNtuple*> _ntuples;
  std::string mClassname;
  //~ Json::Value mRoot;
  Spill* _spill;
};
      
} // ~namespace MAUS

#endif
