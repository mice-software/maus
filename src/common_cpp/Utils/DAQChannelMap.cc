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

#include "Interface/Squeak.hh"
#include "Utils/DAQChannelMap.hh"

////////////////////////////////////////////////////////////

DAQChannelKey::DAQChannelKey(std::string keyStr) throw(Squeal) {
  std::stringstream xConv;
  try {
    xConv << keyStr;
    xConv >> (*this);
  }catch(Squeal e) {
    throw(Squeal(Squeal::recoverable,
                 std::string("corrupted DAQ Channel Key"),
                 "DAQChannelKey::DAQChannelKey(std::string)"));
  }
}

bool DAQChannelKey::operator== ( DAQChannelKey const key ) {
  if ( _ldcId == key._ldcId &&
       _geo == key._geo &&
       _channel == key._channel &&
       _eqType == key._eqType &&
       _detector == key._detector) {
    return true;
  } else {
    return false;
  }
}

bool DAQChannelKey::operator!= ( DAQChannelKey const key ) {
  if ( _ldcId == key._ldcId &&
       _geo == key._geo &&
       _channel == key._channel &&
       _eqType == key._eqType &&
       _detector == key._detector) {
    return false;
  } else {
    return true;
  }
}

std::ostream& operator<<( std::ostream& stream, const DAQChannelKey key ) {
  stream << "DAQChannelKey " << key._ldcId;
  stream << " " << key._geo;
  stream << " " << key._channel;
  stream << " " << key._eqType;
  stream << " " << key._detector;
  return stream;
}

std::istream& operator>>( std::istream& stream, DAQChannelKey &key ) throw(Squeal) {
  std::string xLabel;
  stream >> xLabel >> key._ldcId >> key._geo >> key._channel >> key._eqType >> key._detector;

  if (xLabel != "DAQChannelKey") {
    throw(Squeal(Squeal::recoverable,
                 std::string("corrupted DAQ Channel Key"),
                 "istream& operator>>(istream&, DAQChannelKey)"));
  }
  return stream;
}

std::string DAQChannelKey::str() {
  std::stringstream xConv;
  xConv << (*this);
  return xConv.str();
}

////////////////////////////////////////////////////////////

DAQChannelMap::~DAQChannelMap() {
  for (unsigned int i = 0;i < _chKey.size();i++)
    delete _chKey[i];

  _chKey.resize(0);
}

bool DAQChannelMap::InitFromFile(std::string filename) {
  std::ifstream stream(filename.c_str());
  if ( !stream ) {
    Squeak::mout(Squeak::error)
    << "Error in DAQChannelMap::InitFromFile: Can't open DAQ cabling file. "
    << filename << std::endl;
    return false;
  }
  std::stringstream key_s;
  DAQChannelKey* key;

  try {
    while ( !stream.eof() ) {
      key = new DAQChannelKey();
      stream >> *key;
      _chKey.push_back(key);
    }
  }catch(Squeal e) {
    Squeak::mout(Squeak::error)
    << "Error in DAQChannelMap::InitFromFile : Error during loading." << std::endl
    << e.GetMessage() << std::endl;
    return false;
  }

  if (_chKey.size() == 0) {
    Squeak::mout(Squeak::error)
    << "Error in DAQChannelMap::InitFromFile : No DAQ Channel Keys loaded. "
    << std::endl;
    return false;
  }
  return true;
}

bool DAQChannelMap::InitFromCDB() {}

DAQChannelKey* DAQChannelMap::find(int ldc, int geo, int ch, int eqType) {
  // To be optimised !!!
  for (unsigned int i = 0;i < _chKey.size();i++)
    if ( _chKey[i]->ldc() == ldc &&
        _chKey[i]->geo() == geo &&
        _chKey[i]->channel() == ch &&
        _chKey[i]->eqType() == eqType )
      return _chKey[i];

  return NULL;
}

DAQChannelKey* DAQChannelMap::find(std::string daqKeyStr) {
  DAQChannelKey xKey;
  std::stringstream xKeyConv;
  try {
    xKeyConv << daqKeyStr;
    xKeyConv >> xKey;
  }catch(Squeal e) {
    throw(Squeal(Squeal::recoverable,
                 std::string("corrupted DAQ Channel Key"),
                 "DAQChannelKey::DAQChannelKey(std::string)"));
  }

  return find(xKey.ldc(), xKey.geo(), xKey.channel(), xKey.eqType());
}

std::string DAQChannelMap::detector(int ldc, int geo, int ch, int eqType) {
  DAQChannelKey* key = find(ldc, geo, ch, eqType);
  if ( key )
    return key->detector();

  return "unknown";
}

