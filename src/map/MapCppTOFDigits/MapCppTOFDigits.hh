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


/** @class MapCppTOFDigits
 *  Reconstruct events by running over TOF real data.
 *
 */

#ifndef _MAP_MAPCPPTOFDIGITS_H_
#define _MAP_MAPCPPTOFDIGITS_H_
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
#include "Config/MiceModule.hh"  //  from old file?
#include "Utils/TOFChannelMap.hh"
#include "API/MapBase.hh"

namespace MAUS {

class MapCppTOFDigits : public MapBase<Json::Value> {

 public:
  MapCppTOFDigits();

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
  void _process(Json::Value* value) const;

 private:
  TOFChannelMap _map;

  // Vector to hold the names of all detectors to be included
  // in the digits.
  std::vector<std::string> _stationKeys;

  bool SetConfiguration(std::string json_configuration);

  /** @brief makes digits
   *  @param xDocDetectorData Json document containing the individual
   *  detector data
   *  @param xDocTrigReq Json document containing the "trigger_request" branch
   *  @param xDocTrig Json document containing the "trigger" branch
   *  @param xDocPmtInfo A refernence to the Information to be included in a digit
   */
  Json::Value makeDigits(Json::Value xDocDetectorData,
                         Json::Value xDocTrigReq,
                         Json::Value xDocTrig) const;

  /** @brief Gets the information from the TDC branch and appends it to the digit
   *  @param xDocPartEvent One particle event from raw detector data
   */
  Json::Value getTdc(Json::Value xDocPartEvent) const;

  /** @brief Gets the information from the ADC data ascosiated with the current TDC data
   *  @param xDocPartEvent One particle event from raw detector data
   *  @param xDocInfo Json document with the TDC digit information. This is needed
   *  so ascosiate the proper ADC data to corresponding TDC data.
   */
  bool getAdc(Json::Value xDocAdc,
              Json::Value xDocTdcHit,
              Json::Value &xDocDigit) const throw(Exception);

  /** @brief process JSON document
   *  @param xDocTrigReq Json document with the trigger request for a specific particle event
   *  @param xDocInfo 
   */
  bool getTrigReq(Json::Value xDocTrigReq, Json::Value xDocTdcHit,
                  Json::Value &xDocDigit) const throw(Exception);

  /** @brief process JSON document
   *  @param document Receive a document with raw data and return
   *  a document with digits
   */
  bool getTrig(Json::Value xDocTrig,
               Json::Value xDocTdcHit,
               Json::Value &xDocDigit) const throw(Exception);
  bool map_init;
};
}

#endif

