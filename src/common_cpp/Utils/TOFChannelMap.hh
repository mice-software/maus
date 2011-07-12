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

using std::string;
using std::ostream;
using std::istream;
using std::ifstream;
using std::cout;
using std::endl;
using std::vector;
using std::stringstream;
using std::cerr;

//////////////////////////////////////////////////////////////////////////////////////////////

class TOFChannelKey {
 public:
  TOFChannelKey()
  :_station(-999), _plane(-999), _slab(-999), _pmt(-999), _detector("unknown") {}

  TOFChannelKey(int l, int g, int ch, int e, string d)
  :_station(l), _plane(g), _slab(ch), _pmt(e), _detector(d) {}
	virtual ~TOFChannelKey() {}

  bool operator==( TOFChannelKey key );
	bool operator!=( TOFChannelKey key );

  friend ostream& operator<<( ostream& stream, TOFChannelKey key );
  friend istream& operator>>( istream& stream, TOFChannelKey &key );

	string detector() {return _detector;}
	string str();
  int station()     {return _station;}
  int plane()     {return _plane;}
  int slab() {return _slab;}
  int pmt()  {return _pmt;}
	int make_TOFChannelKey_id() { return _station*1e7 + _plane*1e5 + _slab*1e3 + _pmt; }

  int _station;
  int _plane;
  int _slab;
  int _pmt;
	string _detector;
};

//////////////////////////////////////////////////////////////////////////////////////////////

class TOFChannelMap {
 public:
  TOFChannelMap() {}
  virtual ~TOFChannelMap();

  void InitFromFile(string filename);
  void InitFromCDB();
  TOFChannelKey* find(DAQChannelKey* daqch);

 private:
  vector<TOFChannelKey*> _tofKey;
  vector<DAQChannelKey*> _tdcKey;
	vector<DAQChannelKey*> _fadcKey;
};


#endif




