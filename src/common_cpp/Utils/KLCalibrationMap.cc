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
#include "Utils/KLCalibrationMap.hh"
#include "Utils/KLChannelMap.hh"

#include "calibration/calibration.h"

namespace MAUS {

KLCalibrationMap::KLCalibrationMap() {
}

KLCalibrationMap::~KLCalibrationMap() {
  this->reset();
}

void KLCalibrationMap::reset() {
  _Pkey.resize(0);
  _gain.resize(0);

  gainstr.str("");
  gainstr.clear();
}

bool KLCalibrationMap::InitializeFromCards(Json::Value configJSON) {
  this->reset();

  // Fill the vector containing all KL channel keys.
  this->MakeKLChannelKeys();
  Json::Value gain_file;

  std::string _kl_source = JsonWrapper::GetProperty(configJSON,
                                               "KL_calib_source",
                                               JsonWrapper::stringValue).asString();

  // convert KL_calib_source datacard to uppercase
  std::transform(_kl_source.begin(), _kl_source.end(), _kl_source.begin(),
                                          std::ptr_fun<int, int>(std::tolower));
  if (_kl_source != "file" && _kl_source != "cdb") {
    Squeak::mout(Squeak::error)
    << "Invalid KL_calib_source datacard." << std::endl;
    return false;
  }
  bool fromDB = true;
  if (_kl_source == "file") {
      fromDB = false;
  // Get the calibration text files from the Json document.
  gain_file = JsonWrapper::GetProperty(configJSON,
                                                 "KL_calibration_file",
                                                 JsonWrapper::stringValue);
  }
  // Check what needs to be done.
  _do_gain_correction = JsonWrapper::GetProperty(configJSON,
                                               "Enable_klgain_correction",
                                               JsonWrapper::booleanValue).asBool();


  _kl_calibdate = JsonWrapper::GetProperty(configJSON,
                                               "KL_calib_date_from",
                                               JsonWrapper::stringValue).asString();

  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
  if (!pMAUS_ROOT_DIR) {
    Squeak::mout(Squeak::error)
    << "Could not find the $MAUS_ROOT_DIR environmental variable." << std::endl;
    Squeak::mout(Squeak::error) << "Did you try running: source env.sh ?" << std::endl;
    return false;
  }

  // Load the calibration constants.
  bool loaded;
  if (!fromDB) {
      std::string xMapGainFile = std::string(pMAUS_ROOT_DIR) + gain_file.asString();
      // Load the calibration constants.
      loaded = this->Initialize(xMapGainFile);
  } else {
      // get calib from DB instead of file, the above line is replaced by the one below
      loaded = this->InitializeFromCDB();
  }
  if (!loaded)
    return false;

  return true;
}

bool KLCalibrationMap::Initialize(std::string gainFile) {
  bool status = LoadGainFile(gainFile);
  return status;
}

bool KLCalibrationMap::InitializeFromCDB() {
  bool status = LoadGainCalib();

  return status;
}

int KLCalibrationMap::MakeKLChannelKeys() {
 /** Makes one KLChannelKey for each channel of the KL detector.
  * The size of gain vector is set here.
  */
  _Pkey.resize(0);

  int nCells = 21;
  stringstream detector;

  detector << "kl";

  for (int cell = 0; cell < nCells; cell++) {
    for (int pmt = 0; pmt < 2; pmt++)
      _Pkey.push_back(KLChannelKey(cell, pmt, detector.str()));
  }

  int nChannels = _Pkey.size();
  _gain.resize(nChannels);
  return nChannels;
}

bool KLCalibrationMap::LoadGainFile(std::string gainFile) {
  std::ifstream stream(gainFile.c_str());
  if (!stream) {
    Squeak::mout(Squeak::error)
    << "Error in KLCalibrationMap::LoadGainFile : Can't open KL calibration file."
    << gainFile << std::endl;
    return false;
  }

  double gain;
  KLChannelKey key;
  try {
    while (!stream.eof()) {
      stream >> key >> gain;

      int n = FindKLChannelKey(key);
      _gain[n] = gain;
    }
  } catch (MAUS::Exceptions::Exception e) {
    Squeak::mout(Squeak::error)
    << "Error in KLCalibrationMap::LoadKLFile : Error during loading. " << std::endl
    << e.GetMessage() << std::endl;
    return false;
  }
  return true;
}

int KLCalibrationMap::FindKLChannelKey(KLChannelKey key) const {
  for (unsigned int i = 0; i < _Pkey.size(); ++i )
    if (_Pkey.at(i) == key)
      return i;

  return NOCALIB;
}

double KLCalibrationMap::Gain(KLChannelKey key) const {
  if (!_do_gain_correction)
    return 1.;

  int n = FindKLChannelKey(key);

  if (n != NOCALIB) {
     if ( _gain[n] ) return _gain[n];
  }

  return NOCALIB;
}


void KLCalibrationMap::Print() {

  std::cout << "====================== KLCalibrationMap =========================" << std::endl;
  std::cout << " Number of channels : " << _Pkey.size() << std::endl;

  for (unsigned int i = 0; i < _Pkey.size(); i++) {
    std::cout << _Pkey[i] << " Gain :" << _gain[i]<< std::endl;
  }
  std::cout << "=================================================================" << std::endl;
}

bool KLCalibrationMap::GetCalibCAPI(std::string devname,
                                    std::string caltype,
                                    std::string fromdate) {
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
      std::cout << "++ Getting KL Calib by DATE for " << fromdate.c_str() << std::endl;
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
  gainstr.str(result);
  return true;
}

bool KLCalibrationMap::LoadGainCalib() {
  this->GetCalibCAPI("KL", "gain", _kl_calibdate);
  double gain;
  KLChannelKey key;
  try {
    while (!gainstr.eof()) {
      gainstr >> key >> gain;

      int n = FindKLChannelKey(key);
      _gain[n] = gain;
    }
  } catch (MAUS::Exceptions::Exception e) {
    Squeak::mout(Squeak::error)
    << "Error in KLCalibrationMap::LoadKLCalib : Error during loading. " << std::endl
    << e.GetMessage() << std::endl;
    return false;
  }

  return true;
}
}




