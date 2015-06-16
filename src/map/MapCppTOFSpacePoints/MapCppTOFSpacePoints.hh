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
#include "API/MapBase.hh"

namespace MAUS {

class MapCppTOFSpacePoints : public MapBase<MAUS::Data> {

 public:
  MapCppTOFSpacePoints();

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
 *  @param document Receive a document with slab hits and return
 *  a document with space points.
 */
  void _process(MAUS::Data *data) const;

 private:

  TOFCalibrationMap _map;

  double _makeSpacePointCut; // nanoseconds
  double _findTriggerPixelCut; // nanoseconds
  std::string _triggerStation;
  int _trigStn;

  /// Vector to hold the names of all detectors to be processed.
  std::vector<std::string> _stationKeys;

  void fillSpacePoint(
                      TOFSpacePoint &tofSp,
                      TOFSlabHit &xDocSlabHit0,
                      TOFSlabHit &xDocSlabHit1) const;
  void processTOFStation(
                          MAUS::TOF0SlabHitArray* tof0slabhits,
                          MAUS::TOF0SpacePointArray* tof0sp,
                          std::string detector,
                          unsigned int part_event,
                          std::map<int, std::string>& _triggerhit_pixels) const;

  std::string findTriggerPixel(TOF0SlabHitArray* tof0lslabhitptr,
                               std::vector<int> xPlane0Hits,
                               std::vector<int> xPlane1Hits) const;
  template<typename T>
  bool calibratePmtHit
              (TOFPixelKey xPixelKey, T xPmtHit, double &time) const;
  bool calibrateSlabHit
             (TOFPixelKey xPixelKey, TOFSlabHit &tslh, double &time) const;

  /** @brief makes space points
   *
   *  @param xDocDetectorData Json document containing slab hits from 
   * one particle event in one individual detector.
   */
  void makeSpacePoints(
                          MAUS::TOF0SlabHitArray* slHitArrayPtr,
                          MAUS::TOF0SpacePointArray* spArrayPtr,
                          std::vector<int> xPlane0Hits,
                          std::vector<int> xPlane1Hits,
                          std::map<int, std::string>& triggerhit_pixels) const;
  bool _map_init;
  int runNumberSave;
  void getTofCalib(int rnum);
  Json::Value configJSON;
};
}

#endif
