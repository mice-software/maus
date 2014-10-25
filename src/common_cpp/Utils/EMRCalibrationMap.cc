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
}

EMRCalibrationMap::~EMRCalibrationMap() {
  _Ckey.clear();
  _eps_MA.resize(0);
  _eps_SA.resize(0);
}

bool EMRCalibrationMap::InitializeFromCards(Json::Value configJSON) {
  // Fetch variables
  _number_of_planes = configJSON["EMRnumberOfPlanes"].asInt();
  _number_of_bars = configJSON["EMRnumberOfBars"].asInt();
  _fom = configJSON["EMRfom"].asString();

  // Fill the vector containing all EMR channel keys.
  this->MakeEMRChannelKeys();

  // Find calibration file
  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
  if (!pMAUS_ROOT_DIR) {
    Squeak::mout(Squeak::error)
    << "Could not find the $MAUS_ROOT_DIR environmental variable." << std::endl;
    Squeak::mout(Squeak::error) << "Did you try running: source env.sh ?" << std::endl;
    return false;
  }

  std::string calibFile = std::string(pMAUS_ROOT_DIR)
                 	+ configJSON["EMR_calibration_file"].asString();

  // Load calibration file
  bool loaded;
  loaded = this->Initialize(calibFile);

  if (!loaded)
    return false;

  return true;
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

bool EMRCalibrationMap::Load(std::string calibFile) {

  // Check the calibration file
  FILE *file = fopen(calibFile.c_str(), "r");
  if ( file == NULL ) {
    throw(Exception(Exception::recoverable,
          "Could not find EMR calibration map",
          "EMRCalibrationMap::Load"));
  }

  // Fill the arrays of correction factors
  char line[100];
  try {
    while (fgets(line, 100, file)) {
      char pmt[10], fom[10];
      int plane(-9), bar(-9);
      double epsilon(-1.0);
      sscanf(line, "%s %s %d %d %lf", pmt, fom, &plane, &bar, &epsilon);
      int n = FindEMRChannelKey(EMRChannelKey(plane, plane%2, bar, "emr"));

      if (strcmp(fom, _fom.c_str()) == 0 && n != NOCALIB) {
        if (strcmp(pmt, "MA") == 0) _eps_MA[n] = epsilon;
        if (strcmp(pmt, "SA") == 0) _eps_SA[n] = epsilon;
      }
    }

    fclose(file);
  } catch (MAUS::Exception e) {
    Squeak::mout(Squeak::error)
    << "Error in EMRCalibrationMap::Load : Error during loading. " << std::endl
    << e.GetMessage() << std::endl;
    return false;
  }

  return true;
}

int EMRCalibrationMap::FindEMRChannelKey(EMRChannelKey key) const {
  for (unsigned int i = 0; i < _Ckey.size(); i++ )
    if (_Ckey.at(i) == key)
      return i;

  return NOCALIB;
}

double EMRCalibrationMap::Eps(EMRChannelKey key, const char *pmt) const {

  int n = FindEMRChannelKey(key);

  double epsilon = 1.0;

  if (n != NOCALIB) {
    if (strcmp(pmt, "MA") == 0) epsilon = _eps_MA[n];
    else if (strcmp(pmt, "SA") == 0)
      epsilon = _eps_SA[n];
    else
      Squeak::mout(Squeak::error) << "Wrong PMT ID" << std::endl;

    if ( epsilon ) return epsilon;
  }

  // std::cout << "EMRCalibrationMap -> No " << pmt << " calibration for " << key << std::endl;
  return NOCALIB;
}

void EMRCalibrationMap::Print() {
  std::cout << "====================== EMRCalibrationMap =========================" << std::endl;
  std::cout << " Number of channels : " << _Ckey.size() << std::endl;

  for (unsigned int i = 0; i < _Ckey.size(); i++) {
    std::cout << _Ckey[i] << " MA :" << _eps_MA[i] << std::endl;
    std::cout << _Ckey[i] << " SA :" << _eps_SA[i] << std::endl;
  }

  std::cout<< "===================================================================" << std::endl;
}
}
