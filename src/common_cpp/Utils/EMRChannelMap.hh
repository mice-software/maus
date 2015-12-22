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

#ifndef _SRC_COMMON_CPP_UTILS_EMRCHANNELMAP_HH_
#define _SRC_COMMON_CPP_UTILS_EMRCHANNELMAP_HH_

#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <fstream>

#include "Utils/DAQChannelMap.hh"
#include "Utils/Exception.hh"
#include "Interface/Squeak.hh"

namespace MAUS {

/** @class Identifier for a single EMR channel.
 *    This class is used to hold and manage all the information needed
 *    to identifiy one channel in the EMR detector.
 */

class EMRChannelKey {
 public:

  /** @brief Default constructor - initialises to 0/NULL */
  EMRChannelKey();

  /** @brief Copy constructor */
  EMRChannelKey(int pl, int o, int b, string d);

  /** @brief Destructor - any member pointers are deleted */
  virtual ~EMRChannelKey();

  explicit EMRChannelKey(string keyStr) throw(Exception);

  bool operator==( EMRChannelKey key ) const;
  bool operator!=( EMRChannelKey key ) const;

  friend ostream& operator<<( ostream& stream, EMRChannelKey key );
  friend istream& operator>>( istream& stream, EMRChannelKey &key ) throw(Exception);

  /** @brief This function converts the DAQChannelKey into string.
  *
  * \return 	String identifier.
  */
  string str();

  /** @brief Returns the plane ID in the EMR channel key */
  int GetPlane() const                 { return _plane; }

  /** @brief Sets the plane ID in the EMR channel key */
  void SetPlane(int plane)             { _plane = plane; }

  /** @brief Returns the plane orientation in the EMR channel key */
  int GetOrientation() const           { return _orientation; }

  /** @brief Sets the plane orientation in the EMR channel key */
  void SetOrientation(int orientation) { _orientation = orientation; }

  /** @brief Returns the bar ID in the EMR channel key */
  int GetBar() const                   { return _bar; }

  /** @brief Sets the bar ID in the EMR channel key */
  void SetBar(int bar)                 { _bar = bar; }

  /** @brief Returns the name of the detector associated with the channel key */
  string GetDetector() const           { return _detector; }

  /** @brief Sets the name of the detector associated with the channel key */
  void SetDetector(string detector)    { _detector = detector; }

 private:

  int 		_plane;
  int 		_orientation;
  int 		_bar;
  string	_detector;
};



/** @class Complete map of all EMR channels.
 *    This class is used to hold and manage the informatin for all EMR Channel.
 */

class EMRChannelMap {
 public:

  /** @brief Default constructor - initialises to 0/NULL */
  EMRChannelMap();

  /** @brief Destructor - any member pointers are deleted */
  virtual ~EMRChannelMap();

  /** @brief Initialize the map from text file. */
  bool InitializeFromFile(string filename);

  /** @brief Initialize the map from the CDB !!!TODO!!! */
  void InitializeFromCDB();

  /** @brief This function returns pointer to the EMR channel key for the required DAQ channel.
  * 
  * \param[in]	daqch DAQ channel to search for.
  * \return	The key of the EMR channel connected to the given DAQ channel.
  */
  EMRChannelKey* Find(DAQChannelKey* daqKey) const;

  /** @brief This function returns pointer to the EMR channel key for the required DAQ channel.
  * 
  * \param[in]	daqch DAQ channel to search for, coded as string.
  * return 	The key of the EMR channel connected to the given DAQ channel.
  */
  EMRChannelKey* Find(string daqKeyStr);

  /** @brief Returns the ortientation for a given plane */
  int GetOrientation(int plane);

  /** @brief Print the entire EMR channel map */
  void Print();

 private:

  std::vector<EMRChannelKey*>	_emrKey;
  std::vector<DAQChannelKey*>	_daqKey;
};
} // namespace MAUS

#endif // #define _SRC_COMMON_CPP_UTILS_EMRCHANNELMAP_HH_
