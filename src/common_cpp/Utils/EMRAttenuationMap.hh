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

#ifndef _MAUS_EMRATTENUATIONMAP_HH_
#define _MAUS_EMRATTENUATIONMAP_HH_


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

class EMRAttenuationMap {
 public:

  EMRAttenuationMap();
  virtual ~EMRAttenuationMap();

 /** Initialize the attenuation map by using the text files provided in 
  * ConfigurationDefaults.py
  * \returns true if the text file is loaded successfully.
  */
  bool InitializeFromCards(Json::Value configJSON);

 /** Initialize the map by using the provided text files.
  * \param[in] cfLengthFile name of the text file containing the clear fibre length.
  * \param[in] cfLengthFile name of the text file containing the clear fibre length.
  * \returns true if the text file is loaded successfully.
  */
  bool Initialize(std::string connectorAttenFile, std::string cfLengthFile);

 /** Returns the attenuation factor in the connector of the channel coded by the key.
  * \param[in] key the channel of the measurement.
  * \param[in] pmt the type of photomultiplier it connects to.
  * The photomultiplier type can either be "SA" or "MA"
  * \returns the value of the connector attenuation factor for this channel. If no calibration 
  * for this channel the function returns NOCALIB (-99999).
  */
  double connectorAtten(EMRChannelKey key, const char *pmt) const;

 /** Returns the length [mm] of the clear fibre used for the bar coded by the key.
  * \param[in] key the channel from which stems the fibre.
  * \param[in] pmt the type of photomultiplier it connects to.
  * The photomultiplier type can either be "SA" or "MA"
  * \returns the value of the clear fibre length for this channel. If no value 
  * for this bar the function returns NOCALIB (-99999).
  */
  double cfLength(EMRChannelKey key, const char *pmt) const;

 /** Returns the attenuation factor in the fibres (Clr+WLS) of the channel coded by the key.
  * \param[in] key the channel of the measurement.
  * \param[in] x abscissa of the track within the plane of the key.
  * \param[in] y ordinate of the track within the plane of the key.
  * \param[in] pmt the type of photomultiplier it connects to.
  * The photomultiplier type can either be "SA" or "MA"
  * \returns the value of the fibre attenuation factor for this channel. If no calibration 
  * for this channel the function returns NOCALIB (-99999).
  */
  double fibreAtten(EMRChannelKey key, double x, double y, const char *pmt) const;

 /** Returns the delay in the fibres (Clr+WLS) of the channel coded by the key.
  * \param[in] key the channel of the measurement.
  * \param[in] x abscissa of the track within the plane of the key.
  * \param[in] y ordinate of the track within the plane of the key.
  * \param[in] pmt the type of photomultiplier it connects to.
  * The photomultiplier type can either be "SA" or "MA"
  * \returns the value of the fibre delay for this channel. If no calibration 
  * for this channel the function returns NOCALIB (-99999).
  */
  int fibreDelay(EMRChannelKey key, double x, double y, const char *pmt) const;

 /** Print the attenuation map;
  * To be used only for debugging.
  */
  void Print();

  enum {
   /** This value is returned when the correction can not be calculated.
    */
    NOCALIB = -99999
  };

 private:

 /** Make one EMRChannelKey for each channel of the detector.
  * All EMRChannelKeys are held in the data member _Ckey.
  */
  int MakeEMRChannelKeys();

 /** Find the position of the channel key in the data member _Ckey.
  */
  int FindEMRChannelKey(EMRChannelKey key) const;

 /** Load connector attenuation factors from text file.
  */
  bool LoadConnectorAttenFile(std::string connectorAttenFile);

 /** Load clear fibre lengths from text file.
  */
  bool LoadCfLengthFile(std::string CfLengthFile);

 /** This vector holds one EMRChannelKey for each channel of the detector.
  */
  std::vector<EMRChannelKey> _Ckey;

 /** These vectors hold the connector attenuation factors. IMPORTANT - the order 
  * of the entries here is the same as the order of the entries in _Ckey. 
  * This is used when the constants are read.
  * MA = Multi-Anode PMT, SA = Single-Anode PMT
  */
  std::vector<double> _cfl_MA;
  std::vector<double> _cfl_SA;

 /** These vectors hold the clear fibre lengths. IMPORTANT - the order 
  * of the entries here is the same as the order of the entries in _Bkey. 
  * This is used when the constants are read.
  * MA = Multi-Anode PMT, SA = Single-Anode PMT
  */
  std::vector<double> _caf_MA;
  std::vector<double> _caf_SA;

 /** EMR characteristics
  */
  int _number_of_planes;
  int _number_of_bars;

  int _bar_length;
  double _atten_WLSf;
  double _atten_CLRf;
};
}

#endif


