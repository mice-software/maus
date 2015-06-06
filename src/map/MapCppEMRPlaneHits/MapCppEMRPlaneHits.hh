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
 * Reconstruct EMR data and create EMR bar hits
 * by running over the fADC and DBB data
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
#include "DataStructure/EMRSpillData.hh"
#include "API/MapBase.hh"

namespace MAUS {

typedef std::vector<MAUS::EMRBarHit>    EMRBarHitsVector; // nHits elements
typedef std::vector<EMRBarHitsVector>   EMRBarVector; // 60 elements
typedef std::vector<EMRBarVector>       EMRPlaneVector; // 48 elements
typedef std::vector<EMRPlaneVector>     EMRDBBEventVector; // nTr elements

struct fADCdata {
  int _orientation;
  int _charge;
  int _time;
  int _spill;
  int _deltat;
};

typedef std::vector<fADCdata>                EMRfADCPlaneHitsVector; // 48 elements
typedef std::vector<EMRfADCPlaneHitsVector>  EMRfADCEventVector; // nTr elements

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
		  int nPartEvents,
		  EMRDBBEventVector& emr_dbb_events_tmp,
		  EMRfADCEventVector& emr_fadc_events_tmp) const;

  void processFADC(MAUS::EMRDaq EMRdaq,
		   int nPartEvents,
		   EMRfADCEventVector& emr_fadc_events_tmp) const;

  void fill(MAUS::Spill *spill,
	    int nPartEvents,
	    EMRDBBEventVector emr_dbb_events_tmp,
	    EMRfADCEventVector emr_fadc_events_tmp) const;

  EMRDBBEventVector get_dbb_data_tmp(int nPartEvts) const;
  EMRfADCEventVector get_fadc_data_tmp(int nPartEvts) const;

  EMRChannelMap _emrMap;

  // Detector parameters
  int _number_of_planes;
  int _number_of_bars;

  // Hit pre-selection cuts
  int _deltat_signal_low;
  int _deltat_signal_up;
};
}

#endif
