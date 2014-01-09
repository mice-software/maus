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

#include "Utils/EMRChannelMap.hh"
#include "Utils/Exception.hh"

namespace MAUS {

EMRChannelMap::~EMRChannelMap() {
  for (unsigned int i = 0;i < _emrKey.size();i++) {
    delete _emrKey[i];
    delete _dbbKey[i];
  }
  _dbbKey.resize(0);
  _emrKey.resize(0);
}

bool EMRChannelMap::InitFromFile(string filename) {
  ifstream stream(filename.c_str());
  if ( !stream ) {
    Squeak::mout(Squeak::error)
    << "Error in EMRChannelMap::InitFromFile : Can't open EMR cabling file "
    << filename << std::endl;
    return false;
  }

  EMRChannelKey* emrkey;
  DAQChannelKey* dbbkey;
  try {
    while (!stream.eof()) {
      emrkey = new EMRChannelKey();
      dbbkey = new DAQChannelKey();
      stream >> *emrkey >> *dbbkey;

      _emrKey.push_back(emrkey);
      _dbbKey.push_back(dbbkey);
    }
  } catch(Exception e) {
    Squeak::mout(Squeak::error)
      << "Error in EMRChannelMap::InitFromFile : Error during loading." << std::endl
      << e.GetMessage() << std::endl;
    return false;
  }

  if (_emrKey.size() == 0) {
    Squeak::mout(Squeak::error)
    << "Error in EMRChannelMap::InitFromFile : Nothing is loaded. "  << std::endl;
    return false;
  }
  return true;
}

void EMRChannelMap::InitFromCDB() {}

EMRChannelKey* EMRChannelMap::find(DAQChannelKey *daqKey) {
    for (unsigned int i = 0;i < _emrKey.size();i++) {
      if ( _dbbKey[i]->eqType()  == daqKey->eqType() &&
           _dbbKey[i]->ldc()     == daqKey->ldc() &&
           _dbbKey[i]->geo()     == daqKey->geo() &&
           _dbbKey[i]->channel() == daqKey->channel() ) {
        return _emrKey[i];
      }
    }
  return NULL;
}

EMRChannelKey* EMRChannelMap::find(std::string daqKeyStr) {
  DAQChannelKey xDaqKey;
  stringstream xConv;
  try {
    xConv << daqKeyStr;
    xConv >> xDaqKey;
  }catch(Exception e) {
    throw(Exception(Exception::recoverable,
                 std::string("corrupted DAQ Channel Key"),
                 "EMRChannelMap::find(std::string)"));
  }
  EMRChannelKey* xKlKey = find(&xDaqKey);
  return xKlKey;
}

int EMRChannelMap::getOrientation(int plane) {
  for (unsigned int i = 0;i < _emrKey.size();i++) {
    if ( _emrKey[i]->plane() == plane )
      return _emrKey[i]->orientation();
  }

  return -999;
}
//////////////////////////////////////////////////////////////////////////

EMRChannelKey::EMRChannelKey(string keyStr) throw(Exception) {
  std::stringstream xConv;
  try {
    xConv << keyStr;
    xConv >> (*this);
  }catch(Exception e) {
    throw(Exception(Exception::recoverable,
                 std::string("corrupted EMR Channel Key"),
                 "EMRChannelKey::EMRChannelKey(std::string)"));
  }
}

bool EMRChannelKey::operator==( EMRChannelKey const key ) {
  if ( _orientation == key._orientation &&
       _plane == key._plane &&
       _bar == key._bar &&
       _detector == key._detector) {
    return true;
  } else {
    return false;
  }
}

bool EMRChannelKey::operator!=( EMRChannelKey const key ) {
  if ( _orientation == key._orientation &&
       _plane == key._plane &&
       _bar == key._bar &&
       _detector == key._detector) {
    return false;
  } else {
    return true;
  }
}

ostream& operator<<( ostream& stream, EMRChannelKey key ) {
  stream << "EMRChannelKey " << key._plane;
  stream << " " << key._orientation;
  stream << " " << key._bar;
  stream << " " << key._detector;
  return stream;
}

istream& operator>>( istream& stream, EMRChannelKey &key ) throw(Exception) {
  string xLabel;
  stream >> xLabel >> key._plane >> key._orientation >> key._bar >> key._detector;

  if (xLabel != "EMRChannelKey") {
    throw(Exception(Exception::recoverable,
                 std::string("corrupted EMR Channel Key"),
                 "istream& operator>>(istream& stream, EMRChannelKey)"));
  }

  return stream;
}

string EMRChannelKey::str() {
  stringstream xConv;
  xConv << (*this);
  return xConv.str();
}
}
