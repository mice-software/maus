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

#include "Utils/KLChannelMap.hh"

namespace MAUS {
KLChannelMap::~KLChannelMap() {
  for (unsigned int i = 0;i < _klKey.size();i++) {
    delete _klKey[i];
    delete _fadcKey[i];
  }
  _fadcKey.resize(0);
  _klKey.resize(0);
}

bool KLChannelMap::InitFromFile(string filename) {
  ifstream stream(filename.c_str());
  if ( !stream ) {
    Squeak::mout(Squeak::error)
    << "Error in KLChannelMap::InitFromFile : Can't open KL cabling file "
    << filename << std::endl;
    return false;
  }

  KLChannelKey* klkey;
  DAQChannelKey* fadckey;
  try {
    while (!stream.eof()) {
      klkey = new KLChannelKey();
      fadckey = new DAQChannelKey();
      stream >> *klkey >> *fadckey;
      _klKey.push_back(klkey);
      _fadcKey.push_back(fadckey);
    }
  } catch (Exception e) {
    Squeak::mout(Squeak::error)
      << "Error in KLChannelMap::InitFromFile : Error during loading." << std::endl
      << e.GetMessage() << std::endl;
    return false;
  }

  if (_klKey.size() == 0) {
    Squeak::mout(Squeak::error)
    << "Error in KLChannelMap::InitFromFile : Nothing is loaded. "  << std::endl;
    return false;
  }
  return true;
}

void KLChannelMap::InitFromCDB() {}

KLChannelKey* KLChannelMap::find(DAQChannelKey *daqKey) const {
  if (daqKey->eqType() == 120) {
    for (unsigned int i = 0;i < _klKey.size();i++) {
      if ( _fadcKey[i]->ldc()     == daqKey->ldc() &&
           _fadcKey[i]->geo()     == daqKey->geo() &&
           _fadcKey[i]->channel() == daqKey->channel() ) {
        return _klKey[i];
      }
    }
  }
  return NULL;
}

KLChannelKey* KLChannelMap::find(std::string daqKeyStr) const {
  DAQChannelKey xDaqKey;
  stringstream xConv;
  try {
    xConv << daqKeyStr;
    xConv >> xDaqKey;
  }catch(Exception e) {
    throw(Exception(Exception::recoverable,
                 std::string("corrupted DAQ Channel Key"),
                 "KLChannelMap::find(std::string)"));
  }
  KLChannelKey* xKlKey = find(&xDaqKey);
  return xKlKey;
}


//////////////////////////////////////////////////////////////////////////

KLChannelKey::KLChannelKey(string keyStr) throw(Exception) {
  std::stringstream xConv;
  try {
    xConv << keyStr;
    xConv >> (*this);
  }catch(Exception e) {
    throw(Exception(Exception::recoverable,
                 std::string("corrupted KL Channel Key"),
                 "KLChannelKey::KLChannelKey(std::string)"));
  }
}

bool KLChannelKey::operator==( KLChannelKey const key ) const {
  if ( _cell == key._cell &&
       _pmt == key._pmt &&
       _detector == key._detector) {
    return true;
  } else {
    return false;
  }
}

bool KLChannelKey::operator!=( KLChannelKey const key ) const {
  if ( _cell == key._cell &&
       _pmt == key._pmt &&
       _detector == key._detector) {
    return false;
  } else {
    return true;
  }
}

bool KLChannelKey::inSameCell(KLChannelKey key) {
  if (this->cell() == key.cell() &&
      this->detector() == key.detector()&&
      this->pmt() != key.pmt())
    return true;

  return false;
}

bool KLChannelKey::inSameCell(string keyStr) {
  KLChannelKey key(keyStr);
  return this->inSameCell(key);
}

KLChannelKey KLChannelKey::GetOppositeSidePMT() {
  KLChannelKey key;
  key.SetCell(this->cell());
  key.SetDetector(this->detector());
  if (this->pmt() == 0) key.SetPmt(1);
  if (this->pmt() == 1) key.SetPmt(0);

  return key;
}

string KLChannelKey::GetOppositeSidePMTStr() {
  KLChannelKey key = this->GetOppositeSidePMT();
  return key.str();
}

ostream& operator<<( ostream& stream, KLChannelKey key ) {
  stream << "KLChannelKey " << key._cell;
  stream << " " << key._pmt;
  stream << " " << key._detector;
  return stream;
}

istream& operator>>( istream& stream, KLChannelKey &key ) throw(Exception) {
  string xLabel;
  stream >> xLabel >> key._cell >> key._pmt >> key._detector;
  if (xLabel != "KLChannelKey") {
    throw(Exception(Exception::recoverable,
                 std::string("corrupted KL Channel Key"),
                 "istream& operator>>(istream& stream, KLChannelKey)"));
  }

  return stream;
}

string KLChannelKey::str() {
  stringstream xConv;
  xConv << (*this);
  return xConv.str();
}
}

