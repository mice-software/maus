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

void TOFChannelMap::InitFromFile(string filename) {
  ifstream stream(filename.c_str());
  if ( !stream ) {
    cerr << "Can't open TOF cabling file " << filename << endl;
    exit(1);
  }
	stringstream key_s;
	TOFChannelKey* tofkey;
	DAQChannelKey* tdckey;
	DAQChannelKey* fadckey;

  while (!stream.eof()) {
		tofkey = new TOFChannelKey();
		tdckey = new DAQChannelKey();
		fadckey = new DAQChannelKey();
		stream >> *tofkey >> *fadckey >> *tdckey;
		_tofKey.push_back(tofkey);
		_tdcKey.push_back(tdckey);
		_fadcKey.push_back(fadckey);
	}
}

void TOFChannelMap::InitFromCDB() {}

TOFChannelKey* TOFChannelMap::find(DAQChannelKey *daqch) {
	if (daqch->eqType() == 102)
	for (unsigned int i = 0;i < _tofKey.size();i++)
    if ( _tdcKey[i]->ldc()     == daqch->ldc() &&
        _tdcKey[i]->geo()     == daqch->geo() &&
        _tdcKey[i]->channel() == daqch->channel() ) {
      return _tofKey[i];
    }

	if (daqch->eqType() == 120)
	for (unsigned int i = 0;i < _tofKey.size();i++)
    if ( _fadcKey[i]->ldc()     == daqch->ldc() &&
        _fadcKey[i]->geo()     == daqch->geo() &&
        _fadcKey[i]->channel() == daqch->channel() ) {
      return _tofKey[i];
    }
  return NULL;
}

//////////////////////////////////////////////////////////////////////////

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

ostream& operator<<( ostream& stream, TOFChannelKey key ) {
	stream << "TOFChannelKey " << key._station;
	stream << " " << key._plane;
	stream << " " << key._slab;
	stream << " " << key._pmt;
	stream << " " << key._detector;
	return stream;
}

istream& operator>>( istream& stream, TOFChannelKey &key ) {
	string xLabel;
	stream >> xLabel >> key._station >> key._plane >> key._slab >> key._pmt >> key._detector;
	return stream;
}

string TOFChannelKey::str() {
	stringstream xConv;
	xConv << (*this);
	return xConv.str();
}


