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

#include "json/json.h"
#include "Utils/TOFChannelMap.hh"
#include "Interface/Squeal.hh"
#include "Interface/Squeak.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"


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

  explicit TOFPixelKey(string keyStr) throw(Squeal);

  virtual ~TOFPixelKey() {}

  bool operator==( TOFPixelKey key );
  bool operator!=( TOFPixelKey key );

  friend ostream& operator<<( ostream& stream, TOFPixelKey key );
  friend istream& operator>>( istream& stream, TOFPixelKey &key ) throw(Squeal);

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

  /** This function creates unique integer identifier.
  * \return Integer identifier.
  */
  int make_TOFPixelKey_id() {return _station*1e6 + _slabY*1e3 + _slabX;}

 private:

  /// TOF station number.
  int _station;

  /// Number of the horizontal Slab.
  int _slabX;

  /// Number of the vertical Slab.
  int _slabY;
  
  /// Name of the detector.
  string _detector;
};

/** Complete map of all calibration constants needed in order to reconstruct 
 * the time measured by the TOF detectors. This class is used to hold and 
 * manage calibration constants and to calculate the calibration corrections.
 */
class TOFCalibrationMap {
 public:

  TOFCalibrationMap() {}
  virtual ~TOFCalibrationMap();
  
  bool InitializeFromCards(std::string json_configuration);
  
 /** Initialize the map by using the provided text files.
  */
  bool Initialize(std::string t0File, std::string twFile, std::string triggerFile);

  /** Return the T0 correction for the channel coded by the key.
   */
  double T0( TOFChannelKey key ,int &r);

  /** Return the Trigger delay correction for the pixel coded by the key.
   */
  double TriggerT0( TOFPixelKey key);

  /** Calculate the TimeWalk correction for the channel coded by the key and for 
   * given adc value.
   */
  double TW( TOFChannelKey key, int adc );

  /** Calculate the combined correction for the channel coded by Pkey, trigger 
   * pixel coded by the Tkey and for given adc value.
   */
  double dT(TOFChannelKey Pkey, TOFPixelKey Tkey, int adc);

  /** Return the data member Name.
   */
  std::string GetName()        const        {return Name;};

  /** Return the data member Detector.
   */
  std::string GetDetector()        const {return Detector;};

  /**  Print the calibration map;
   */
  void Print();

  void SetTriggerStation(int station) {TriggerStation = station;}
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
  int FindTOFChannelKey( TOFChannelKey key );

 /** Find the position of the trigger key in the data member _Tkey.
  */
  int FindTOFPixelKey( TOFPixelKey key );

 /** This vector holds one TOFChannelKey for each channel of the detector.
  */
  std::vector<TOFChannelKey> _Pkey;

 /** This vector holds the TimeWalk constants.\ IMPORTANT - the order of the entries
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
  int TriggerStation;

 /** Name of the calibration as in the CBD.
  */
  std::string Name;

 /** Detector name as in the CBD.
  */
  std::string Detector;
};

#endif


