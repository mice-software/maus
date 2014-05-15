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


/** @class MapCppEMRPlaneHits
 * Reconstruct TOF data and creat Slab hits
 * by running over the TOF digits.
 *
 */

#ifndef _MAP_MAPCPPEMRPLANEHITS_H_
#define _MAP_MAPCPPEMRPLANEHITS_H_

// C headers
#include <assert.h>
#include <json/json.h>

// C++ headers
#include <string>
#include <vector>
#include <map>

#include "Utils/EMRChannelMap.hh"
#include "DataStructure/Data.hh"
#include "DataStructure/Spill.hh"
#include "DataStructure/ReconEvent.hh"
#include "DataStructure/DAQData.hh"
#include "DataStructure/EMRDaq.hh"
#include "DataStructure/DBBSpillData.hh"
#include "DataStructure/EMRBar.hh"
#include "DataStructure/EMRBarHit.hh"
#include "DataStructure/EMRPlaneHit.hh"
#include "DataStructure/EMREvent.hh"
#include "src/common_cpp/API/MapBase.hh"

namespace MAUS {

typedef std::vector<MAUS::EMRBarHit>    EMRBarHitsVector;
typedef std::vector<EMRBarHitsVector>   EMRBarVector;
typedef std::vector<EMRBarVector>       EMRPlaneVector;
typedef std::vector<EMRPlaneVector>     EMREventVector_4;

struct EMRPlaneData {
  int _orientation;
  int _charge;
  int _time;
  int _spill;
  int _deltat;
};


#define NUM_DBB_PLANES      48
#define NUM_DBB_CHANNELS    61

typedef std::vector<EMRPlaneData>                EMRPlaneHitsVector;
typedef std::vector<EMRPlaneHitsVector>          EMREventVector_2;

class MapCppEMRPlaneHits : public MapBase<MAUS::Data> {
 public:
  MapCppEMRPlaneHits();

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

  /** @brief process the data object
 *
 *  @param
 */
  void _process(MAUS::Data *data) const;

  void processDBB(MAUS::EMRDaq EMRdaq,
                  int nPartTrigger,
                  EMREventVector_2& emr_events_tmp2,
                  EMREventVector_4& emr_events_tmp4) const;
  void processFADC(MAUS::EMRDaq EMRdaq,
                   int nPartTrigger,
                   EMREventVector_2& emr_events_tmp2) const;
  void fill(MAUS::Spill *spill,
            int nPartTrigger, 
            EMREventVector_2& emr_events_tmp2,
            EMREventVector_4& emr_events_tmp4) const;

  EMREventVector_2 get_data_tmp2(int nPartEvts) const;
  EMREventVector_4 get_data_tmp4(int nPartEvts) const;

  EMRChannelMap _emrMap;

  int _trigger_window_lower;
  int _trigger_window_upper;
};
}

#endif
