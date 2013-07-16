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

#ifndef _REDUCECPPPATTERNRECOGNITION_H
#define _REDUCECPPPATTERNRECOGNITION_H

// C++ includes
#include <string>
#include <vector>
#include <map>

#include "json/json.h"

// MAUS includes
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/Recon/SciFi/TrackerDataManager.hh"
#include "src/common_cpp/Recon/SciFi/TrackerDataPlotterBase.hh"
#include "src/common_cpp/Recon/SciFi/TrackerDataPlotterInfoBox.hh"
#include "src/common_cpp/Recon/SciFi/TrackerDataPlotterXYZ.hh"

namespace MAUS {

class ReduceCppPatternRecognition {

 public:

  /** @brief Sets up the worker
   *  @param argJsonConfigDocument a JSON document with the configuration.
   */
  bool birth(std::string aJsonConfigDocument);

  /** @brief Shutdowns the worker, does nothing */
  bool death();

  /** @brief Process JSON document */
  std::string process(std::string aDocument);

  /** @brief Return the spill object */
  MAUS::Spill* get_spill() { return mSpill; }

  /** @brief Takes json data and returns a Spill
  *   @param json_data a string holding spill's worth of data in json format
  */
  bool read_in_json(std::string aJsonData);

 private:

  std::string mClassname;
  Json::Value mRoot;
  Spill *mSpill;

  TrackerDataManager mDataManager;
  TrackerDataPlotterBase *mXYZPlotter;
  TrackerDataPlotterBase *mInfoBoxPlotter;
  std::vector<TrackerDataPlotterBase*> mPlotters;
};

} // ~namespace MAUS

#endif

