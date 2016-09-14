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

class MapCppTOFDigits : public MapBase<MAUS::Data> {

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

  /** @brief process the data object
 *
 *  @param
 */
  void _process(MAUS::Data *data) const;

 private:
  TOFChannelMap _map;

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
  void setTdc(MAUS::TOFDigit *digit, MAUS::V1290 &tdc) const;

  /** @brief Gets the information from the ADC data ascosiated with the current TDC data
   */
  bool findAdc(MAUS::TOFDigit *digit,
               MAUS::V1724Array &adc_hits) const throw(Exceptions::Exception);

  /** @brief  Gets the Trigger Request information from the TDC data
   * ascosiated with the current TDC data
   */
  bool findTriggerReq(MAUS::TOFDigit *digit,
                      MAUS::V1290 &tdc,
                      MAUS::V1290Array &tr_req_hits) const  throw(Exceptions::Exception);

  /** @brief  Gets the Trigger information from the TDC data
   * ascosiated with the current TDC data
   */
  bool findTrigger(MAUS::TOFDigit *digit,
                   MAUS::V1290 &tdc,
                   MAUS::V1290Array &tr_hits) const throw(Exceptions::Exception);
  bool map_init;
  Json::Value configJSON;
  int runNumberSave;
  void getTofCabling(int runNumber);
};
}

#endif

