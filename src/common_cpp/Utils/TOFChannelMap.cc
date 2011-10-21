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

#include "Utils/TOFChannelMap.hh"

TOFChannelMap::~TOFChannelMap() {
  for (unsigned int i = 0;i < _tofKey.size();i++) {
    delete _tofKey[i];
    delete _tdcKey[i];
    delete _fadcKey[i];
  }
  _tdcKey.resize(0);
  _fadcKey.resize(0);
  _tofKey.resize(0);
}

bool TOFChannelMap::InitFromFile(string filename) {
  ifstream stream(filename.c_str());
  if ( !stream ) {
    Squeak::mout(Squeak::error)
    << "Error in TOFChannelMap::InitFromFile : Can't open TOF cabling file "
    << filename << std::endl;
    return false;
  }

  TOFChannelKey* tofkey;
  DAQChannelKey* tdckey;
  DAQChannelKey* fadckey;
  try {
    while (!stream.eof()) {
      tofkey = new TOFChannelKey();
      tdckey = new DAQChannelKey();
      fadckey = new DAQChannelKey();
      stream >> *tofkey >> *fadckey >> *tdckey;
      _tofKey.push_back(tofkey);
      _tdcKey.push_back(tdckey);
      _fadcKey.push_back(fadckey);
    }
  } catch(Squeal e) {
    Squeak::mout(Squeak::error)
    << "Error in TOFChannelMap::InitFromFile : Error during loading." << std::endl
    << e.GetMessage() << std::endl;
    return false;
  }

  if (_tofKey.size() == 0) {
    Squeak::mout(Squeak::error)
    << "Error in TOFChannelMap::InitFromFile : Nothing is loaded. "  << std::endl;
    return false;
  }
  return true;
}

void TOFChannelMap::InitFromCDB() {}

TOFChannelKey* TOFChannelMap::find(DAQChannelKey *daqKey) {
  if (daqKey->eqType() == 102)
    for (unsigned int i = 0;i < _tofKey.size();i++) {
      if ( _tdcKey[i]->ldc()     == daqKey->ldc() &&
           _tdcKey[i]->geo()     == daqKey->geo() &&
           _tdcKey[i]->channel() == daqKey->channel() ) {
        return _tofKey[i];
      }
    }

  if (daqKey->eqType() == 120) {
    for (unsigned int i = 0;i < _tofKey.size();i++) {
      if ( _fadcKey[i]->ldc()     == daqKey->ldc() &&
           _fadcKey[i]->geo()     == daqKey->geo() &&
           _fadcKey[i]->channel() == daqKey->channel() ) {
        return _tofKey[i];
      }
    }
  }
  return NULL;
}

TOFChannelKey* TOFChannelMap::find(std::string daqKeyStr) {
  DAQChannelKey xDaqKey;
  stringstream xConv;
  try {
    xConv << daqKeyStr;
    xConv >> xDaqKey;
  }catch(Squeal e) {
    throw(Squeal(Squeal::recoverable,
                 std::string("corrupted DAQ Channel Key"),
                 "TOFChannelMap::find(std::string)"));
  }
  TOFChannelKey* xTofKey = find(&xDaqKey);
  return xTofKey;
}

DAQChannelKey* TOFChannelMap::findfAdcKey(std::string tdcKeyStr) {
  DAQChannelKey xDaqKey;
  stringstream xConv;
  try {
    xConv << tdcKeyStr;
    xConv >> xDaqKey;
  }catch(Squeal e) {
    throw(Squeal(Squeal::recoverable,
                 std::string("corrupted DAQ Channel Key"),
                 "TOFChannelMap::findfAdcKey(std::string)"));
  }

  if (xDaqKey.eqType() == 102) {
    for (unsigned int i = 0;i < _tofKey.size();i++) {
      if ( *(_tdcKey[i]) == xDaqKey ) {
        return _fadcKey[i];
      }
    }
  }
  return NULL;
}

DAQChannelKey* TOFChannelMap::findTdcKey(std::string adcKeyStr) {
  DAQChannelKey xDaqKey;
  stringstream xConv;
  try {
    xConv << adcKeyStr;
    xConv >> xDaqKey;
  }catch(Squeal e) {
    throw(Squeal(Squeal::recoverable,
                 std::string("corrupted DAQ Channel Key"),
                 "TOFChannelMap::findTdcKey(std::string)"));
  }

  if (xDaqKey.eqType() == 120) {
    for (unsigned int i = 0;i < _tofKey.size();i++) {
      if ( *(_fadcKey[i])     == xDaqKey ) {
        return _tdcKey[i];
      }
    }
  }
  return NULL;
}

//////////////////////////////////////////////////////////////////////////

TOFChannelKey::TOFChannelKey(string keyStr) throw(Squeal) {
  std::stringstream xConv;
  try {
    xConv << keyStr;
    xConv >> (*this);
  }catch(Squeal e) {
    throw(Squeal(Squeal::recoverable,
                 std::string("corrupted TOF Channel Key"),
                 "TOFChannelKey::TOFChannelKey(std::string)"));
  }
}

bool TOFChannelKey::operator==( TOFChannelKey const key ) {
  if ( _station == key._station &&
       _plane == key._plane &&
       _slab == key._slab &&
       _pmt == key._pmt &&
       _detector == key._detector) {
    return true;
  } else {
    return false;
  }
}

bool TOFChannelKey::operator!=( TOFChannelKey const key ) {
  if ( _station == key._station &&
       _plane == key._plane &&
       _slab == key._slab &&
       _pmt == key._pmt &&
       _detector == key._detector) {
    return false;
  } else {
    return true;
  }
}

bool TOFChannelKey::inSameSlab(TOFChannelKey key) {
  if (this->station() == key.station() &&
      this->plane() == key.plane() &&
      this->slab() == key.slab() &&
      this->detector() == key.detector()&&
      this->pmt() != key.pmt())
    return true;

  return false;
}

bool TOFChannelKey::inSameSlab(string keyStr) {
  TOFChannelKey key(keyStr);
  return this->inSameSlab(key);
}

TOFChannelKey TOFChannelKey::GetOppositeSidePMT() {
  TOFChannelKey key;
  key.SetStation(this->station());
  key.SetPlane(this->plane());
  key.SetSlab(this->slab());
  key.SetDetector(this->detector());
  if (this->pmt() == 0) key.SetPmt(1);
  if (this->pmt() == 1) key.SetPmt(0);

  return key;
}

string TOFChannelKey::GetOppositeSidePMTStr() {
  TOFChannelKey key = this->GetOppositeSidePMT();
  return key.str();
}

ostream& operator<<( ostream& stream, TOFChannelKey key ) {
  stream << "TOFChannelKey " << key._station;
  stream << " " << key._plane;
  stream << " " << key._slab;
  stream << " " << key._pmt;
  stream << " " << key._detector;
  return stream;
}

istream& operator>>( istream& stream, TOFChannelKey &key ) throw(Squeal) {
  string xLabel;
  stream >> xLabel >> key._station >> key._plane >> key._slab >> key._pmt >> key._detector;

  if (xLabel != "TOFChannelKey") {
    throw(Squeal(Squeal::recoverable,
                 std::string("corrupted TOF Channel Key"),
                 "istream& operator>>(istream& stream, TOFChannelKey)"));
  }

  return stream;
}

string TOFChannelKey::str() {
  stringstream xConv;
  xConv << (*this);
  return xConv.str();
}


