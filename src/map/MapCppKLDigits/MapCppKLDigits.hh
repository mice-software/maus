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


/** @class MapCppKLDigits
 *  Reconstruct events by running over KL real data.
 *
 */

#ifndef _MAP_MAPCPPKLDIGITS_H_
#define _MAP_MAPCPPKLDIGITS_H_
// C headers
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <json/json.h>

// C++ headers
#include <cmath>  //  from old file?
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

// G4MICE from commonCpp
#include "Utils/KLChannelMap.hh"

namespace MAUS {
class MapCppKLDigits {

 public:
 /** @brief Sets up the worker
  *  @param argJsonConfigDocument a JSON document with
  *         the configuration.
  */
  bool birth(std::string argJsonConfigDocument);

 /** @brief Shutdowns the worker
  *  This takes no arguments and does nothing
  */
  bool death();

 /** @brief process JSON document
  *  @param document Receive a document with raw data and return
  *  a document with digits
  */
  std::string process(std::string document);

 private:
  std::string _classname;

  KLChannelMap _map;

  // Vector to hold the names of all detectors to be included
  // in the digits.
  std::vector<std::string> _stationKeys;

  bool SetConfiguration(std::string json_configuration);

  /** @brief makes digits
   *  @param xDocDetectorData Json document containing the individual
   *  detector data
   *  @param xDocTrig Json document containing the "trigger" info
   *  @param xDocPmtInfo A refernence to the Information to be included in a digit
   */
  Json::Value makeDigits(Json::Value xDocDetectorData, Json::Value xDocTrig);

  /** @brief Gets the information from the flash ADC branch and appends it to the digit
   *  @param xDocPartEvent One particle event from raw detector data
   */
  Json::Value getAdc(Json::Value xDocPartEvent);
};
}
#endif

