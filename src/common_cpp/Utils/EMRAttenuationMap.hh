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

#ifndef _SRC_COMMON_CPP_UTILS_EMRATTENUATIONMAP_HH_
#define _SRC_COMMON_CPP_UTILS_EMRATTENUATIONMAP_HH_

// C++ headers
#include <Python.h>
#include <string>
#include <vector>
#include <iostream>

// MAUS headers
#include "json/json.h"
#include "Utils/EMRChannelMap.hh"
#include "Utils/Exception.hh"
#include "Utils/JsonWrapper.hh"
#include "Interface/Squeak.hh"

namespace MAUS {

/** @class Complete map of all attenuation constants needed in order to correct the charge
 *         and time measured in the EMR detector. This class is used to hold and manage
 *         attenuation constants and to calculate attenuation corrections.
 */

class EMRAttenuationMap {
 public:

  EMRAttenuationMap();
  virtual ~EMRAttenuationMap();

 /** @brief Initialize the attenuation map by using the text files
  *         provided in ConfigurationDefaults.py
  * 
  *  \returns true if the text file is loaded successfully.
  */
  bool InitializeFromCards(Json::Value configJSON);

 /** @brief Initialize the map by using the provided text files.
  *
  *  \param[in] connectorAttenFile	name of the file containing the connector attenuation.
  *  \param[in] cfLengthFile 		name of the text file containing the clear fibre length.
  *  \returns 				true if the text file is loaded successfully.
  */
  bool Initialize(std::string connectorAttenFile, std::string cfLengthFile);

 /** @brief Returns the attenuation factor in the connector of the channel coded by the key.
  *
  *  \param[in] key	channel of the measurement.
  *  \param[in] pmt 	type of photomultiplier it connects to ("MA", "SA").
  *  \returns 		the value of the connector attenuation factor for this channel. If no
  * 	      		value for this channel the function returns NOATTEN (-99999).
  */
  double connectorAtten(EMRChannelKey key, const char *pmt) const;

 /** @brief Returns the length [mm] of the clear fibre used for the channel coded by the key.
  *
  *  \param[in] key 	channel from which stems the fibre.
  *  \param[in] pmt	type of photomultiplier it connects to ("MA", "SA").
  *  \returns 		the value of the clear fibre length for this channel. If no value 
  *           		for this channel the function returns NOATTEN (-99999).
  */
  double cfLength(EMRChannelKey key, const char *pmt) const;

 /** @brief Returns the attenuation factor in the fibres (Clr+WLS) of the channel coded by the key.
  *
  *  \param[in] key 	channel of the measurement.
  *  \param[in] x 	abscissa of the track within the plane of the key.
  *  \param[in] y 	ordinate of the track within the plane of the key.
  *  \param[in] pmt 	type of photomultiplier it connects to ("MA", "SA").
  *  \returns 		the value of the fibre attenuation factor for this channel. If no value 
  *  	      		for this channel the function returns NOATTEN (-99999).
  */
  double fibreAtten(EMRChannelKey key, double x, double y, const char *pmt) const;

 /** @brief Returns the delay in the fibres (Clr+WLS) of the channel coded by the key.
  *
  *  \param[in] key	channel of the measurement.
  *  \param[in] x	abscissa of the track within the plane of the key.
  *  \param[in] y	ordinate of the track within the plane of the key.
  *  \param[in] pmt	type of photomultiplier it connects to ("MA", "SA").
  *  \returns 		the value of the fibre delay for this channel. If no value 
  *  	     		for this channel the function returns NOATTEN (-99999).
  */
  double fibreDelay(EMRChannelKey key, double x, double y, const char *pmt) const;

 /** @brief Print the attenuation map (debugging)
  */
  void Print();

 /** @brief This value is returned when the correction can not be calculated.
  */
  enum {
    NOATTEN = -99999
  };

 private:

 /** @brief Make one EMRChannelKey for each channel of the detector.
  * 	    All EMRChannelKeys are held in the data members _CkeyCA, _CkeyCFL.
  */
  int MakeEMRChannelKeys();

 /** @brief Find the position of the channel key in the data member _CkeyCA.
  */
  int FindEMRChannelKey(EMRChannelKey key) const;

 /** @brief Find the position of the channel key in the data member _CkeyCFL.
  */
  int FindEMRBarKey(EMRChannelKey key) const;

 /** @brief Load connector attenuation factors from text file.
  */
  bool LoadConnectorAttenFile(std::string connectorAttenFile);

 /** @brief Load clear fibre lengths from text file.
  */
  bool LoadCfLengthFile(std::string CfLengthFile);

 /** @brief This vector holds one EMRChannelKey for each channel of the detector.
  */
  std::vector<EMRChannelKey> _CkeyCA;
  std::vector<EMRChannelKey> _CkeyCFL;

 /** @brief These vectors hold the connector attenuation factors. IMPORTANT - the order
  * 	    of the entries here is the same as the order of the entries in _CkeyCA.
  * 	    This is used when the constants are read.
  * 	    MA = Multi-Anode PMT, SA = Single-Anode PMT
  */
  std::vector<double> _caf_MA;
  std::vector<double> _caf_SA;

 /** @brief These vectors hold the clear fibre lengths. IMPORTANT - the order
  * 	    of the entries here is the same as the order of the entries in _CkeyCFL.
  * 	    This is used when the constants are read.
  * 	    MA = Multi-Anode PMT, SA = Single-Anode PMT
  */
  std::vector<double> _cfl_MA;
  std::vector<double> _cfl_SA;

 /** @brief EMR characteristics
  */
  int _number_of_planes;
  int _number_of_bars;

  double _bar_length;
  double _atten_WLSf;
  double _atten_CLRf;
};
} // namespace MAUS

#endif // #define _SRC_COMMON_CPP_UTILS_EMRATTENUATIONMAP_HH_
