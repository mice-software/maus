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


/** @class MapCppEMRSpacePoints
 * Reconstruct EMR data and create EMR bar hits
 * by running over the fADC and DBB data
 */

#ifndef _MAP_MAPCPPEMRSPACEPOINTS_H_
#define _MAP_MAPCPPEMRSPACEPOINTS_H_

// C headers
#include <assert.h>
#include <json/json.h>
#include <cmath>
#include <utility>
#include <algorithm>

// C++ headers
#include <string>
#include <vector>
#include <map>

// MAUS
#include "Utils/EMRChannelMap.hh"
#include "Utils/EMRCalibrationMap.hh"
#include "Utils/EMRAttenuationMap.hh"
#include "Utils/EMRGeometryMap.hh"
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
#include "DataStructure/ThreeVector.hh"

namespace MAUS {

typedef std::vector<EMRBarHit>		EMRBarHitVector; 	// nHits elements

struct EMRSpacePointTmp {
  ThreeVector 		_pos;
  ThreeVector 		_errors;
  int 			_ch;
  double 		_time;
  double 		_deltat;
  double 		_chargema;
  double 		_chargesa;
};

typedef std::vector<EMRSpacePointTmp>	EMRSpacePointVector;	// nHits elements

struct EMRPlaneTmp {
  EMRBarHitVector	_barhits;
  EMRSpacePointVector 	_spacepoints;
  int 			_plane;
  double		_charge;
};

typedef std::vector<EMRPlaneTmp>	EMRPlaneVector;		// nPlanes elements
typedef std::vector<EMRPlaneVector>	EMREventVector;		// nTr elements

class MapCppEMRSpacePoints : public MapBase<MAUS::Data> {
 public:
  MapCppEMRSpacePoints();

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

  void clean_crosstalk(MAUS::Spill* spill,
		       EMREventVector& emr_events_tmp) const;

  void reconstruct_coordinates(EMREventVector& emr_events_tmp) const;

  void correct_charge(EMREventVector& emr_events_tmp,
		      size_t nPartEvents) const;

  void fill(MAUS::Spill *spill,
	    EMREventVector emr_events_tmp,
	    size_t nPartEvents) const;

  ThreeVector get_weighted_position(EMRBarHitArray barHitArray) const;

  // Maps
  EMRCalibrationMap _calibMap;
  EMRAttenuationMap _attenMap;
  EMRGeometryMap _geoMap;

  // Detector parameters
  size_t _number_of_planes;
  size_t _number_of_bars;

  // Charge reconstruction variables
  double _tot_func_p1;
  double _tot_func_p2;
  double _tot_func_p3;
};
} // namespace MAUS

#endif // #define _MAP_MAPCPPEMRSPACEPOINTS_H_
