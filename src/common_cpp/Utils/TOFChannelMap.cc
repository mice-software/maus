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
#include "Globals/PyLibMausCpp.hh"
#include "cabling/cabling.h"
#include <exception>
#include "generated/CablingImplPortBinding.nsmap"

namespace MAUS {

TOFChannelMap::~TOFChannelMap() {
  this->reset();
}

void TOFChannelMap::reset() {
  for (unsigned int i = 0;i < _tofKey.size();i++) {
    delete _tofKey[i];
    delete _tdcKey[i];
    delete _fadcKey[i];
  }
  _tdcKey.resize(0);
  _fadcKey.resize(0);
  _tofKey.resize(0);

  cblstr.str("");
  cblstr.clear();
}

TOFChannelMap::TOFChannelMap() {
  pymod_ok = true;
  if (!this->InitializePyMod()) pymod_ok = false;
  runNumber = 0;
}

bool TOFChannelMap::InitializeCards(Json::Value configJSON, int rnum) {
  this->reset();

  _tof_station = JsonWrapper::GetProperty(configJSON,
                                               "TOF_trigger_station",
                                               JsonWrapper::stringValue).asString();

  // convert trigger station name to upper case
  // the DB holds detector names in upper case
  std::transform(_tof_station.begin(), _tof_station.end(),
                                       _tof_station.begin(),
                                       std::ptr_fun<int, int>(std::toupper));

  _tof_cablingdate = JsonWrapper::GetProperty(configJSON,
                                               "TOF_cabling_date_from",
                                               JsonWrapper::stringValue).asString();
  std::string _cabling_source = JsonWrapper::GetProperty(configJSON,
                                               "TOF_cabling_source",
                                               JsonWrapper::stringValue).asString();
  try {
      _tof_cabling_by = JsonWrapper::GetProperty(configJSON,
                                           "TOF_cabling_by",
                                           JsonWrapper::stringValue).asString();
  } catch (MAUS::Exceptions::Exception e) {
    Squeak::mout(Squeak::error)
    << "Error getting data card TOF_cabling_by" << std::endl
    << e.GetMessage() << std::endl;
    return false;
  }
//   std::cout << "TOF cabling date: " << _tof_cablingdate << std::endl;
//   std::cout << "TOF cabling source: " << _cabling_source << std::endl;

  MAUS::CDB::Cabling cbl;
  std::string result;
  try {
      std::string status;
      cbl.getStatus(status);
      std::cerr << " Cabling status returned " << status << std::endl;
      int run = 8681;
      cbl.getDetectorCablingForRun("tof1", run, result);
      std::cerr << result << "(" << result.size() << " characters)" << std::endl;
  } catch (std::exception &e) {
      std::cerr << e.what() << std::endl;
  }
  if (!pymod_ok) return false;
  runNumber = rnum;

  bool fromDB = true;
  if (_cabling_source == "file")
      fromDB = false;

//  std::cerr << ">>> SOURCE: " << fromDB << std::endl;
//  std::cerr << ">>> BY: " << _tof_cabling_by << std::endl;
  bool loaded;
  if (!fromDB) {
      std::string tof_cabling_file = JsonWrapper::GetProperty(configJSON,
                                         "TOF_cabling_file",
                                         JsonWrapper::stringValue).asString();
      loaded = this->InitFromFile(tof_cabling_file);
  } else {
      loaded = this->InitFromCDB();
  }
  if (!loaded)
    return false;

  return true;
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
  } catch (Exceptions::Exception e) {
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

bool TOFChannelMap::InitFromCDB() {
  this->GetCabling(_tof_station, _tof_cablingdate);
  std::cout << "got cabling" << std::endl;
  TOFChannelKey* tofkey;
  DAQChannelKey* tdckey;
  DAQChannelKey* fadckey;
  try {
    while (!cblstr.eof()) {
      tofkey = new TOFChannelKey();
      tdckey = new DAQChannelKey();
      fadckey = new DAQChannelKey();
      cblstr >> *tofkey >> *fadckey >> *tdckey;
      _tofKey.push_back(tofkey);
      _tdcKey.push_back(tdckey);
      _fadcKey.push_back(fadckey);
    }
  } catch (Exceptions::Exception e) {
    Squeak::mout(Squeak::error)
    << "Error in TOFChannelMap::InitFromCDB : Error during loading." << std::endl
    << e.GetMessage() << std::endl;
    return false;
  }

  if (_tofKey.size() == 0) {
    Squeak::mout(Squeak::error)
    << "Error in TOFChannelMap::InitFromCDB : Nothing is loaded. "  << std::endl;
    return false;
  }
  return true;
}

TOFChannelKey* TOFChannelMap::find(DAQChannelKey *daqKey) const {
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

TOFChannelKey* TOFChannelMap::find(std::string daqKeyStr) const {
  DAQChannelKey xDaqKey;
  stringstream xConv;
  try {
    xConv << daqKeyStr;
    xConv >> xDaqKey;
  }catch(Exceptions::Exception e) {
    throw(Exceptions::Exception(Exceptions::recoverable,
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
  }catch(Exceptions::Exception e) {
    throw(Exceptions::Exception(Exceptions::recoverable,
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
  }catch(Exceptions::Exception e) {
    throw(Exceptions::Exception(Exceptions::recoverable,
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

TOFChannelKey::TOFChannelKey(string keyStr) throw(Exceptions::Exception) {
  std::stringstream xConv;
  try {
    xConv << keyStr;
    xConv >> (*this);
  }catch(Exceptions::Exception e) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 std::string("corrupted TOF Channel Key"),
                 "TOFChannelKey::TOFChannelKey(std::string)"));
  }
}

bool TOFChannelKey::operator==( const TOFChannelKey& key ) const {
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

bool TOFChannelKey::operator!=( const TOFChannelKey& key ) const {
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

istream& operator>>( istream& stream, TOFChannelKey &key ) throw(Exceptions::Exception) {
  string xLabel;
  stream >> xLabel >> key._station >> key._plane >> key._slab >> key._pmt >> key._detector;

  if (xLabel != "TOFChannelKey") {
    throw(Exceptions::Exception(Exceptions::recoverable,
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


bool TOFChannelMap::InitializePyMod() {
  // import the get_tof_cabling module
  // this python module access and gets cabling from the DB
  PyLibMausCpp::initlibMausCpp();
  _cabling_mod = PyImport_ImportModule("calibration.get_tof_cabling");
  if (_cabling_mod == NULL) {
    std::cerr << "Failed to import get_tof_cabling module" << std::endl;
    return false;
  }

  PyObject* cabling_mod_dict = PyModule_GetDict(_cabling_mod);
  if (cabling_mod_dict != NULL) {
    PyObject* cabling_init = PyDict_GetItemString
                                              (cabling_mod_dict, "GetCabling");
    if (PyCallable_Check(cabling_init)) {
        _tcabling = PyObject_Call(cabling_init, NULL, NULL);
    }
  }
  if (_tcabling == NULL) {
    std::cerr << "Failed to instantiate get_tof_cabling" << std::endl;
    return false;
  }

    // get the get_cabling_func() function
  _get_cabling_func = PyObject_GetAttrString(_tcabling, "get_cabling");
  if (_get_cabling_func == NULL) {
    std::cerr << "Failed to find get_cabling function" << std::endl;
    return false;
  }
  return true;
}

void TOFChannelMap::GetCabling(std::string devname, std::string fromdate) {
  PyObject *py_arg = NULL, *py_value = NULL;
  // setup the arguments to get_calib_func
  // the arguments are 2 strings
  // arg1 = device name (TOF0/TOF1/TOF2) uppercase
  // this is actually not very meaningful since the cabling is for TOF not TOF0/1/2
  // however the cabling schema does not have a generic "TOF" device
  // hence using the trigger station as the default device
  // arg2 = valid_from_date == either "current" or an actual date 'YYYY-MM-DD HH:MM:SS'
  // default date argument is "current"
  // this is set via TOF_cabling_date_from card in ConfigurationDefaults
  if (_tof_cabling_by == "date") {
      py_arg = Py_BuildValue("(ss)", devname.c_str(), fromdate.c_str());
      _get_cabling_func = PyObject_GetAttrString(_tcabling, "get_cabling");
  } else if (_tof_cabling_by == "run_number") {
      py_arg = Py_BuildValue("(si)", devname.c_str(), runNumber);
      _get_cabling_func = PyObject_GetAttrString(_tcabling, "get_cabling_for_run");
  }
  std::cout << "Building" << std::endl;
  if (py_arg == NULL) {
    PyErr_Clear();
    throw(Exceptions::Exception(Exceptions::recoverable,
              "Failed to resolve arguments to get_cabling",
              "MAUSEvaluator::evaluate"));
    }
    if (_get_cabling_func != NULL && PyCallable_Check(_get_cabling_func)) {
        py_value = PyObject_CallObject(_get_cabling_func, py_arg);
        // setup the stream to hold the cabling
        if (py_value != NULL)
            cblstr << PyString_AsString(py_value);
    }
    if (py_value == NULL) {
        PyErr_Clear();
        Py_XDECREF(py_arg);
        throw(Exceptions::Exception(Exceptions::recoverable,
                     "Failed to parse argument "+devname,
                     "GetCabling::get_cabling"));
    }
    // clean up
    Py_XDECREF(py_value);
    Py_XDECREF(py_arg);
}
}
