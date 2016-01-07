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

#ifndef _MAP_MAPCPPEMRPLANEHITS_H_
#define _MAP_MAPCPPEMRPLANEHITS_H_

// C headers
#include <assert.h>
#include <json/json.h>

// C++ headers
#include <string>
#include <vector>
#include <map>
#include <algorithm>

// MAUS
#include "Utils/EMRChannelMap.hh"
#include "Utils/CppErrorHandler.hh"
#include "Utils/JsonWrapper.hh"
#include "Utils/DAQChannelMap.hh"
#include "Utils/Exception.hh"
#include "Interface/dataCards.hh"
#include "API/PyWrapMapBase.hh"
#include "API/MapBase.hh"
#include "DataStructure/Data.hh"
#include "DataStructure/Spill.hh"
#include "DataStructure/ReconEvent.hh"
#include "DataStructure/DAQData.hh"
#include "DataStructure/EMRDaq.hh"
#include "DataStructure/DBBSpillData.hh"
#include "DataStructure/EMREvent.hh"
#include "DataStructure/EMRSpillData.hh"

namespace MAUS {

/** @class MapCppEMRPlaneHits
 *
 * Reconstruct EMR data and create EMR plane hits
 * by running over the fADC and DBB data
 */

typedef std::vector<EMRBarHit>		EMRBarHitVector; 	// nHits elements

struct EMRPlaneHitTmp {
  EMRBarHitVector _barhits;
  int _orientation;
  int _time;
  int _deltat;
  int _charge;
  std::vector<int> _samples;
};

typedef std::vector<EMRPlaneHitTmp>	EMRPlaneHitVector;	// 48 elements
typedef std::vector<EMRPlaneHitVector>	EMREventPlaneHitVector;	// nTr elements

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

  void process_DBB(MAUS::EMRDaq EMRdaq,
		   EMREventPlaneHitVector& emr_events_tmp,
		   EMRBarHitVector& emr_bar_hits_tmp,
		   size_t nPartEvents) const;

  void process_fADC(MAUS::EMRDaq EMRdaq,
		    EMREventPlaneHitVector& emr_events_tmp,
		    size_t nPartEvents) const;

  void process_candidates(EMRBarHitVector emr_bar_hits_tmp,
		          EMREventPlaneHitVector& emr_candidates_tmp) const;

  void fill(MAUS::Spill *spill,
	    EMREventPlaneHitVector emr_events_tmp,
	    size_t nPartEvents) const;

  EMREventPlaneHitVector get_emr_events_tmp(size_t nPartEvts) const;

  // Maps
  EMRChannelMap _emrMap;

  // Detector parameters
  size_t _number_of_planes;
  size_t _number_of_bars;

  // Hit pre-selection cuts
  int _primary_deltat_low;
  int _primary_deltat_up;

  size_t _secondary_n_low;
  int _secondary_tot_low;
  int _secondary_bunching_width;
};
} // namespace MAUS

#endif // #define _MAP_MAPCPPEMRPLANEHITS_H_
