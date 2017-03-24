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

#ifndef _MAUS_TOFCALIBRATIONMAP_HH_
#define _MAUS_TOFCALIBRATIONMAP_HH_


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
#include "Utils/TOFChannelMap.hh"
#include "Utils/Exception.hh"
#include "Utils/Squeak.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"

namespace MAUS {

/** Identifier for a single TOF pixel.
 * This class is used to hold and manage all the information needed
 * to identifiy one pixel in the TOF detectors.
 */
class TOFPixelKey {

 public:
  TOFPixelKey()
  :_station(-999), _slabX(-999), _slabY(-999), _detector("unknown") {}

  TOFPixelKey(int st, int slX, int slY, string d)
  :_station(st), _slabX(slX), _slabY(slY), _detector(d) {}

  explicit TOFPixelKey(string keyStr) throw(Exceptions::Exception);

  virtual ~TOFPixelKey() {}

  bool operator==( const TOFPixelKey& key ) const;
  bool operator!=( const TOFPixelKey& key ) const;

  friend ostream& operator<<( ostream& stream, TOFPixelKey key );
  friend istream& operator>>( istream& stream, TOFPixelKey &key ) throw(Exceptions::Exception);

  string detector() {return _detector;}

  /** This function converts the DAQChannelKey into string.
  * \return String identifier.
  */
  string str();

  int station()   {return _station;}
  int slabX()      {return _slabX;}
  int slabY()      {return _slabY;}

  void SetStation(int xStation)       {_station = xStation;}
  void SetSlabX(int xSlab)             {_slabX = xSlab;}
  void SetSlabY(int xSlab)             {_slabY = xSlab;}
  void SetDetector(string xDetector)  {_detector = xDetector;}

  /* This function creates unique integer identifier.
  * \return Integer identifier.
  *
  int make_TOFPixelKey_id() {return _station*1e6 + _slabY*1e3 + _slabX;}
  */
 private:

  /// TOF station number.
  int _station;

 /** Number of the horizontal Slab. ATTENTION : according to the convention used in 
  * the cabling file the horizontal slabs are always in plane 0.
  */   
  int _slabX;

  /** Number of the vertical Slab. ATTENTION : according to the convention used in 
  * the cabling file the vertical slabs are always in plane 1.
  */
  int _slabY;

  /// Name of the detector.
  string _detector;
};

/** Complete map of all calibration constants needed in order to reconstruct the time
 * measured by the TOF detectors. This class is used to hold and manage calibration 
 * constants and to calculate the calibration corrections. The algorithm, used to calibrate
 * the time measurements, is described in MICE Note 251 "TOF Detectors Time Calibration".
 * http://mice.iit.edu/micenotes/public/pdf/MICE0251/MICE0251.pdf
 */
class TOFCalibrationMap {
 public:

  TOFCalibrationMap();
  virtual ~TOFCalibrationMap();

 /** Initialize the calibration map by using the text files provided in 
  * ConfigurationDefaults.py
  * \param[in] json_configuration Json document containing the configuration.
  * \returns true if all text files are loaded successfully.
  */
  bool InitializeFromCards(Json::Value configJSON, int);

  /// Get calibrations from CDB
  bool InitializeFromCDB();

 /** Initialize the map by using the provided text files.
  * \param[in] t0File name of the text file containing the t0 calibration constants.
  * \param[in] twFile name of the text file containing the TimeWalk calibration constants.
  * \param[in] triggerFile name of the text file containing the trigger delay calibration 
  * constants.
  * \returns true if all text files are loaded successfully.
  */
  bool Initialize(std::string t0File, std::string twFile, std::string triggerFile);

 /** Return the T0 correction for the channel coded by the key.
  * \param[in] key the channel of the measurement.
  * \param[out] r the number of the refference pixel in the slab.
  * The refference pixel is the pixel where the T0 calibration constant for this channel
  * has been calculated.
  * \returns the value of the T0 correction for this channel and sets the number of the 
  * refference pixel. If no calibration for this channel the function returns NOCALIB (-99999).
  */
  double T0(TOFChannelKey key, int &r) const;

 /** Return the Trigger delay correction for the pixel coded by the key.
  * \param[in] key the pixel of the hit that gives the trigger.
  * \returns the value of the trigger delay correction. If no calibration for this pixel the 
  * function returns NOCALIB (-99999).
  */
  double TriggerT0(TOFPixelKey key) const;

 /** Calculate the TimeWalk correction for the channel coded by the key and for given adc value.
  * \param[in] key the channel of the measurement.
  * \param[in] adc the measured value of the amplitude of the signal.
  * \returns the value of the time-walk correction. If no calibration for this channel the function
  * returns NOCALIB (-99999).
  */
  double TW(TOFChannelKey key, int adc ) const;

 /** Calculate the combined correction for the channel coded by Pkey, trigger 
  * pixel coded by the Tkey and for given adc value.
  * \param[in] Pkey the channel of the measurement.
  * \param[in] Tkey the pixel of the hit that gives the trigger.
  * \param[in] adc the measured value of the amplitude of the signal.
  */
  double dT(TOFChannelKey Pkey, TOFPixelKey Tkey, int adc) const;

 /** Return the data member Name.
  */
  std::string Name() const {return _name;}

 /** Return the data member _triggerStation.
  */
  int TriggerStationNumber() const {return _triggerStation;}

 /** Print the calibration map;
  * To be used only for debugging.
  */
  void Print();
  /* interface to the CDB-C++ API*/
  bool GetCalibCAPI(std::string devname, std::string caltype);
  void SetTriggerStation(int station) {_triggerStation = station;}
  enum {
   /** This value is returned when the correction can not be calculated.
    */
    NOCALIB = -99999
  };

 private:

 /** Make one TOFChannelKey for each channel of the detector.
  * All TOFChannelKeys are held in the data member _Pkey.
  * ATTENTION : The detector configuration is HARDCODED !!!!
  * TO BE IMPROVED !!!!
  */
  int MakeTOFChannelKeys();

  /** Use this function to reset the map before reloading. */
  void reset();

 /** Load T0 constants from text file.
  */
  bool LoadT0File(std::string t0File);

 /** Load Trigger delay constants from text file.
  */
  bool LoadTWFile(std::string twFile);

 /** Load TimeWalk constants from text file.
  */
  bool LoadTriggerFile(std::string triggerFile);

 /** Find the position of the PMT key in the data member _Pkey.
  */
  int FindTOFChannelKey(TOFChannelKey key) const;

 /** Find the position of the trigger key in the data member _Tkey.
  */
  int FindTOFPixelKey(TOFPixelKey key) const;

 /** This vector holds one TOFChannelKey for each channel of the detector.
  */
  std::vector<TOFChannelKey> _Pkey;

 /** This vector holds the TimeWalk constants. IMPORTANT - the order of the entries
  * here is the same as the order of the entries in _Pkey. 
  * This is used when the constants are read.
  */
  std::vector< std::vector<double> > _twPar;

 /** This vector holds the T0 constants. IMPORTANT - the order of the entries here is
  * the same as the order of the entries in _Pkey. 
  * This is used when the constants are read.
  */
  std::vector<double> _t0;

 /** This vector holds the number of the refference bar. IMPORTANT - the order of
  * the entries here is the same as the order of the entries in _Pkey. 
  * This is used when the constants are read.
  */
  std::vector<int>_reff;

 /** This vector holds one TOFPixelKey for each calibrated pixel of the trigger station.
  * IMPORTANT - uncalibrated pixels are not presented here.
  */
  std::vector<TOFPixelKey> _Tkey;

 /** This vector holds the Trigger delay constants. IMPORTANT - the order of
  * the entries here is the same as the order of the entries in _Tkey.
  * This is used when the constants are read.
  */
  std::vector<double> _Trt0;

 /** Number of the trigger station. It is automatically set during the initialization.
  */
  int _triggerStation;

 /** Name of the calibration as in the CDB.
  */
  std::string _name;
  std::stringstream t0str, twstr, trigstr;
  std::string _tof_station, _tof_calibdate, _tof_calib_by;

 /** Flags for switching On and Off of the different types of calibration corrections.
  */
  bool _do_timeWalk_correction;
  bool _do_triggerDelay_correction;
  bool _do_t0_correction;

  bool LoadT0Calib();
  bool LoadTWCalib();
  bool LoadTriggerCalib();
  int runNumber;
};
}

#endif


