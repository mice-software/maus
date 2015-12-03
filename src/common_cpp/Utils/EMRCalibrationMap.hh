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

#ifndef _MAUS_EMRCALIBRATIONMAP_HH_
#define _MAUS_EMRCALIBRATIONMAP_HH_


#include <Python.h>
#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <cctype>
#include <functional>

#include "json/json.h"
#include "Utils/EMRChannelMap.hh"
#include "Utils/Exception.hh"
#include "Interface/Squeak.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"

namespace MAUS {

/** Complete map of all calibration constants needed in order to reconstruct the energy
 * measured by the EMR detector. This class is used to hold and manage calibration 
 * constants and to calculate the calibration corrections.
 */

class EMRCalibrationMap {
 public:

  EMRCalibrationMap();
  virtual ~EMRCalibrationMap();

 /** Initialize the calibration map by using the text files provided in 
  * ConfigurationDefaults.py
  * \returns true if the text file is loaded successfully.
  */
  bool InitializeFromCards(Json::Value configJSON);

  /// Get calibrations from CDB
  bool InitializeFromCDB();

 /** Initialize the map by using the provided text files.
  * \param[in] calibFile name of the text file containing the energy calibration constants.
  * \returns true if the text file is loaded successfully.
  */
  bool Initialize(std::string calibFile);

 /** Returns the energy correction for the channel coded by the key.
  * \param[in] key the channel of the measurement.
  * \param[in] pmt the type of photomultiplier in question.
  * The photomultiplier type can either be "SA" or "MA"
  * \returns the value of the eps correction for this channel. If no calibration 
  * for this channel the function returns NOCALIB (-99999).
  */
  double Eps(EMRChannelKey key, const char *pmt) const;

 /** Print the calibration map;
  * To be used only for debugging.
  */
  void Print();

 /** Import the get_emr_calib module which accesses and gets calibrations from the CDB
  */
  bool InitializePyMod();

  enum {
   /** This value is returned when the correction can not be calculated.
    */
    NOCALIB = -99999
  };

 private:

  /** Use this function to reset the map before reloading. */
  void reset();

 /** Make one EMRChannelKey for each channel of the detector.
  * All EMRChannelKeys are held in the data member _Ckey.
  */
  int MakeEMRChannelKeys();

 /** Find the position of the channel key in the data member _Ckey.
  */
  int FindEMRChannelKey(EMRChannelKey key) const;

 /** Load calibration constants from text file.
  */
  bool Load(std::string calibFile);

 /** Load calibration constants from the CDB.
  */
  bool LoadFromCDB();

 /** Fetch the calibration string in the CDB
  */
  void GetCalib(std::string devname, std::string caltype, std::string fromdate);

 /** This vector holds one EMRChannelKey for each channel of the detector.
  */
  std::vector<EMRChannelKey> _Ckey;

 /** These vectors hold the calibration constants. IMPORTANT - the order of the entries here is
  * the same as the order of the entries in _Ckey. 
  * This is used when the constants are read.
  * MA = Multi-Anode PMT, SA = Single-Anode PMT
  */
  std::vector<double> _eps_MA;
  std::vector<double> _eps_SA;

 /** EMR characteristics
  */
  int _number_of_planes;
  int _number_of_bars;

 /** Choice of 'figure of merit'
  */
  std::string _fom;

 /** CDB calibration validity start data
  */
  std::string _calibDate;

 /** CDB string that harbours the calibration constants
  */
  std::stringstream epsstr;

 /** Python function to fetch the calibration string
  */
  bool pymod_ok;
  PyObject* _get_calib_func;
};
}

#endif


