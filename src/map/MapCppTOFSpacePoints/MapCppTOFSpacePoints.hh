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


/** @class MapCppTOFSpacePoints
 * Reconstruct TOF data and creat Slab hits
 * by running over the TOF slab hits.
 *
 */

#ifndef _MAP_MAPCPPTOFSPACEPOINTS_H_
#define _MAP_MAPCPPTOFSPACEPOINTS_H_

#include <string>
#include <vector>
#include <map>
#include "json/json.h"

#include "Utils/TOFCalibrationMap.hh"

class MapCppTOFSpacePoints {

 public:

 /** @brief Sets up the worker
 *
 *  @param argJsonConfigDocument a JSON document with
 *         the configuration.
 */
  bool birth(std::string argJsonConfigDocument);

  /** @brief Shutdowns the worker
 *
 *  This takes no arguments and does nothing.
 */
  bool death();

  /** @brief process JSON document
 *
 *  @param document Receive a document with slab hits and return
 *  a document with space points.
 */
  std::string process(std::string document);

 private:

  std::string _classname;

  TOFCalibrationMap _map;

  std::vector<int> xPlane0Hits;
  std::vector<int> xPlane1Hits;

  double _makeSpacePiontCut; // nanoseconds
  double _findTriggerPixelCut; // nanoseconds
  std::string _triggerStation;

  /// Vector to hold the names of all detectors to be processed.
  std::vector<std::string> _stationKeys;
  std::map<int, std::string> _triggerhit_pixels;

  Json::Value fillSpacePoint
                         (Json::Value &xDocSlabHit0, Json::Value &xDocSlabHit1);
  Json::Value processTOFStation
        (Json::Value &xSlabHits, std::string detector, unsigned int part_event);
  std::string findTriggerPixel(Json::Value xDocPartEvent);
  bool calibratePmtHit
                    (TOFPixelKey xPixelKey, Json::Value &xPmtHit, double &time);
  bool calibrateSlabHit
                   (TOFPixelKey xPixelKey, Json::Value &xSlabHit, double &time);

  /** @brief makes space points
   *
   *  @param xDocDetectorData Json document containing slab hits from 
   * one particle event in one individual detector.
   */
  Json::Value makeSpacePoints(Json::Value &xDocPartEvent);
  bool map_init;
};
#endif
