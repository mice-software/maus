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

#ifndef _SRC_COMMON_CPP_UTILS_EMRCALIBRATIONMAP_HH_
#define _SRC_COMMON_CPP_UTILS_EMRCALIBRATIONMAP_HH_

// C++ headers
#include <Python.h>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>

// MAUS headers
#include "json/json.h"
#include "Utils/EMRChannelMap.hh"
#include "Utils/Exception.hh"
#include "Utils/JsonWrapper.hh"
#include "Interface/Squeak.hh"

namespace MAUS {

/** @class Complete map of all calibration constants needed in order to reconstruct the energy
 * 	   measured by the EMR detector. This class is used to hold and manage calibration 
 * 	   constants and to calculate the calibration corrections.
 */

class EMRCalibrationMap {
 public:

  EMRCalibrationMap();
  virtual ~EMRCalibrationMap();

 /** @brief Initialize the calibration map by using the text files
  *         provided in ConfigurationDefaults.py
  *
  *  \returns 	true if the text file is loaded successfully.
  */
  bool InitializeFromCards(Json::Value configJSON);

 /** @brief Initialize the calibration map by using the CDB configuration
  *
  *  \returns 	true if the text file is loaded successfully.
  */
  bool InitializeFromCDB();

 /** @brief Initialize the map by using the provided text files.
  *
  *  \param[in] calibFile	name of the text file containing the energy calibration constants.
  *  \returns 			true if the text file is loaded successfully.
  */
  bool Initialize(std::string calibFile);

 /** @brief Returns the energy correction for the channel coded by the key.
  *
  *  \param[in] key 	channel of the measurement.
  *  \param[in] pmt 	type of photomultiplier in question ("MA", "SA").
  *  \returns 		the value of the eps correction for this channel. If no calibration
  * 	      		for this channel the function returns NOCALIB (-99999).
  */
  double Eps(EMRChannelKey key, const char *pmt) const;

 /** @brief Print the calibration map (debugging)
  */
  void Print();

 /** @brief Import the get_emr_calib module which accesses and gets calibrations from the CDB
  */
  bool InitializePyMod();

 /** @brief This value is returned when the correction can not be found.
  */
  enum {
    NOCALIB = -99999
  };

 private:

 /** @brief Use this function to reset the map before reloading.
  */
  void reset();

 /** @brief Make one EMRChannelKey for each channel of the detector.
  *         All EMRChannelKeys are held in the data member _Ckey.
  */
  int MakeEMRChannelKeys();

 /** @brief Find the position of the channel key in the data member _Ckey.
  */
  int FindEMRChannelKey(EMRChannelKey key) const;

 /** @brief Load calibration constants from text file.
  */
  bool Load(std::string calibFile);

 /** @brief Load calibration constants from the CDB.
  */
  bool LoadFromCDB();

 /** @brief Fetch the calibration string in the CDB
  */
  void GetCalib(std::string devname, std::string caltype, std::string fromdate);

 /** @brief This vector holds one EMRChannelKey for each channel of the detector.
  */
  std::vector<EMRChannelKey> _Ckey;

 /** @brief These vectors hold the calibration constants. IMPORTANT - the order of the entries here is
  * 	    the same as the order of the entries in _Ckey. 
  * 	    This is used when the constants are read.
  * 	    MA = Multi-Anode PMT, SA = Single-Anode PMT
  */
  std::vector<double> _eps_MA;
  std::vector<double> _eps_SA;

 /** @brief EMR characteristics
  */
  int _number_of_planes;
  int _number_of_bars;

 /** @brief Choice of 'figure of merit'
  */
  std::string _fom;

 /** @brief CDB calibration validity start data
  */
  std::string _calibDate;

 /** @brief CDB string that harbours the calibration constants
  */
  std::stringstream epsstr;

 /** @brief Python function to fetch the calibration string
  */
  bool pymod_ok;
  PyObject* _get_calib_func;
};
} // namespace MAUS

#endif // #define _SRC_COMMON_CPP_UTILS_EMRCALIBRATIONMAP_HH_
