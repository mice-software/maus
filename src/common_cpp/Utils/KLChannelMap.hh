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

#ifndef _MAUS_KL_CABLINGTOOLS_HH_
#define _MAUS_KL_CABLINGTOOLS_HH_


#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <fstream>


#include "src/common_cpp/Utils/DAQChannelMap.hh"
#include "Utils/Exception.hh"
#include "src/legacy/Interface/Squeak.hh"

using std::string;
using std::ostream;
using std::istream;
using std::ifstream;
using std::stringstream;

namespace MAUS {
//////////////////////////////////////////////////////////////////////////////////////////////
/** Identifier for a single KL channel.
 * This class is used to hold and manage all the information needed
 * to identifiy one channel in the KL detectors.
 */
class KLChannelKey {
 public:

  KLChannelKey()
  :_cell(-999), _pmt(-999), _detector("unknown") {}

  KLChannelKey(int cl, int pmt, string d)
  :_cell(cl), _pmt(pmt), _detector(d) {}

  explicit KLChannelKey(string keyStr) throw(Exception);
  virtual ~KLChannelKey() {}

  bool operator==( KLChannelKey key ) const;
  bool operator!=( KLChannelKey key ) const;

  /** Return true only if the given KL channel is connected 
  * to the the opposit side of the cell.
  */
  bool inSameCell(KLChannelKey key);

  /** Return true only if the given KL channel coded as 
  * string is connected to the opposit side of the cell.
  */  
  bool inSameCell(string keyStr);

  KLChannelKey GetOppositeSidePMT();
  string GetOppositeSidePMTStr();

  friend ostream& operator<<( ostream& stream, KLChannelKey key );
  friend istream& operator>>( istream& stream, KLChannelKey &key ) throw(Exception);

  string detector() const {return _detector;}

  /** This function converts the DAQChannelKey into string.
  * \return String identifier.
  */
  string str();

  int cell() const {return _cell;}
  int pmt() const {return _pmt;}

  void SetCell(int xCell) {_cell = xCell;}
  void SetPmt(int xPmt) {_pmt = xPmt;}
  void SetDetector(string xDetector) {_detector = xDetector;}

 private:

  /// Cell number.
  int _cell;

  /// PMT number.
  int _pmt;

  /// Name of the detector.
  string _detector;
};

//////////////////////////////////////////////////////////////////////////////////////////////
/** Complete map of all KL channels.
 * This class is used to hold and manage the informatin for all KL Channel.
 */
class KLChannelMap {
 public:

  KLChannelMap() {}
  virtual ~KLChannelMap();

  /// Initialize the map from text file.
  bool InitFromFile(string filename);

  /// Not implemented.
  void InitFromCDB();

 /** Return pointer to the KL key.
 * This function returns pointer to the KL channel key for the required DAQ channel.
 * \param[in] daqch DAQ channel to search for.
 * \return The key of the KL channel connected to the given DAQ channel.
 */
  KLChannelKey* find(DAQChannelKey* daqKey) const;

 /** Return pointer to the KL key.
 * This function returns pointer to the KL channel key for the required DAQ channel.
 * \param[in] daqch DAQ channel to search for, coded as string.
 * \return The key of the KL channel connected to the given DAQ channel.
 */  
  KLChannelKey* find(string daqKeyStr) const;

 private:

  std::vector<KLChannelKey*> _klKey;
  std::vector<DAQChannelKey*> _fadcKey;
};
}
#endif




