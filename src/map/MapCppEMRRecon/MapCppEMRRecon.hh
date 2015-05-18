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
#include "API/MapBase.hh"

namespace MAUS {

typedef std::vector<MAUS::EMRBarHit>    EMRBarHitsVector; /* nHits elements */
typedef std::vector<EMRBarHitsVector>   EMRBarVector; /* 60 elements */
typedef std::vector<EMRBarVector>       EMRPlaneVector; /* 48 elements */
typedef std::vector<EMRPlaneVector>     EMRDBBEventVector; /* nTr elements */

struct fADCdata_er {
  int    _orientation;
  double _charge;
  double _charge_corrected;
  double _pedestal_area;
  int    _time;
  std::vector<int> _samples;
};

typedef std::vector<fADCdata_er>                EMRfADCPlaneHitsVector_er;/* 48 elements */
typedef std::vector<EMRfADCPlaneHitsVector_er>  EMRfADCEventVector_er;/* nTr elements */

struct TrackData {
  double _range_primary;
  double _range_secondary;
  double _secondary_to_primary_track_distance;
  bool _has_primary;
  bool _has_secondary;
  double _total_charge_ma;
  double _total_charge_sa;
  double _charge_ratio_ma;
  double _charge_ratio_sa;
  double _plane_density;
  double _chi2_x;
  double _chi2_y;
};

typedef std::vector<TrackData> EMRTrackEventVector;

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

  void process_preselected_events(MAUS::Spill *spill,
				  EMRDBBEventVector& emr_dbb_events_tmp,
				  EMRfADCEventVector_er& emr_fadc_events_tmp) const;

  void process_secondary_events(EMRDBBEventVector emr_dbb_events_tmp,
			        EMRfADCEventVector_er emr_fadc_events_tmp,
			        EMRDBBEventVector *emr_dbb_events,
			        EMRfADCEventVector_er& emr_fadc_events,
			        EMRTrackEventVector& emr_track_events) const;

  void tot_cleaning(int nPartEvents,
		    EMRDBBEventVector *emr_dbb_events,
		    EMRfADCEventVector_er& emr_fadc_events,
		    EMRTrackEventVector& emr_track_events) const;

  void pid_variables(int nPartEvents,
		    EMRDBBEventVector *emr_dbb_events,
		    EMRTrackEventVector& emr_track_events) const;

  void coordinates_reconstruction(int nPartEvents,
				  EMRDBBEventVector *emr_dbb_events,
				  EMRfADCEventVector_er& emr_fadc_events) const;

  void energy_correction(int nPartEvents,
			 EMRDBBEventVector *emr_dbb_events,
			 EMRfADCEventVector_er& emr_fadc_events) const;

  void track_matching(int nPartEvents,
		      EMRDBBEventVector *emr_dbb_events,
		      EMRfADCEventVector_er& emr_fadc_events,
		      EMRTrackEventVector& emr_track_events) const;

  void event_charge_calculation(int nPartEvents,
				EMRDBBEventVector *emr_dbb_events,
				EMRfADCEventVector_er& emr_fadc_events,
				EMRTrackEventVector& emr_track_events) const;

  void fill(Spill *spill,
	    int nPartEvents,
	    EMRDBBEventVector *emr_dbb_events,
	    EMRfADCEventVector_er& emr_fadc_events,
	    EMRTrackEventVector& emr_track_events) const;

  EMRDBBEventVector get_dbb_data_tmp(int nPartEvts) const;
  EMRfADCEventVector_er get_fadc_data_tmp(int nPartEvts) const;
  EMRTrackEventVector get_track_data_tmp(int nPartEvts) const;

  // Maps
  EMRCalibrationMap _calibMap;
  EMRAttenuationMap _attenMap;

  // Detector parameters
  int _number_of_planes;
  int _number_of_bars;
  double _bar_width;
  double _bar_height;
  double _gap;

  // Configuration variables
  int _secondary_hits_bunching_distance;
  int _secondary_hits_bunching_width;
  int _primary_trigger_minXhits;
  int _primary_trigger_minYhits;
  int _primary_trigger_minNhits;
  int _secondary_trigger_minXhits;
  int _secondary_trigger_minYhits;
  int _secondary_trigger_minNhits;
  int _secondary_trigger_minTot;
  int _max_secondary_to_primary_track_distance;

  // Charge reconstruction variables
  double _tot_func_p1;
  double _tot_func_p2;
  double _tot_func_p3;
  double _tot_func_p4;
};
}

#endif
