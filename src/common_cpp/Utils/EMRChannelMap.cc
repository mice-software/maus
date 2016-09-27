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

EMRChannelKey::EMRChannelKey()
  : _plane(-999), _orientation(-999), _bar(-999), _detector("unknown") {
}

EMRChannelKey::EMRChannelKey(int pl, int o, int b, string d)
  : _plane(pl), _orientation(o), _bar(b), _detector(d) {
}

EMRChannelKey::~EMRChannelKey() {
}

EMRChannelKey::EMRChannelKey(string keyStr) throw(Exceptions::Exception) {

  std::stringstream xConv;
  try {
    xConv << keyStr;
    xConv >> (*this);
  } catch (Exceptions::Exception e) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 std::string("corrupted EMR Channel Key"),
                 "EMRChannelKey::EMRChannelKey(std::string)"));
  }
}

bool EMRChannelKey::operator==( EMRChannelKey const key ) const {

  if ( _orientation == key._orientation &&
       _plane == key._plane &&
       _bar == key._bar &&
       _detector == key._detector ) {
    return true;
  } else {
    return false;
  }
}

bool EMRChannelKey::operator!=( EMRChannelKey const key ) const {

  if ( _orientation == key._orientation &&
       _plane == key._plane &&
       _bar == key._bar &&
       _detector == key._detector ) {
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

istream& operator>>( istream& stream, EMRChannelKey &key ) throw(Exceptions::Exception) {

  string xLabel;
  stream >> xLabel >> key._plane >> key._orientation >> key._bar >> key._detector;

  if (xLabel != "EMRChannelKey") {
    throw(Exceptions::Exception(Exceptions::recoverable,
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



EMRChannelMap::EMRChannelMap() {
}

EMRChannelMap::~EMRChannelMap() {
  for (size_t i = 0;i < _emrKey.size();i++) {

    delete _emrKey[i];
    delete _daqKey[i];
  }
  _daqKey.resize(0);
  _emrKey.resize(0);
}

bool EMRChannelMap::InitializeFromFile(string filename) {

  ifstream stream(filename.c_str());
  if ( !stream ) {
    Squeak::mout(Squeak::error)
    << "Error in EMRChannelMap::InitializeFromFile : Can't open EMR cabling file "
    << filename << std::endl;
    return false;
  }

  EMRChannelKey* emrkey;
  DAQChannelKey* daqKey;
  try {
    while (!stream.eof()) {
      emrkey = new EMRChannelKey();
      daqKey = new DAQChannelKey();
      stream >> *emrkey >> *daqKey;

      _emrKey.push_back(emrkey);
      _daqKey.push_back(daqKey);
    }
  } catch (Exceptions::Exception e) {
    Squeak::mout(Squeak::error)
      << "Error in EMRChannelMap::InitializeFromFile : Error during loading." << std::endl
      << e.GetMessage() << std::endl;
    return false;
  }

  if (_emrKey.size() == 0) {
    Squeak::mout(Squeak::error)
    << "Error in EMRChannelMap::InitializeFromFile : Nothing is loaded. "  << std::endl;
    return false;
  }
  return true;
}

void EMRChannelMap::InitializeFromCDB() {}

EMRChannelKey* EMRChannelMap::Find(DAQChannelKey *daqKey) const {

    for (size_t i = 0;i < _emrKey.size();i++) {
      if ( _daqKey[i]->eqType()  == daqKey->eqType() &&
           _daqKey[i]->ldc()     == daqKey->ldc() &&
           _daqKey[i]->geo()     == daqKey->geo() &&
           _daqKey[i]->channel() == daqKey->channel() ) {
        return _emrKey[i];
      }
    }
  return NULL;
}

EMRChannelKey* EMRChannelMap::Find(std::string daqKeyStr) {

  DAQChannelKey xDaqKey;
  stringstream xConv;
  try {
    xConv << daqKeyStr;
    xConv >> xDaqKey;
  }catch(Exceptions::Exception e) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 std::string("corrupted DAQ Channel Key"),
                 "EMRChannelMap::find(std::string)"));
  }
  EMRChannelKey* xKlKey = Find(&xDaqKey);
  return xKlKey;
}

int EMRChannelMap::GetOrientation(int plane) {

  for (size_t i = 0;i < _emrKey.size();i++) {
    if ( _emrKey[i]->GetPlane() == plane )
      return _emrKey[i]->GetOrientation();
  }

  return -999;
}

void EMRChannelMap::Print() {

  for (size_t i = 0; i < _emrKey.size(); i++)
    std::cerr << *(_emrKey[i]) << " " << *(_daqKey[i]) << std::endl;
}
}
