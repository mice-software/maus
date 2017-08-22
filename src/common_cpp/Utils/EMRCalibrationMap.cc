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
#include "Utils/EMRCalibrationMap.hh"
#include "Utils/EMRChannelMap.hh"

#include "calibration/calibration.h"
// #include "generated/CalibrationImplPortBinding.nsmap"

namespace MAUS {

EMRCalibrationMap::EMRCalibrationMap() {
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

  this->GetCalibCAPI("EMR", "eps", _calibDate);

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

bool EMRCalibrationMap::GetCalibCAPI(std::string devname, std::string caltype, std::string fromdate) {
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
      std::cout << "++ Getting EMR Calib by DATE for " << fromdate.c_str() << std::endl;
      if (fromdate.compare("current") == 0)
          cali.getCurrentDetectorCalibration(devname.c_str(),
                                             caltype.c_str(),
                                             result);
      else
          cali.getDetectorCalibrationForDate(devname.c_str(),
                                             caltype.c_str(),
                                             fromdate.c_str(),
                                             result);
      // std::cerr << result << "(" << result.size() << " characters)" << std::endl;
  } catch (std::exception &e) {
      std::cerr << e.what() << std::endl;
      return false;
  } // end try-catch
  epsstr.str(result);
  return true;
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

} // namespace MAUS
