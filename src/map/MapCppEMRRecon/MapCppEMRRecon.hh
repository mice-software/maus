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


/** @class MapCppEMRRecon
 * Reconstruct EMR data and create associated tracks
 * by reconstructing each hit coordinates, fitting them 
 * together in one track and matching the tracks
 */

#ifndef _MAP_MAPCPPEMRRECON_H_
#define _MAP_MAPCPPEMRRECON_H_

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
#include "Utils/EMRCalibrationMap.hh"
#include "Utils/EMRAttenuationMap.hh"
#include "Utils/EMRGeometryMap.hh"
#include "DataStructure/Data.hh"
#include "DataStructure/Spill.hh"
#include "DataStructure/ReconEvent.hh"
#include "DataStructure/DAQData.hh"
#include "DataStructure/EMRDaq.hh"
#include "DataStructure/DBBSpillData.hh"
#include "DataStructure/EMREvent.hh"
#include "Recon/EMR/TrackFitter.hh"
#include "Recon/EMR/TrackRange.hh"
#include "Recon/EMR/TrackMomentum.hh"
#include "API/MapBase.hh"

namespace MAUS {

class MapCppEMRRecon : public MapBase<MAUS::Data> {

 public:
  MapCppEMRRecon();

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

  void reconstruct_plane_density(MAUS::Spill *spill,
				 size_t nPartEvents) const;

  void reconstruct_event_charge(MAUS::Spill *spill,
				size_t nPartEvents) const;

  void reconstruct_tracks(MAUS::Spill *spill,
			  size_t nPartEvents) const;

  void match_daughters(MAUS::Spill *spill,
		       size_t nPartEvents) const;

  double dist(MAUS::ThreeVector p1,
	      MAUS::ThreeVector p2) const;

  // Maps
  EMRCalibrationMap _calibMap;
  EMRAttenuationMap _attenMap;
  EMRGeometryMap _geoMap;

  // Detector parameters
  size_t _number_of_planes;
  size_t _number_of_bars;

  // Max distance for matching
  double _charge_threshold;
  int _polynomial_order;
  double _max_time;
  double _max_distance;
  double _dbb_count;
  double _hole_fraction;
};
} // namespace MAUS

#endif // #define _MAP_MAPCPPEMRRECON_H_
