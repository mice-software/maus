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

#include "Utils/EMRCalibrationMap.hh"
#include "Utils/EMRChannelMap.hh"

namespace MAUS {

EMRCalibrationMap::EMRCalibrationMap() {

  pymod_ok = true;
  if ( !this->InitializePyMod() )
      pymod_ok = false;
}

EMRCalibrationMap::~EMRCalibrationMap() {

  this->reset();
}

void EMRCalibrationMap::reset() {

  _Ckey.clear();
  _eps_MA.resize(0);
  _eps_SA.resize(0);

  epsstr.str("");
  epsstr.clear();
}

bool EMRCalibrationMap::InitializeFromCards(Json::Value configJSON) {

  // Fetch variables
  _number_of_planes = configJSON["EMRnumberOfPlanes"].asInt();
  _number_of_bars = configJSON["EMRnumberOfBars"].asInt();
  _fom = configJSON["EMRfom"].asString();

  // Fill the vector containing all EMR channel keys.
  this->MakeEMRChannelKeys();
  std::string calibFile;

  // Get the calibration source and convert it to lower cases
  std::string source = JsonWrapper::GetProperty(configJSON,
                                                "EMR_calib_source",
                                                JsonWrapper::stringValue).asString();
  std::transform(source.begin(), source.end(), source.begin(),
		 std::ptr_fun<int, int>(std::tolower));

  if (source != "file" && source != "cdb") {
    Squeak::mout(Squeak::error)
    << "Invalid EMR_calib_source datacard." << std::endl;
    return false;
  }

  // Load the calibration file or find the right CDB entry
  bool fromDB = true;
  if ( source == "file" ) {
    char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
    if ( !pMAUS_ROOT_DIR ) {
      Squeak::mout(Squeak::error)
      << "Could not find the $MAUS_ROOT_DIR environmental variable." << std::endl;
      Squeak::mout(Squeak::error) << "Did you try running: source env.sh ?" << std::endl;
      return false;
    }

    // Get the calibration text files from the Json document.
    calibFile = std::string(pMAUS_ROOT_DIR)
	        + configJSON["EMR_calib_file"].asString();
    fromDB = false;
  } else {
    // Get the CDB calibration validity start date
    _calibDate = JsonWrapper::GetProperty(configJSON,
                                          "EMR_calib_date_from",
                                          JsonWrapper::stringValue).asString();
  }

  // Load the calibration constants, return false if it fails
  bool loaded;
  if ( !fromDB ) {
      loaded = this->Initialize(calibFile);
  } else {
      // Get calib from CDB instead of file
      if ( !pymod_ok )
	  return false;
      loaded = this->InitializeFromCDB();
  }
  if ( !loaded )
      return false;

  return true;
}

bool EMRCalibrationMap::InitializeFromCDB() {

  bool status = LoadFromCDB();
  return status;
}

bool EMRCalibrationMap::Initialize(std::string calibFile) {

  bool status = Load(calibFile);
  return status;
}

int EMRCalibrationMap::MakeEMRChannelKeys() {

  for (int iPlane = 0; iPlane < _number_of_planes; iPlane++)
    for (int iBar = -1; iBar < _number_of_bars; iBar++) // NB: average (-1), test channels (0)
      _Ckey.push_back(EMRChannelKey(iPlane, iPlane%2, iBar, "emr"));

  int nChannels = _Ckey.size();
  _eps_MA.resize(nChannels);
  _eps_SA.resize(nChannels);

  return nChannels;
}

bool EMRCalibrationMap::LoadFromCDB() {

  this->GetCalib("EMR", "eps", _calibDate);

  try {
    while ( !epsstr.eof() ) {
      char pmt[10], fom[10];
      int plane(-9), bar(-9);
      double epsilon(-1.0);
      epsstr >> pmt >> fom >> plane >> bar >> epsilon;

      int n = FindEMRChannelKey(EMRChannelKey(plane, plane%2, bar, "emr"));

      if ( strcmp(fom, _fom.c_str()) == 0 && n != NOCALIB ) {
        if ( strcmp(pmt, "MA") == 0 ) {
	  _eps_MA[n] = epsilon;
        } else if ( strcmp(pmt, "SA") == 0 ) {
	  _eps_SA[n] = epsilon;
	}
      }
    }
  } catch (MAUS::Exceptions::Exception e) {
    Squeak::mout(Squeak::error)
    << "Error in EMRCalibrationMap::LoadFromCDB() : Error during loading. " << std::endl
    << e.GetMessage() << std::endl;
    return false;
  }

  return true;
}

void EMRCalibrationMap::GetCalib(std::string devname, std::string caltype, std::string fromdate) {

  PyObject *py_arg = NULL, *py_value = NULL;
  // setup the arguments to get_calib_func
  // the arguments are 3 strings
  // arg1 = device name (EMR) uppercase
  // arg2 = calibration type (eps) lowercase
  // arg3 = valid_from_date == either "current" or an actual date 'YYYY-MM-DD HH:MM:SS'
  // default date argument is "current"
  // this is set via EMR_calib_date_from card in ConfigurationDefaults
  py_arg = Py_BuildValue("(sss)", devname.c_str(), caltype.c_str(), fromdate.c_str());
  if ( !py_arg ) {
    PyErr_Clear();
    throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable,
              "Failed to resolve arguments to get_calib",
              "MAUSEvaluator::evaluate"));
    }
    if ( _get_calib_func && PyCallable_Check(_get_calib_func) ) {
        py_value = PyObject_CallObject(_get_calib_func, py_arg);
        // setup the streams to hold the different calibs
        if ( py_value && strcmp(caltype.c_str(), "eps") == 0 )
            epsstr << PyString_AsString(py_value);
    }
    if ( !py_value ) {
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

bool EMRCalibrationMap::Load(std::string calibFile) {

  // Check the calibration file
  std::ifstream stream(calibFile.c_str());
  if ( !stream ) {
    Squeak::mout(Squeak::error)
    << "Error in EMRCalibrationMap::Load : Can't open EMR calibration file."
    << calibFile << std::endl;
    return false;
  }

  // Fill the arrays of correction factors
  try {
    while ( !stream.eof() ) {
      char pmt[10], fom[10];
      int plane(-9), bar(-9);
      double epsilon(-1.0);
      stream >> pmt >> fom >> plane >> bar >> epsilon;

      int n = FindEMRChannelKey(EMRChannelKey(plane, plane%2, bar, "emr"));

      if ( strcmp(fom, _fom.c_str()) == 0 && n != NOCALIB ) {
        if (strcmp(pmt, "MA") == 0) {
	  _eps_MA[n] = epsilon;
        } else if (strcmp(pmt, "SA") == 0) {
	  _eps_SA[n] = epsilon;
        }
      }
    }
  } catch (MAUS::Exceptions::Exception e) {
    Squeak::mout(Squeak::error)
    << "Error in EMRCalibrationMap::Load() : Error during loading. " << std::endl
    << e.GetMessage() << std::endl;
    return false;
  }

  return true;
}

int EMRCalibrationMap::FindEMRChannelKey(EMRChannelKey key) const {

  int xPlane = key.GetPlane();
  int xBar = key.GetBar();
  if ( xPlane > -1 && xPlane < _number_of_planes &&	// Can go from 0 to 47
       xBar > -2 && xBar < _number_of_bars )		// Can go from -1 to 59
      return (xPlane)*(_number_of_bars+1)+(xBar+1);

  return NOCALIB;
}

double EMRCalibrationMap::Eps(EMRChannelKey key, const char *pmt) const {

  int n = FindEMRChannelKey(key);
  double epsilon(0.);

  if ( n != NOCALIB ) {
    if ( strcmp(pmt, "MA") == 0 ) {
      epsilon = _eps_MA[n];
    } else if ( strcmp(pmt, "SA") == 0 ) {
      epsilon = _eps_SA[n];
    } else {
      Squeak::mout(Squeak::error) << "Wrong PMT ID" << std::endl;
    }
    if ( epsilon )
	return epsilon;
  }

  return NOCALIB;
}

void EMRCalibrationMap::Print() {
  std::cerr << "====================== EMRCalibrationMap =========================" << std::endl;
  std::cerr << " Number of channels : " << _Ckey.size() << std::endl;

  for (unsigned int i = 0; i < _Ckey.size(); i++)
    std::cerr << _Ckey[i] << " MA :" << _eps_MA[i] << ", SA :" << _eps_SA[i] << std::endl;

  std::cerr<< "===================================================================" << std::endl;
}

bool EMRCalibrationMap::InitializePyMod() {
  PyObject* calib_mod = PyImport_ImportModule("calibration.get_emr_calib");
  if ( !calib_mod ) {
    std::cerr << "Failed to import get_emr_calib module" << std::endl;
    return false;
  }

  PyObject* calib_mod_dict = PyModule_GetDict(calib_mod);
  PyObject* t_calib = NULL;
  if ( calib_mod_dict ) {
    PyObject* calib_init = PyDict_GetItemString(calib_mod_dict, "GetCalib");
    if ( PyCallable_Check(calib_init) ) {
        t_calib = PyObject_Call(calib_init, NULL, NULL);
    }
  }
  if ( !t_calib ) {
    std::cerr << "Failed to instantiate get_emr_calib" << std::endl;
    return false;
  }

  // Get the get_calib_func() function
  _get_calib_func = PyObject_GetAttrString(t_calib, "get_calib");
  if ( !_get_calib_func ) {
    std::cerr << "Failed to find get_calib function" << std::endl;
    return false;
  }
  return true;
}
} // namespace MAUS
