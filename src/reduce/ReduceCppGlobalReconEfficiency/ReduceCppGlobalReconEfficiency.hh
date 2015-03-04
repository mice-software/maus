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

#ifndef _REDUCECPPGLOBALRECONEFFICIENCY_H
#define _REDUCECPPGLOBALRECONEFFICIENCY_H

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

namespace MAUS {

class ReduceCppGlobalReconEfficiency {

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
  bool checkDetector(const MAUS::DataStructure::Global::TrackPoint* track_point,
                     int min, int max);
  bool checkDetector(const MAUS::DataStructure::Global::SpacePoint* space_point,
                     int min, int max);
 private:
  size_t tof0_matches;
  size_t tof0_matches_expected;
  size_t tof1_matches;
  size_t tof1_matches_expected;
  size_t tof2_matches;
  size_t tof2_matches_expected;
  size_t kl_matches;
  size_t kl_matches_expected;
  size_t emr_matches;
  size_t emr_matches_expected;
  std::string mClassname;
  //~ Json::Value mRoot;
  Spill* _spill;
};
      
} // ~namespace MAUS

#endif
