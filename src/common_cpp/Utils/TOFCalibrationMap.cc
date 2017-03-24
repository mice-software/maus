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

#include <exception>
#include "Utils/TOFCalibrationMap.hh"

#include "calibration/calibration.h"
#include "generated/CalibrationImplPortBinding.nsmap"

namespace MAUS {

TOFCalibrationMap::TOFCalibrationMap() {
    /*
  pymod_ok = true;
  if (!this->InitializePyMod()) pymod_ok = false;
  */
  runNumber = 0;
}

TOFCalibrationMap::~TOFCalibrationMap() {
  this->reset();
}

void TOFCalibrationMap::reset() {
  _Pkey.clear();
  _Tkey.clear();
  _twPar.resize(0);
  _t0.resize(0);
  _reff.resize(0);

  t0str.str("");
  t0str.clear();
  twstr.str("");
  twstr.clear();
  trigstr.str("");
  trigstr.clear();
}

bool TOFCalibrationMap::InitializeFromCards(Json::Value configJSON, int rnum) {
  // Fill the vector containing all TOF channel keys.
  this->reset();
  this->MakeTOFChannelKeys();

  // Get the calibration text files from the Json document.
  // This is no longer done. Calibration is got from CDB now
  // - DR, 12/11/12
  Json::Value t0_file;
  Json::Value tw_file;
  Json::Value trigger_file;
  std::string _calib_source = JsonWrapper::GetProperty(configJSON,
                                               "TOF_calib_source",
                                               JsonWrapper::stringValue).asString();
  // Find out how we want to get the calibrations
  // They are set by the TOF_calib_by flag in the data cards
  // They can be by
  // 1. run - to get calibrations by run number
  // 2. date - to get calibrations by valid date range
  // Default is by run
  try {
      _tof_calib_by = JsonWrapper::GetProperty(configJSON,
                                           "TOF_calib_by",
                                           JsonWrapper::stringValue).asString();
  } catch (MAUS::Exceptions::Exception e) {
    Squeak::mout(Squeak::error)
    << "Error getting data card TOF_calib_by" << std::endl
    << e.GetMessage() << std::endl;
    return false;
  }
  bool fromDB = true;
  if (_calib_source == "file") {
      fromDB = false;
      // Check what needs to be done.
      t0_file = JsonWrapper::GetProperty(configJSON,
                                         "TOF_T0_calibration_file",
                                         JsonWrapper::stringValue);

      tw_file = JsonWrapper::GetProperty(configJSON,
                                         "TOF_TW_calibration_file",
                                         JsonWrapper::stringValue);

      trigger_file = JsonWrapper::GetProperty(configJSON,
                                              "TOF_Trigger_calibration_file",
                                              JsonWrapper::stringValue);
  }
  // Check what needs to be done.
  _do_timeWalk_correction = JsonWrapper::GetProperty(configJSON,
                                                     "Enable_timeWalk_correction",
                                                     JsonWrapper::booleanValue).asBool();
  _do_triggerDelay_correction = JsonWrapper::GetProperty(configJSON,
                                                         "Enable_triggerDelay_correction",
                                                         JsonWrapper::booleanValue).asBool();
  _do_t0_correction = JsonWrapper::GetProperty(configJSON,
                                               "Enable_t0_correction",
                                               JsonWrapper::booleanValue).asBool();

  _tof_station = JsonWrapper::GetProperty(configJSON,
                                               "TOF_trigger_station",
                                               JsonWrapper::stringValue).asString();

  // convert trigger station name to upper case
  // the DB holds detector names in upper case
  std::transform(_tof_station.begin(), _tof_station.end(),
                                       _tof_station.begin(),
                                       std::ptr_fun<int, int>(std::toupper));

  _tof_calibdate = JsonWrapper::GetProperty(configJSON,
                                               "TOF_calib_date_from",
                                               JsonWrapper::stringValue).asString();
  // std::cout << "calib date: " << _tof_calibdate << std::endl;

  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
  if (!pMAUS_ROOT_DIR) {
    Squeak::mout(Squeak::error)
    << "Could not find the $MAUS_ROOT_DIR environmental variable." << std::endl;
    Squeak::mout(Squeak::error) << "Did you try running: source env.sh ?" << std::endl;
    return false;
  }
  runNumber = rnum;
//   std::cout << "#### Getting TOF calibration by " << _tof_calib_by
//             << "  Run " << runNumber << " ####" << std::endl;

  bool loaded;
  if (!fromDB) {
//       std::cout << "#### initializing from FILE ####" << std::endl;
      std::string xMapT0File = std::string(pMAUS_ROOT_DIR) + t0_file.asString();
      std::string xMapTWFile = std::string(pMAUS_ROOT_DIR) + tw_file.asString();
      std::string xMapTriggerFile = std::string(pMAUS_ROOT_DIR) + trigger_file.asString();
      // Load the calibration constants.
      loaded = this->Initialize(xMapT0File, xMapTWFile, xMapTriggerFile);
  } else {
//       std::cout << "#### initializing from CDB ####" << std::endl;
      // get calib from DB instead of file, the above line is replaced by the one below
      /*
      if (!pymod_ok) return false;
      */
      loaded = this->InitializeFromCDB();
  }
  if (!loaded)
    return false;

  return true;
}

bool TOFCalibrationMap::Initialize(std::string t0File,
                                   std::string twFile,
                                   std::string triggerFile) {
  bool status = LoadT0File(t0File) &&
                LoadTWFile(twFile) &&
                LoadTriggerFile(triggerFile);

  return status;
}

bool TOFCalibrationMap::InitializeFromCDB() {
  // bool status = LoadT0Calib() &&
  //              LoadTWCalib() &&
  //              LoadTriggerCalib();
  // The above lines have been replaces by the line below
  // The new functions get the calibs from the DB
  bool status = LoadT0Calib() && LoadTWCalib() && LoadTriggerCalib();

  return status;
}

int TOFCalibrationMap::MakeTOFChannelKeys() {
 /** Makes one TOFChannelKey for each channel of the TOF detector.
  * The size of _t0, _reff and _twPar is set here.
  * ATTENTION : The detector configuration is HARDCODED !!!!
  * TO BE IMPROVED !!!!
  */

  int nStation = 3;
  int nPlanes = 2;
  int nSlabs[] = {10, 7, 10};

  for (int st = 0; st < nStation; st++) {
    stringstream detector;
    detector << "tof" << st;
    for (int pl = 0; pl < nPlanes; pl++)
      for (int sl = 0; sl < nSlabs[st]; sl++)
        for (int pmt = 0; pmt < 2; pmt++)
          _Pkey.push_back(TOFChannelKey(st, pl, sl, pmt, detector.str()));
  }

  int nChannels = _Pkey.size();
  _t0.resize(nChannels);
  _reff.resize(nChannels);
  _twPar.resize(nChannels);
  return nChannels;
}

bool TOFCalibrationMap::LoadT0File(std::string t0File) {
  std::ifstream stream(t0File.c_str());
  if (!stream) {
    Squeak::mout(Squeak::error)
    << "Error in TOFCalibrationMap::LoadT0File : Can't open TOF calibration file."
    << t0File << std::endl;
    return false;
  }

  int reff;
  double p0;
  TOFChannelKey key;
  try {
    while (!stream.eof()) {
      stream >> key >> p0 >> reff;

      int n = FindTOFChannelKey(key);
      _t0[n] = p0;
      _reff[n] = reff;
      // std::cout << key << " pos:" << n << "  t0:" << p0 << "  reff:" << reff << std::endl;
    }
  } catch (MAUS::Exceptions::Exception e) {
    Squeak::mout(Squeak::error)
    << "Error in TOFCalibrationMap::LoadT0File : Error during loading. " << std::endl
    << e.GetMessage() << std::endl;
    return false;
  }

  return true;
}

bool TOFCalibrationMap::LoadTWFile(std::string twFile) {
  std::ifstream stream(twFile.c_str());
  if (!stream) {
    Squeak::mout(Squeak::error)
    << "Error in TOFCalibrationMap::LoadTWFile : Can't open TOF calibration file. "
    << twFile << std::endl;
    return false;
  }

  double p0, p1, p2, p3;
  TOFChannelKey key;
  try {
    while (!stream.eof()) {
      stream >> key >> p0 >> p1 >> p2 >> p3;

      int n = FindTOFChannelKey(key);
      _twPar[n].resize(4);
      _twPar[n][0] = p0;
      _twPar[n][1] = p1;
      _twPar[n][2] = p2;
      _twPar[n][3] = p3;
      // std::cout<< key << " pos:" << n << "  p0:" << p0 << "  p1:" << p1 << std::endl;
    }
  } catch (MAUS::Exceptions::Exception e) {
    Squeak::mout(Squeak::error)
    << "Error in TOFCalibrationMap::LoadTWFile : Error during loading. " << std::endl
    << e.GetMessage() << std::endl;
    return false;
  }

  return true;
}

bool TOFCalibrationMap::LoadTriggerFile(std::string triggerFile) {
  std::ifstream stream(triggerFile.c_str());
  if (!stream) {
    Squeak::mout(Squeak::error)
    << "Error in TOFCalibrationMap::LoadTriggerFile. Can't open TOF calibration file. "
    << triggerFile << std::endl;
    return false;
  }
  TOFPixelKey Pkey;
  double dt;
  try {
    while (!stream.eof()) {
      stream >> Pkey >> dt;

      _Tkey.push_back(Pkey);
      _Trt0.push_back(dt);
      // std::cout<< Pkey << "  dt:" << dt << std::endl;
    }
  } catch (MAUS::Exceptions::Exception e) {
    Squeak::mout(Squeak::error)
    << "Error in TOFCalibrationMap::LoadTriggerFile. Error during loading. " << std::endl
    << e.GetMessage() << std::endl;
    return false;
  }
  // Use the last readed pixel key to set the number of the trigger station.
  _triggerStation = Pkey.station();

  return true;
}

int TOFCalibrationMap::FindTOFChannelKey(TOFChannelKey key) const {
  for (unsigned int i = 0; i < _Pkey.size(); ++i )
    if (_Pkey.at(i) == key)
      return i;

  return NOCALIB;
}

int TOFCalibrationMap::FindTOFPixelKey(TOFPixelKey key) const {
  for (unsigned int i = 0; i < _Tkey.size(); ++i )
    if  (_Tkey.at(i) == key)
      return i;

  return NOCALIB;
}

double TOFCalibrationMap::T0(TOFChannelKey key, int &r) const {
  if (!_do_t0_correction)
    return 0.;

  int n = FindTOFChannelKey(key);

  if (n != NOCALIB) {
    r = _reff[n];
    if ( _t0[n] ) return _t0[n];
  }

  // std::cout << "TOFCalibrationMap -> No T0 calibration for " << key << std::endl;
  return NOCALIB;
}

double TOFCalibrationMap::TriggerT0(TOFPixelKey key) const {
  if (!_do_triggerDelay_correction)
    return 0.;

  int n = FindTOFPixelKey(key);
  if (n != NOCALIB)
    return _Trt0[n];

  // std::cout << "TOFCalibrationMap -> No Trigger calibration for " << key << std::endl;
  return n;
}

double TOFCalibrationMap::TW(TOFChannelKey key, int adc) const {
  if (!_do_timeWalk_correction)
    return 0.;

  int n = FindTOFChannelKey(key);
  // See equation 46 in MICE Note 251 "TOF Detectors Time Calibration".
  if (n != NOCALIB) {
    double x = adc + _twPar[n][0];
    double x2 = x*x;
    double p1 = _twPar[n][1];
    double p2 = _twPar[n][2]/x;
    double p3 =_twPar[n][3]/x2;
    double dt_tw = p1 + p2 + p3;
    if (_twPar[n][0] && _twPar[n][1] && _twPar[n][2] && _twPar[n][3])
      return dt_tw;
  }

  // std::cout << "TOFCalibrationMap -> No TW calibration for " << key << std::endl;
  return NOCALIB;
}

double TOFCalibrationMap::dT(TOFChannelKey Pkey, TOFPixelKey TrKey, int adc) const {
  // See equations 37-40 and 45 in MICE Note 251 "TOF Detectors Time Calibration".
  int reffSlab;
  double tw = TW(Pkey, adc);
  double t0 = T0(Pkey, reffSlab);
  double trt0 = TriggerT0(TrKey);
  // std::cout << "TOFCalibrationMap -> "<< Pkey << " " << TrKey << "  tw = " << tw;
  // std::cout << "  t0 = " << t0 << "  trt0 = " << trt0 << std::endl;
  if (tw == NOCALIB || t0 == NOCALIB || trt0 == NOCALIB) {
    return NOCALIB;
  }

  double dt = t0 - tw - trt0;

  // If this measurement is in the trigger station we need one additional correction.
  if (Pkey.station() == _triggerStation) {
    if (Pkey.plane() == 0) {
      TOFPixelKey refTr(_triggerStation, Pkey.slab(), reffSlab, Pkey.detector());
      if (TriggerT0(refTr) == NOCALIB)
        return NOCALIB;

      dt += TriggerT0(refTr);
      // std::cout << refTr << "  dt = " << TriggerT0(refTr) << std::endl;
    } else {
      TOFPixelKey refTr(_triggerStation, reffSlab, Pkey.slab(), Pkey.detector());
      if (TriggerT0(refTr) == NOCALIB)
        return NOCALIB;

      dt += TriggerT0(refTr);
       // std::cout << refTr << "  dt = " << TriggerT0(refTr) << std::endl;
    }
  }

  // std::cout << "TOFCalibrationMap -> dT = " << dt << std::endl;
  return dt*1e-3;
}

void TOFCalibrationMap::Print() {

  std::cout << "====================== TofCalibrationMap =========================" << std::endl;
  std::cout << " Name : " << _name << std::endl;
  std::cout << " Trigger in TOF" << _triggerStation << std::endl;
  std::cout << " Number of channels : " << _Pkey.size() << std::endl;
  std::cout << " Number of calibrated pixels in the trigger station : ";
  std::cout << _Tkey.size() << std::endl;

  for (unsigned int i = 0; i < _Pkey.size(); i++) {
    std::cout << _Pkey[i] << " T0 :" << _t0[i] << ", " << _reff[i];
    std::cout << "  TW:  "<< _twPar[i][0] << ", " << _twPar[i][1];
    std::cout << ", " << _twPar[i][2] << ", " << _twPar[i][3] << std::endl;
  }
  for (unsigned int i = 0; i < _Tkey.size(); i++)
    std::cout << _Tkey[i] << "  " << _Trt0[i] << std::endl;

  std::cout<< "===================================================================" << std::endl;
}

TOFPixelKey::TOFPixelKey(string keyStr) throw(MAUS::Exceptions::Exception) {
  std::stringstream xConv;
  try {
    xConv << keyStr;
    xConv >> (*this);
  }catch(MAUS::Exceptions::Exception e) {
    throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable,
                 std::string("corrupted TOF Pixel Key"),
                 "TOFPixelKey::TOFPixelKey(std::string)"));
  }
}

bool TOFPixelKey::operator==( const TOFPixelKey& key ) const {
  if ( _station == key._station &&
       _slabX == key._slabX &&
       _slabY == key._slabY &&
       _detector == key._detector) {
    return true;
  } else {
    return false;
  }
}

bool TOFPixelKey::operator!=( const TOFPixelKey& key ) const {
  if ( _station == key._station &&
       _slabX == key._slabX &&
       _slabY == key._slabY &&
       _detector == key._detector) {
    return false;
  } else {
    return true;
  }
}

ostream& operator<<( ostream& stream, TOFPixelKey key ) {
  stream << "TOFPixelKey " << key._station;
  stream << " " << key._slabX;
  stream << " " << key._slabY;
  stream << " " << key._detector;
  return stream;
}

istream& operator>>( istream& stream, TOFPixelKey &key ) throw(MAUS::Exceptions::Exception) {
  string xLabel;
  stream >> xLabel >> key._station >> key._slabX >> key._slabY >> key._detector;

  if (xLabel != "TOFPixelKey") {
    throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable,
                 std::string("corrupted TOF Pixel Key Extr"),
                 "istream& operator>>(istream& stream, TOFPixelKey)"));
  }

  return stream;
}

string TOFPixelKey::str() {
  stringstream xConv;
  xConv << (*this);
  return xConv.str();
}

/*
bool TOFCalibrationMap::InitializePyMod() {
  // import the get_tof_calib module
  // this python module access and gets calibrations from the DB
  _calib_mod = PyImport_ImportModule("calibration.get_tof_calib");
  if (_calib_mod == NULL) {
    std::cerr << "Failed to import get_tof_calib module" << std::endl;
    return false;
  }

  PyObject* calib_mod_dict = PyModule_GetDict(_calib_mod);
  if (calib_mod_dict != NULL) {
    PyObject* calib_init = PyDict_GetItemString
                                              (calib_mod_dict, "GetCalib");
    if (PyCallable_Check(calib_init)) {
        _tcalib = PyObject_Call(calib_init, NULL, NULL);
    }
  }
  if (_tcalib == NULL) {
    std::cerr << "Failed to instantiate get_tof_calib" << std::endl;
    return false;
  }
  return true;
}
*/
bool TOFCalibrationMap::GetCalibCAPI(std::string devname, std::string caltype) {
  MAUS::CDB::Calibration cali;
  std::string result;
  try {
      std::string status;
      cali.getStatus(status);
      if (status.compare("OK") != 0) {
          std::cerr << "+++ CDB Error status = " << status << std::endl;
          return false;
      }
      // std::cerr << " Calibration status returned " << status << std::endl;
      if (_tof_calib_by == "date") {
          std::cout << "++ Getting Calib by DATE for " << _tof_calibdate.c_str() << std::endl;
          cali.getDetectorCalibrationForDate(devname.c_str(),
                                             caltype.c_str(),
                                             _tof_calibdate.c_str(),
                                             result);
      } else if (_tof_calib_by == "run_number") {
          std::cout << "++ Getting Calib by RUN# " << runNumber << std::endl;
          cali.getDetectorCalibrationForRun(devname.c_str(),
                                            caltype.c_str(),
                                            runNumber,
                                            result);
      }
      // std::cerr << result << "(" << result.size() << " characters)" << std::endl;
  } catch (std::exception &e) {
      std::cerr << e.what() << std::endl;
      return false;
  }
  if (strcmp(caltype.c_str(), "t0") == 0)
      t0str.str(result);
  else if (strcmp(caltype.c_str(), "tw") == 0)
      twstr.str(result);
  else if (strcmp(caltype.c_str(), "trigger") == 0)
      trigstr.str(result);

  return true;
}

/*
void TOFCalibrationMap::GetCalib(std::string devname, std::string caltype) {
  PyObject *py_arg = NULL, *py_value = NULL;
  // setup the arguments to get_calib_func
  // the arguments are 3 strings
  // arg1 = device name (TOF0/TOF1/TOF2) uppercase
  // arg2 = calibration type (tw/t0/trigger) lowercase

  _get_calib_func = NULL;
  if (_tof_calib_by == "date") {
      py_arg = Py_BuildValue("(sss)", devname.c_str(), caltype.c_str(), _tof_calibdate.c_str());
      _get_calib_func = PyObject_GetAttrString(_tcalib, "get_calib");
  } else if (_tof_calib_by == "run_number") {
      py_arg = Py_BuildValue("(sis)", devname.c_str(), runNumber, caltype.c_str());
      _get_calib_func = PyObject_GetAttrString(_tcalib, "get_calib_for_run");
  } else {
      throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable,
                     "Invalid tof_calib_by type "+_tof_calib_by,
                     "TOFCalibrationMap::GetCalib"));
  }

  if (_get_calib_func == NULL)
      throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable,
                     "Failed to find get_calib function",
                     "TOFCalibrationMap::GetCalib"));

  if (py_arg == NULL) {
    PyErr_Clear();
    throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable,
              "Failed to resolve arguments to get_calib",
              "MAUSEvaluator::evaluate"));
    }
    if (_get_calib_func != NULL && PyCallable_Check(_get_calib_func)) {
        py_value = PyObject_CallObject(_get_calib_func, py_arg);
        // setup the streams to hold the different calibs
        if (py_value != NULL && strcmp(caltype.c_str(), "t0") == 0)
            t0str << PyString_AsString(py_value);
        if (strcmp(caltype.c_str(), "tw") == 0)
            twstr << PyString_AsString(py_value);
        if (strcmp(caltype.c_str(), "trigger") == 0)
            trigstr << PyString_AsString(py_value);
    }
    if (py_value == NULL) {
        PyErr_Clear();
        Py_XDECREF(py_arg);
        throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable,
                     "Failed to parse argument "+devname,
                     "GetCalib::get_calib"));
    }
    // clean up
    Py_XDECREF(py_value);
    Py_XDECREF(py_arg);
}
*/

bool TOFCalibrationMap::LoadT0Calib() {
  // this->GetCalib(_tof_station, "t0");
  if (!this->GetCalibCAPI(_tof_station, "t0"))
      return false;
  int reff;
  double p0;
  TOFChannelKey key;
  try {
    while (!t0str.eof()) {
      t0str >> key >> p0 >> reff;
      // std::cerr << key << "  t0:" << p0 << "  reff:" << reff << std::endl;
      int n = FindTOFChannelKey(key);
      _t0[n] = p0;
      _reff[n] = reff;
      // std::cerr << " pos:" << n << std::endl;
    }
  } catch (MAUS::Exceptions::Exception e) {
    Squeak::mout(Squeak::error)
    << "Error in TOFCalibrationMap::LoadT0Calib : Error during loading. " << std::endl
    << e.GetMessage() << std::endl;
    return false;
  }

  return true;
}

bool TOFCalibrationMap::LoadTWCalib() {
  // this->GetCalib(_tof_station, "tw");
  if (!this->GetCalibCAPI(_tof_station, "tw"))
      return false;
  double p0, p1, p2, p3;
  TOFChannelKey key;
  try {
    while (!twstr.eof()) {
      twstr >> key >> p0 >> p1 >> p2 >> p3;
      // std::cerr << "tw: " << key.str() << " " << key << std::endl;

      int n = FindTOFChannelKey(key);
      _twPar[n].resize(4);
      _twPar[n][0] = p0;
      _twPar[n][1] = p1;
      _twPar[n][2] = p2;
      _twPar[n][3] = p3;
       // std::cout<< key << " pos:" << n << "  p0:" << p0 << "  p1:" << p1 << std::endl;
    }
  } catch (MAUS::Exceptions::Exception e) {
    Squeak::mout(Squeak::error)
    << "Error in TOFCalibrationMap::LoadTWCalib : Error during loading. " << std::endl
    << e.GetMessage() << std::endl;
    return false;
  }

  return true;
}

bool TOFCalibrationMap::LoadTriggerCalib() {
  // this->GetCalib(_tof_station, "trigger");
  if (!this->GetCalibCAPI(_tof_station, "trigger"))
      return false;
  TOFPixelKey Pkey;
  double dt;
  try {
    while (!trigstr.eof()) {
      trigstr >> Pkey >> dt;

      _Tkey.push_back(Pkey);
      _Trt0.push_back(dt);
      // std::cerr << Pkey << "  dt:" << dt << std::endl;
    }
  } catch (MAUS::Exceptions::Exception e) {
    Squeak::mout(Squeak::error)
    << "Error in TOFCalibrationMap::LoadTriggerCalib. Error during loading. " << std::endl
    << e.GetMessage() << std::endl;
    return false;
  }
  // Use the last readed pixel key to set the number of the trigger station.
  _triggerStation = Pkey.station();

  return true;
}
}
