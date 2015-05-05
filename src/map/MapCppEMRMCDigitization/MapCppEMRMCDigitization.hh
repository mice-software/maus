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


/** @class MapCppEMRMCDigitization
 * Digitization of the EMR MC data.
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
#include "DataStructure/Data.hh"
#include "DataStructure/Spill.hh"
#include "DataStructure/ReconEvent.hh"
#include "DataStructure/DAQData.hh"
#include "DataStructure/DBBSpillData.hh"
#include "DataStructure/EMRBar.hh"
#include "DataStructure/EMRBarHit.hh"
#include "DataStructure/EMRPlaneHit.hh"
#include "DataStructure/EMREvent.hh"
#include "DataStructure/V1731.hh"
#include "DataStructure/Primary.hh"
#include "Utils/Globals.hh"
#include "Globals/GlobalsManager.hh"
#include "API/MapBase.hh"

// ROOT
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TSystem.h"
#include "TRandom3.h"
#include "TH1F.h"

namespace MAUS {

typedef std::vector<MAUS::EMRBarHit>    EMRBarHitsVector; /* nHits elements */
typedef std::vector<EMRBarHitsVector>   EMRBarVector; /* 60 elements */
typedef std::vector<EMRBarVector>       EMRPlaneVector; /* 48 elements */
typedef std::vector<EMRPlaneVector>     EMRDBBEventVector; /* nTr elements */

struct fADCdata {
  int    _orientation;
  double _charge;
  double _pedestal_area;
  int    _time;
  std::vector<int> _samples;
};

typedef std::vector<fADCdata>                EMRfADCPlaneHitsVector;/* 48 elements */
typedef std::vector<EMRfADCPlaneHitsVector>  EMRfADCEventVector;/* nTr elements */

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
   *  Receive a document with MC hits and return
   *  a document with digitized recEvts
   */
  void _process(MAUS::Data *data) const;


  void processDBB(MAUS::EMRHitArray *EMRhits,
		  int xPe,
		  double pTime,
		  EMRDBBEventVector& emr_dbb_events_tmp,
		  EMRfADCEventVector& emr_fadc_events_tmp) const;

  void processFADC(MAUS::EMRHitArray *EMRhits,
		   int xPe,
		   EMRfADCEventVector& emr_fadc_events_tmp) const;

  void digitize(MAUS::EMREvent *EMRevt,
		int xPe,
		int nPartEvents,
		int *deltat_limits,
		EMRDBBEventVector& emr_dbb_events_tmp,
		EMRfADCEventVector& emr_fadc_events_tmp) const;

  void fill(MAUS::Spill *spill,
	    int nPartEvents,
	    EMRDBBEventVector emr_dbb_events_tmp,
	    EMRfADCEventVector emr_fadc_events_tmp) const;

  EMRDBBEventVector get_dbb_data_tmp(int nPartEvts) const;
  EMRfADCEventVector get_fadc_data_tmp(int nPartEvts) const;

  // Maps
  EMRCalibrationMap _calibMap;
  EMRAttenuationMap _attenMap;

  // Detector parameters
  int _number_of_planes;
  int _number_of_bars;

  // Hit pre-selection cuts
  int _tot_noise_up;
  int _tot_noise_low;

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
  double _tot_func_p4;
  int _acquisition_window;
  int _signal_integration_window;
  int _arrival_time_shift;
  double _arrival_time_gaus_width;
  double _arrival_time_uniform_width;
  double _pulse_shape_landau_width;
  std::string _fom;
  double _birks_constant;
  double _average_path_length;
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
};  // Don't forget this trailing colon!!!!
} // ~namespace MAUS

#endif

