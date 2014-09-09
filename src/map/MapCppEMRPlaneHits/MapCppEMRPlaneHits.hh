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

// MAUS
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

namespace MAUS {

typedef std::vector<MAUS::EMRBarHit>    EMRBarHitsVector; /* nHits elements */
typedef std::vector<EMRBarHitsVector>   EMRBarVector; /* 60 elements */
typedef std::vector<EMRBarVector>       EMRPlaneVector; /* 48 elements */
typedef std::vector<EMRPlaneVector>     EMRDBBEventVector; /* nTr elements */

struct fADCdata {
  int    _orientation;
  double _charge;
  int    _time;
  int    _deltat;
  int    _spill;
};

typedef std::vector<fADCdata>                EMRfADCPlaneHitsVector;/* 48 elements */
typedef std::vector<EMRfADCPlaneHitsVector>  EMRfADCEventVector;/* nTr elements */

class MapCppEMRPlaneHits {

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
 *  @param document ?????.
 */
  std::string process(std::string document);

  /** @brief process the data object
 *
 *  @param
 */
  void process(MAUS::Data *data);

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
  void processDBB(EMRDaq EMRdaq, int nPartTrigger);

  /** @brief fill fADC Events
 *
 *  Fill fADCEventVector
 */
  void processFADC(EMRDaq EMRdaq, int nPartTrigger);

  /** @brief fill Recon Events
 *
 *  Fill the EMRData with 
 *  reconstructed events
 */
  void fill(Spill *spill, int nPartTrigger);

  std::string _classname;

  EMRChannelMap _emrMap;

  // fADC and DBB data arrays
  EMRDBBEventVector  _emr_dbb_events_tmp;
  EMRfADCEventVector _emr_fadc_events_tmp;

  // Detector parameters
  int _number_of_planes;
  int _number_of_bars;

  // Hit pre-selection cuts
  int _tot_noise_low;
  int _tot_noise_up;
  int _deltat_signal_low;
  int _deltat_signal_up;
  int _deltat_noise_low;
  int _deltat_noise_up;
};
}

#endif
