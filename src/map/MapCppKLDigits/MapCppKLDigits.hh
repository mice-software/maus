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
#include "Utils/KLCalibrationMap.hh"
#include "API/MapBase.hh"

namespace MAUS {
class MapCppKLDigits : public MapBase<MAUS::Data> {

 public:
  MapCppKLDigits();

 private:
 /** @brief Sets up the worker
  *  @param argJsonConfigDocument a JSON document with
  *         the configuration.
  */
  void _birth(const std::string& argJsonConfigDocument);

 /** @brief Shutdowns the worker
  *  This takes no arguments and does nothing
  */
  void _death();

 /** @brief process JSON document
  *  @param document Receive a document with raw data and return
  *  a document with digits
  */
  void _process(MAUS::Data* data) const;

 private:
  KLChannelMap _map;
  KLCalibrationMap _mapcal;

  // Vector to hold the names of all detectors to be included
  // in the digits.
  std::vector<std::string> _stationKeys;

  bool SetConfiguration(std::string json_configuration);

  /** @brief Gets the information from the flash ADC branch and appends it to the digit
   *  @param xDocPartEvent One particle event from raw detector data
   */
  bool getAdc(MAUS::KLDigit* digit, MAUS::V1724 &adc) const;
};
}
#endif

