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

#ifndef _MAUS_INPUTCPPREALDATA_CA_HH_
#define _MAUS_INPUTCPPREALDATA_CA_HH_


#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <fstream>

#include "json/json.h"
#include "unpacking/MDfileManager.h"
#include "Utils/Exception.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"

namespace MAUS {

/** Identifier for a single DAQ channel.
 * This class is used to hold and manage all the information needed
 * to identifiy one DAQ Channel.
 */
class DAQChannelKey {
 public:

  DAQChannelKey()
  :_ldcId(-999), _geo(-999), _channel(-999), _eqType(-999), _detector("unknown") {}

  DAQChannelKey(int l, int g, int ch, int e, std::string d)
  :_ldcId(l), _geo(g), _channel(ch), _eqType(e), _detector(d) {}

  explicit DAQChannelKey(std::string keyStr) throw(Exception);

  virtual ~DAQChannelKey() {}

  bool operator==( DAQChannelKey key );
  bool operator!=( DAQChannelKey key );

  friend std::ostream& operator<< ( std::ostream& stream, DAQChannelKey key );
  friend std::istream& operator>> ( std::istream& stream, DAQChannelKey &key )  throw(Exception);

  std::string detector() {return _detector;}

  /** This function converts the DAQChannelKey into string.
  * \return String identifier.
  */
  std::string str();

  int ldc() const {return _ldcId;}
  int geo() const {return _geo;}
  int channel() const {return _channel;}
  int eqType() const {return _eqType;}

  /*  This function creates unique integer identifier.
  * \return Integer identifier.
  *
  int make_DAQChannelKey_id() { return _ldcId*1e8 + _geo*1e6 + _channel*1e3 + _eqType; }
  */

 private:

  /// Id number of the Local Data Concentrator (DAQ computer).
  int _ldcId;

  /// Id number of the board.
  int _geo;

  /// Channel number.
  int _channel;

  /// Type of the equipement as coded in DATE.
  int _eqType;

  /// Name of the detector connected to this channel.
  std::string _detector;
};

//////////////////////////////////////////////////////////////////////////////////////////////

/** Complete map of all DAQ channels.
 * This class is used to hold and manage the informatin for all DAQ Channel.
 */
class DAQChannelMap {
 public:

  DAQChannelMap() {}
  virtual ~DAQChannelMap();

 /** Initialize the map from text file.
 * \param[in] filename name of the text file.
 * \returns true if the map is initialized successfully.
 */
  bool InitFromFile(std::string filename);
  bool InitFromCards(Json::Value configJSON);

  /// Not implemented.
  bool InitFromCDB();

  // date file manager
  MDfileManager _dataFileManager;
  std::string _datafiles;
  bool is_number(const std::string& s);

 /** Return pointer to the key.
 * This function returns pointer to the key for the required DAQ channel.
 * \param[in] ldc     Id number of the Local Data Concentrator (computer).
 * \param[in] geo     Id number of the board.
 * \param[in] ch      Channel number.
 * \param[in] eqType  Type of the equipement as coded in DATE.
 * \return The key of the DAQ channel.
 */
  DAQChannelKey* find(int ldc, int geo, int ch, int eqType);

 /** Return pointer to the key.
 * This function returns pointer to the key for the required DAQ channel.
 * \param[in] daqKeyStr the required key coded as string.
 * \return The key of the DAQ channel.
 */  
  DAQChannelKey* find(std::string daqKeyStr);

  /// Return the name of the detectro conected to this DAQ channel.
  std::string detector(int ldc, int geo, int ch, int eqType);

 private:
  /// vector holding DAQChannelKeys for all channels of the MICE DAQ system
  std::vector<DAQChannelKey*> _chKey;
};

}  // namespace MAUS

#endif


