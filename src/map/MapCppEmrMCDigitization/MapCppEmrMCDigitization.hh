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


/** @class MapCppEmrMCDigitization
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
#include <iostream>
#include <algorithm>

// MAUS
#include "Utils/EMRCalibrationMap.hh"
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

// ROOT
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TSystem.h"
#include "TString.h"
#include "TRandom3.h"
#include "TH1F.h"
#include "TCanvas.h"

namespace MAUS {

typedef std::vector<MAUS::EMRBarHit>    EMRBarHitsVector; /* nHits elements */
typedef std::vector<EMRBarHitsVector>   EMRBarVector; /* 60 elements */
typedef std::vector<EMRBarVector>       EMRPlaneVector; /* 48 elements */
typedef std::vector<EMRPlaneVector>     EMRDBBEventVector; /* nTr elements */

struct fADCdata {
  int    _orientation;
  double _charge;
  double _pedestal_area;
  int    _arrival_time;
  double _g4_edep_mev;
  std::vector<int> _samples;
};

typedef std::vector<fADCdata>                EMRfADCPlaneHitsVector;/* 48 elements */
typedef std::vector<EMRfADCPlaneHitsVector>  EMRfADCEventVector;/* nTr elements */

class MapCppEmrMCDigitization {
 public:
  /** @brief Sets up the worker
   *
   *  argJsonConfigDocument a JSON document with
   *  the configuration
   */
  bool birth(std::string argJsonConfigDocument);

  /** @brief Shutdowns the worker
   *
   *  This takes no arguments and does nothing
   */
  bool death();

  /** @brief process JSON document
   *
   *  Receive a document with MC hits and return
   *  a document with digitized recEvts
   */
  std::string process(std::string document);

 private:

  /** @brief resets fADC and DBB data
 *
 *  Resizes the DBB and fADC event
 *  vectors and reinitialize their
 *  values and parameters
 */
  void reset_data_tmp(int nPartEvts);

  /** @brief fill DBB Events
 *
 *  Fill DBBEventVector
 */
  void processDBB(EMRHitArray *hits, int xPe, int ptime);

  /** @brief fill fADC Events
 *
 *  Fill fADCEventVector
 */
  void processFADC(EMRHitArray *hits, int xPe, int ptime);

  /** @brief fill Recon Events
 *
 *  Fill the EMRData with 
 *  reconstructed events
 */
  void fill(Spill *spill, int nPartEvents);

  /** @brief digitize ReconEvents
 *
 *  Fill the EMRData with 
 *  digitized data
 */
  void digitize(EMREvent *evt, int xPe, int nPartEvents);

  /** @brief fiber attenuation map
 *
 *  Receive a Bar, PMT ID and position and
 *  return an attenuation value
 */
  double fiber_atten_map(int planeid, int barid, double x, double y, int pmt);

  /** @brief fiber delay map
 *
 *  Receive a Bar, PMT ID and position and
 *  return a delay value in ns
 */
  int fiber_delay_map(int planeid, int barid, double x, double y, int pmt);

  /** @brief clear fiber length map
 *
 *  Receive a Bar, PMT ID and return
 *  return a length value in mm
 */
  double clear_fiber_length_map(int barid, int pmt);

  /** @brief connector attenuation map
 *
 *  Receive a Bar and PMT ID and return
 *  an attenuation value
 */
  double connector_atten_map(int barid, int pmt);

  std::string _classname;

  // Spill count
  int _spillCount;

  // fADC and DBB data arrays
  EMRDBBEventVector  _emr_dbb_events_tmp;
  EMRfADCEventVector _emr_fadc_events_tmp;

  // Parmaters defined in the configuration files
  FILE *_configfile;
  TString _calibfilename;
  FILE *_calibfile;
  TString _cflengthfilename;
  FILE *_cflengthfile;
  TString _cattenfilename;
  FILE *_cattenfile;

  // Detector parameters
  int _number_of_planes;
  int _number_of_bars;

  // Hit pre-selection cuts
  int _deltat_signal_up;
  int _deltat_signal_low;
  int _deltat_noise_up;
  int _deltat_noise_low;
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
  double _atten_WLSf;
  double _atten_CLRf;
  int _bar_length;
  int _nbars;
  double _dbb_count;
  double _fadc_count;
  int _spill_width;
  int _time_response_spread;
  double _tot_func_p1;
  double _tot_func_p2;
  double _tot_func_p3;
  double _tot_func_p4;
  double _tot_func_p5;
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

