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


/** @class MapCppKLCellHits
 * Reconstruct KL data and creat Cell hits
 * by running over the KL digits.
 *
 */

#ifndef _MAP_MAPCPPKLCELLHITS_H_
#define _MAP_MAPCPPKLCELLHITS_H_

#include <string>
#include <vector>
#include <map>
#include "json/json.h"
#include "src/common_cpp/API/MapBase.hh"
#include "src/common_cpp/Utils/KLChannelMap.hh"

namespace MAUS {
class MapCppKLCellHits : public MapBase<Json::Value>{

 public:
 MapCppKLCellHits();

 private:
 /** @brief Sets up the worker
 *
 *  @param argJsonConfigDocument a JSON document with
 *         the configuration.
 */
  void _birth(const std::string& argJsonConfigDocument);

  /** @brief Shutdowns the worker
 *
 *  This takes no arguments and does nothing.
 */
  void _death();

  /** @brief process JSON document
 *
 *  @param document Receive a document with digits and return
 *  a document with slab hits.
 */
  void _process(Json::Value* data) const;

  /// Vector to hold the names of all detectors to be processed.
  std::vector<std::string> _stationKeys;

  Json::Value fillCellHit(Json::Value xDocDigit0, Json::Value xDocDigit1) const;

  /** @brief makes slab hits
   *
   *  @param xDocDetectorData Json document containing digits from 
   * one particle event in one individual detector.
   */
  Json::Value makeCellHits(Json::Value xDocPartEvent) const;
};
}
#endif
