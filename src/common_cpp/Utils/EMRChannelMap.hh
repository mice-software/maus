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

#ifndef _MAUS_INPUTCPPREALDATA_CABLINGTOOLS_EMR_HH_
#define _MAUS_INPUTCPPREALDATA_CABLINGTOOLS_EMR_HH_

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

using std::string;
using std::ostream;
using std::istream;
using std::ifstream;
using std::stringstream;

namespace MAUS {

//////////////////////////////////////////////////////////////////////////////////////////////
/** Identifier for a single EMR channel.
 * This class is used to hold and manage all the information needed
 * to identifiy one channel in the EMR detectors.
 */
class EMRChannelKey {
 public:

  EMRChannelKey()
  :_plane(-999), _orientation(-999), _bar(-999), _detector("unknown") {}

  EMRChannelKey(int pl, int o, int b, string d)
  : _plane(pl), _orientation(o), _bar(b), _detector(d) {}

  explicit EMRChannelKey(string keyStr) throw(Exception);
  virtual ~EMRChannelKey() {}

  bool operator==( EMRChannelKey key ) const;
  bool operator!=( EMRChannelKey key ) const;

  friend ostream& operator<<( ostream& stream, EMRChannelKey key );
  friend istream& operator>>( istream& stream, EMRChannelKey &key ) throw(Exception);

  /** This function converts the DAQChannelKey into string.
  * \return String identifier.
  */
  string str();

  int plane()  const {return _plane;}
  int orientation() const {return _orientation;}
  int bar()    const {return _bar;}
  string detector() const {return _detector;}

  void SetPlane(int xPlane)   {_plane = xPlane;}
  void SetOrientation(int xOrientation) {_orientation = xOrientation;}
  void SetBar(int xBar)       {_bar = xBar;}
  void SetDetector(string xDetector) {_detector = xDetector;}

 private:

  /// Plane number.
  int _plane;

  /// Bar orientation.
  int _orientation;

  /// Bar number.
  int _bar;

  /// Name of the detector.
  string _detector;
};

//////////////////////////////////////////////////////////////////////////////////////////////
/** Complete map of all EMR channels.
 * This class is used to hold and manage the informatin for all EMR Channel.
 */
class EMRChannelMap {
 public:

  EMRChannelMap() {}
  virtual ~EMRChannelMap();

  /// Initialize the map from text file.
  bool InitFromFile(string filename);

  /// Not implemented.
  void InitFromCDB();

 /** Return pointer to the EMR key.
 * This function returns pointer to the EMR channel key for the required DAQ channel.
 * \param[in] daqch DAQ channel to search for.
 * \return The key of the EMR channel connected to the given DAQ channel.
 */
  EMRChannelKey* find(DAQChannelKey* daqKey) const;

 /** Return pointer to the EMR key.
 * This function returns pointer to the EMR channel key for the required DAQ channel.
 * \param[in] daqch DAQ channel to search for, coded as string.
 * \return The key of the EMR channel connected to the given DAQ channel.
 */
  EMRChannelKey* find(string daqKeyStr);
  int getOrientation(int plane);


  void print() {
    for (unsigned int i = 0; i < _emrKey.size(); i++)
      std::cout << *(_emrKey[i]) << " " << *(_dbbKey[i]) << std::endl;
  }

 private:

  std::vector<EMRChannelKey*>   _emrKey;
  std::vector<DAQChannelKey*>   _dbbKey;
};
}

#endif
