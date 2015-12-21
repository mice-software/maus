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

#ifndef _MAP_MAPCPPEMRMCDIGITIZATION_H_
#define _MAP_MAPCPPEMRMCDIGITIZATION_H_

// C headers
#include <assert.h>
#include <json/json.h>

// C++ headers
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>

// MAUS
#include "Utils/EMRChannelMap.hh"
#include "Utils/EMRCalibrationMap.hh"
#include "Utils/EMRAttenuationMap.hh"
#include "Utils/CppErrorHandler.hh"
#include "Utils/JsonWrapper.hh"
#include "Interface/dataCards.hh"
#include "Converter/DataConverters/CppJsonSpillConverter.hh"
#include "Converter/DataConverters/JsonCppSpillConverter.hh"
#include "API/PyWrapMapBase.hh"
#include "API/MapBase.hh"
#include "DataStructure/Data.hh"
#include "DataStructure/Spill.hh"
#include "DataStructure/ReconEvent.hh"
#include "DataStructure/EMREvent.hh"
#include "DataStructure/EMRSpillData.hh"
#include "DataStructure/V1731.hh"
#include "DataStructure/Primary.hh"
#include "DataStructure/ThreeVector.hh"
#include "Utils/Globals.hh"
#include "Globals/GlobalsManager.hh"

// ROOT
#include "TRandom3.h"
#include "TH1F.h"

namespace MAUS {

/** @class MapCppEMRMCDigitization
 *
 * Digitization of the EMR MC data.
 */

struct EMRBarHitTmp {
  EMRBarHit _barhit;
  ThreeVector _pos;	// Position of the hit (attenuation)
  double _path_length;	// Path length in the bar (Birk's law)
};

typedef std::vector<EMRBarHitTmp>	EMRBarHitVector;	// nHits elements

struct EMRPlaneHitTmp {
  EMRBarHitVector _barhits;
  int    _orientation;
  int    _time;
  int    _deltat;
  double _charge;
  double _pedestal_area;
  std::vector<int> _samples;
};

typedef std::vector<EMRPlaneHitTmp>	EMRPlaneHitVector;	// 48 elements
typedef std::vector<EMRPlaneHitVector> 	EMREventPlaneHitVector; // nTr elements

class MapCppEMRMCDigitization : public MapBase<MAUS::Data> {
 public:
  MapCppEMRMCDigitization();

 private:
  /** @brief Sets up the worker
   *
   *  argJsonConfigDocument a JSON document with
   *  the configuration
   */
  void _birth(const std::string& argJsonConfigDocument);

  /** @brief Shutdowns the worker
   *
   *  This takes no arguments and does nothing
   */
  void _death();

  /** @brief process JSON document
   *
   *  Receives MAUS::Data with MC hits and return
   *  a document with digitized recEvts
   */
  void _process(MAUS::Data *data) const;

  void processMC(MAUS::MCEventPArray *mcEvts,
		 EMREventPlaneHitVector& mc_events_tmp,
		 int deltat_min,
		 int deltat_max) const;

  void digitize(EMREventPlaneHitVector mc_events_tmp,
		EMREventPlaneHitVector& emr_events_tmp,
		EMRBarHitArray& emr_bar_hits_tmp,
		size_t nPartEvents) const;

  void process_candidates(EMRBarHitArray emr_bar_hits_tmp,
		          EMREventPlaneHitVector& emr_candidates_tmp) const;

  void fill(MAUS::Spill *spill,
	    EMREventPlaneHitVector& emr_events_tmp,
	    size_t nPartEvents) const;

  EMREventPlaneHitVector get_emr_events_tmp(size_t nPartEvts) const;

  // Maps
  EMRCalibrationMap _calibMap;
  EMRAttenuationMap _attenMap;

  // Detector parameters
  size_t _number_of_planes;
  size_t _number_of_bars;

  // Hit pre-selection cuts
  size_t _secondary_n_low;
  int _secondary_tot_low;
  int _secondary_bunching_width;

  // Digitization variables
  int _do_sampling;
  int _nph_per_MeV;
  int _seed;
  double _trap_eff;
  double _QE_MAPMT;
  double _QE_SAPMT;
  double _nADC_per_pe_MAPMT;
  double _nADC_per_pe_SAPMT;
  double _electronics_response_spread_MAPMT;
  double _electronics_response_spread_SAPMT;
  double _dbb_count;
  double _fadc_count;
  int _time_response_spread;
  double _tot_func_p1;
  double _tot_func_p2;
  double _tot_func_p3;
  int _acquisition_window;
  int _signal_integration_window;
  int _arrival_time_shift;
  double _arrival_time_gaus_width;
  double _arrival_time_uniform_width;
  double _pulse_shape_landau_width;
  std::string _fom;
  double _birks_constant;
  double _signal_energy_threshold;
  int _baseline_spread;
  int _maximum_noise_level;
  double _deltat_shift;
  double _baseline_position;
  double _arrival_time_spread;

  // Noise simulation variables
  TRandom3 *_rand;
  int _baseline[48];
  int _noise_level[48];
  int _noise_position[48];
};
} // namespace MAUS

#endif // #define _MAP_MAPCPPEMRMCDIGITIZATION_H_
