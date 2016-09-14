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

#ifndef _MAUS_KLCALIBRATIONMAP_HH_
#define _MAUS_KLCALIBRATIONMAP_HH_


#include <Python.h>
#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <functional>

#include "json/json.h"
#include "Utils/KLChannelMap.hh"
#include "Utils/Exception.hh"
#include "Utils/Squeak.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"

namespace MAUS {
/** Map of gain constants needed in order to reconstruct the charge
 * measured by the KL detector. This class is used to hold and manage gain 
 * constants and to calculate the calibration corrections. 
 */
class KLCalibrationMap {
 public:

  KLCalibrationMap();
  virtual ~KLCalibrationMap();

 /** Initialize the calibration map by using the text files provided in 
  * ConfigurationDefaults.py
  * \param[in] json_configuration Json document containing the configuration.
  * \returns true if the text file is loaded successfully.
  */
  bool InitializeFromCards(Json::Value configJSON);

  /// Get cabling from CDB.
  bool InitializeFromCDB();

 /** Initialize the map by using the provided text file.
  * \param[in] gainFile name of the text file containing the gain calibration constants.
  * \returns true if the text file is loaded successfully.
  */
  bool Initialize(std::string gainFile);

 /** Return the gain correction for the channel coded by the key.
  * \param[in] key the channel of the measurement.
  * \returns the value of the gain correction for this channel.
  * If no calibration for this channel the function returns NOCALIB (-99999).
  */
  double Gain(KLChannelKey key) const;
  bool InitializePyMod();

 /** Print the calibration map;
  * To be used only for debugging.
  */
  void Print();
  void GetCalib(std::string devname, std::string caltype, std::string fromdate);

  enum {
   /** This value is returned when the correction can not be calculated.
    */
    NOCALIB = -99999
  };

 private:
  /** Use this function to reset the map before reloading. */
  void reset();

 /** Make one KLChannelKey for each channel of the detector.
  * All KLChannelKeys are held in the data member _Pkey.
  */
  int MakeKLChannelKeys();

 /** Load gain constants from text file.
  */
  bool LoadGainFile(std::string gain_file);

  /** Find the position of the PMT key in the data member _Pkey.
  */
  int FindKLChannelKey(KLChannelKey key) const;

 /** This vector holds one KLChannelKey for each channel of the detector.
  */
  std::vector<KLChannelKey> _Pkey;

  /** This vector holds the gain constants. IMPORTANT - the order of the entries here is
  * the same as the order of the entries in _Pkey. 
  * This is used when the constants are read.
  */
  std::vector<double> _gain;

  /** Name of the calibration as in the CDB.
  */
  std::string _name;
  std::stringstream gainstr;
  std::string  _kl_calibdate;

  /** Flags for switching On/Off gain corrections.
  */
  PyObject* _calib_mod;
  PyObject* _tcalib;
  PyObject* _get_calib_func;
  bool LoadGainCalib();
  bool pymod_ok;
  bool _do_gain_correction;
};
}
#endif


