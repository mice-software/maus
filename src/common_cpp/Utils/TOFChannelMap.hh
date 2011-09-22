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

#ifndef _MAUS_INPUTCPPREALDATA_CABLINGTOOLS_HH_
#define _MAUS_INPUTCPPREALDATA_CABLINGTOOLS_HH_


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
#include "Interface/Squeal.hh"
#include "Interface/Squeak.hh"

using std::string;
using std::ostream;
using std::istream;
using std::ifstream;
using std::stringstream;

//////////////////////////////////////////////////////////////////////////////////////////////
/** Identifier for  a single TOF channel.
 * This class is used to hold and manage all the information needed
 * to identifiy one channel in the TOF detectors.
 */
class TOFChannelKey {
 public:
  TOFChannelKey()
  :_station(-999), _plane(-999), _slab(-999), _pmt(-999), _detector("unknown") {}

  TOFChannelKey(int l, int g, int ch, int e, string d)
  :_station(l), _plane(g), _slab(ch), _pmt(e), _detector(d) {}
  
  TOFChannelKey(string keyStr) throw(Squeal);
  virtual ~TOFChannelKey() {}

  bool operator==( TOFChannelKey key );
  bool operator!=( TOFChannelKey key );
  
  /** Return true only if the given TOF channel is connected 
  * to the the opposit side of the slab.
  */
  bool inSameSlab(TOFChannelKey key);
  
  /** Return true only if the given TOF channel coded as 
  * string is connected to the opposit side of the slab.
  */  
  bool inSameSlab(string keyStr);

  friend ostream& operator<<( ostream& stream, TOFChannelKey key );
  friend istream& operator>>( istream& stream, TOFChannelKey &key ) throw(Squeal);

  string detector() {return _detector;}

  /** This function converts the DAQChannelKey into string.
  * \return String identifier.
  */
  string str();

  int station()     {return _station;}
  int plane()     {return _plane;}
  int slab() {return _slab;}
  int pmt()  {return _pmt;}

  /** This function creates unique integer identifier.
  * \return Integer identifier.
  */
  int make_TOFChannelKey_id() { return _station*1e8 + _plane*1e6 + _slab*1e3 + _pmt; }

  /// TOF station number.
  int _station;

  /// Plane number.
  int _plane;

  /// Slab number.
  int _slab;

  /// PMT number.
  int _pmt;

  /// Name of the detector.
  string _detector;
};

//////////////////////////////////////////////////////////////////////////////////////////////
/** Complete map of all TOF channels.
 * This class is used to hold and manage the informatin for all TOF Channel.
 */
class TOFChannelMap {
 public:
  TOFChannelMap() {};
  virtual ~TOFChannelMap();

  /// Initialize the map from text file.
  bool InitFromFile(string filename);

  /// Not implemented.
  void InitFromCDB();

 /** Return pointer to the TOF key.
 * This function returns pointer to the TOF channel key for the required DAQ channel.
 * \param[in] daqch DAQ channel to search for.
 * \return The key of the TOF channel connected to the given DAQ channel.
 */
  TOFChannelKey* find(DAQChannelKey* daqKey);
  
 /** Return pointer to the TOF key.
 * This function returns pointer to the TOF channel key for the required DAQ channel.
 * \param[in] daqch DAQ channel to search for, coded as string.
 * \return The key of the TOF channel connected to the given DAQ channel.
 */  
  TOFChannelKey* find(string daqKeyStr);
  
 /** Return pointer to the fADC DAQ key.
 * This function returns pointer to the fADC DAQ channel key coupled to the required TDC channel.
 * \param[in] daqch TDC channel to search for, coded as string.
 * \return The key of the fADC channel coupled to the given TDC channel.
 */   
  DAQChannelKey* findfAdcKey(string tdcKeyStr);
  
 /** Return pointer to the TDC DAQ key.
 * This function returns pointer to the TDC DAQ channel key coupled to the required fADC channel.
 * \param[in] daqch fADC channel to search for, coded as string.
 * \return The key of the TDC channel coupled to the given fADC channel.
 */   
  DAQChannelKey* findTdcKey(string adcKeyStr);
  
 private:
  std::vector<TOFChannelKey*> _tofKey;
  std::vector<DAQChannelKey*> _tdcKey;
  std::vector<DAQChannelKey*> _fadcKey;
};


#endif




