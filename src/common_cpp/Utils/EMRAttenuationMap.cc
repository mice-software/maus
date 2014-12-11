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

#include "Utils/EMRAttenuationMap.hh"
#include "Utils/EMRChannelMap.hh"

namespace MAUS {

EMRAttenuationMap::EMRAttenuationMap() {
}

EMRAttenuationMap::~EMRAttenuationMap() {
  _Ckey.clear();
  _caf_MA.resize(0);
  _caf_SA.resize(0);
  _cfl_MA.resize(0);
  _cfl_SA.resize(0);
}

bool EMRAttenuationMap::InitializeFromCards(Json::Value configJSON) {
  // Fetch variables
  _number_of_planes = configJSON["EMRnumberOfPlanes"].asInt();
  _number_of_bars = configJSON["EMRnumberOfBars"].asInt();

  _bar_length = configJSON["EMRbarLength"].asInt();
  _atten_WLSf = configJSON["EMRattenWLSf"].asDouble();
  _atten_CLRf = configJSON["EMRattenCLRf"].asDouble();

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

  std::string connectorAttenFile = std::string(pMAUS_ROOT_DIR)
			   	 + configJSON["EMR_connector_attenuation_map"].asString();

  std::string cfLengthFile = std::string(pMAUS_ROOT_DIR)
			   + configJSON["EMR_clear_fiber_length_map"].asString();

  // Load calibration file
  bool loaded;
  loaded = this->Initialize(connectorAttenFile, cfLengthFile);

  if (!loaded)
    return false;

  return true;
}

bool EMRAttenuationMap::Initialize(std::string connectorAttenFile, std::string cfLengthFile) {

  bool status = LoadConnectorAttenFile(connectorAttenFile) &&
		LoadCfLengthFile(cfLengthFile);
  return status;
}

int EMRAttenuationMap::MakeEMRChannelKeys() {
  for (int iPlane = 0; iPlane < _number_of_planes; iPlane++)
    for (int iBar = 1; iBar < _number_of_bars; iBar++)
      _Ckey.push_back(EMRChannelKey(iPlane, iPlane%2, iBar, "emr"));

  int nChannels = _Ckey.size();
  _caf_MA.resize(nChannels);
  _caf_SA.resize(nChannels);

  _cfl_MA.resize(nChannels);
  _cfl_SA.resize(nChannels);

  return nChannels;
}

bool EMRAttenuationMap::LoadConnectorAttenFile(std::string connectorAttenFile) {

  // Check the calibration file
  FILE *file = fopen(connectorAttenFile.c_str(), "r");
  if ( file == NULL ) {
    throw(Exception(Exception::recoverable,
          "Could not find EMR connector attenuation map",
          "EMRAttenuationMap::LoadConnectorAtten"));
  }

  // Fill the arrays of correction factors
  char line[100];
  try {
    while (fgets(line, 100, file)) {
      int pmid(-1), chid(-1);
      float attenuation(-1.0);
      sscanf(line, "%d  %d  %f", &pmid, &chid, &attenuation);
      if (pmid == 0) {
	int n = FindEMRChannelKey(EMRChannelKey(chid/59, (chid/59)%2, chid%59+1, "emr"));
        _caf_MA[n] = attenuation;
      }
      if (pmid == 1) {
	int n = FindEMRChannelKey(EMRChannelKey(chid/59, (chid/59)%2, chid%59+1, "emr"));
        _caf_SA[n] = attenuation;
      }
    }

    fclose(file);
  } catch (MAUS::Exception e) {
    Squeak::mout(Squeak::error)
    << "Error in EMRAttenuationMap::LoadConnectorAtten : Error during loading. " << std::endl
    << e.GetMessage() << std::endl;
    return false;
  }

  return true;
}

bool EMRAttenuationMap::LoadCfLengthFile(std::string cfLengthFile) {

  // Check the calibration file
  FILE *file = fopen(cfLengthFile.c_str(), "r");
  if ( file == NULL ) {
    throw(Exception(Exception::recoverable,
          "Could not find EMR clear fibre length",
          "EMRAttenuationMap::LoadCfLength"));
  }

  // Fill the arrays of correction factors
  char line[100];
  try {
    while (fgets(line, 100, file)) {
      int pmid(-1), barid(-1);
      float length(-1.0);
    sscanf(line, "%d  %d  %f", &pmid, &barid, &length);
      for (int iPlane = 0; iPlane < _number_of_planes; iPlane++) {
        if (pmid == 0) {
	  int n = FindEMRChannelKey(EMRChannelKey(iPlane, iPlane%2, barid, "emr"));
          _cfl_MA[n] = length;
        }
        if (pmid == 1) {
	  int n = FindEMRChannelKey(EMRChannelKey(iPlane, iPlane%2, barid, "emr"));
          _cfl_SA[n] = length;
	}
      }
    }

    fclose(file);
  } catch (MAUS::Exception e) {
    Squeak::mout(Squeak::error)
    << "Error in EMRAttenuationMap::LoadCfLength : Error during loading. " << std::endl
    << e.GetMessage() << std::endl;
    return false;
  }

  return true;
}

int EMRAttenuationMap::FindEMRChannelKey(EMRChannelKey key) const {
  for (unsigned int i = 0; i < _Ckey.size(); ++i )
    if (_Ckey.at(i) == key)
      return i;

  return NOCALIB;
}

double EMRAttenuationMap::connectorAtten(EMRChannelKey key, const char *pmt) const {

  int n = FindEMRChannelKey(key);

  double attenuation = -999.0;

  if (n != NOCALIB) {
    if (strcmp(pmt, "MA") == 0) attenuation = _caf_MA[n] + 1;
    else if (strcmp(pmt, "SA") == 0)
      attenuation = _caf_SA[n] + 1;
    else
      Squeak::mout(Squeak::error) << "Wrong PMT ID" << std::endl;

    if ( attenuation ) return attenuation;
  }

  return NOCALIB;
}

double EMRAttenuationMap::cfLength(EMRChannelKey key, const char *pmt) const {

  int n = FindEMRChannelKey(key);

  double length = -1.0;

  if (n != NOCALIB) {
    if (strcmp(pmt, "MA") == 0) length = _cfl_MA[n];
    else if (strcmp(pmt, "SA") == 0)
      length = _cfl_SA[n];
    else
      Squeak::mout(Squeak::error) << "Wrong PMT ID" << std::endl;

    if ( length ) return length;
  }

  return NOCALIB;
}

double EMRAttenuationMap::fibreAtten(EMRChannelKey key, double x, double y, const char *pmt) const {

  /*------------ WLS fiber ---------*/
  double WLSf_length;

  if (key.plane()%2 == 0) { // X plane - light travels along Y direction
    if (strcmp(pmt, "MA") == 0) {WLSf_length = _bar_length/2./100.0 - y/1000.0; // MAPMT
    } else {WLSf_length = _bar_length/2./100.0 + y/1000.0;} // SAPMT
  } else { // Y plane - light travels along X direction
    if (strcmp(pmt, "MA") == 0) {WLSf_length = _bar_length/2./100.0 + x/1000.0; // MAPMT
    } else {WLSf_length = _bar_length/2./100.0 - x/1000.0;} // SAPMT
  }

  double atten_wlsf = pow(10, -_atten_WLSf*WLSf_length/10.0);

  /*---------- Clear fiber ---------*/
  double clearf_length = cfLength(key, pmt)/1000.0;
  double atten_clrf = pow(10, -_atten_CLRf*clearf_length/10.0);

  return atten_wlsf*atten_clrf;
}

int EMRAttenuationMap::fibreDelay(EMRChannelKey key, double x, double y, const char *pmt) const {

  int c = 299792458; // m/s
  double n = 1.59; // Polystyrene refractive index

  /*------------ WLS fiber ---------*/
  double WLSf_length;

  if (key.plane()%2 == 0) { // X plane - light travels along Y direction
    if (strcmp(pmt, "MA") == 0) {WLSf_length = _bar_length/2./100.0 - y/1000.0; // MAPMT
    } else {WLSf_length = _bar_length/2./100.0 + y/1000.0;} // SAPMT
  } else { // Y plane - light travels along X direction
    if (strcmp(pmt, "MA") == 0) {WLSf_length = _bar_length/2./100.0 + x/1000.0; // MAPMT
    } else {WLSf_length = _bar_length/2./100.0 - x/1000.0;} // SAPMT
  }
  double delay_wlsf = WLSf_length*n*1000000000/c; // ns

  /*---------- Clear fiber ---------*/
  double clearf_length = cfLength(key, pmt)/1000.0;
  double delay_clrf = clearf_length*n*1000000000/c; // ns

  return static_cast<int>(delay_wlsf+delay_clrf);
}

void EMRAttenuationMap::Print() {
  std::cout << "====================== EMRAttenuationMap =========================" << std::endl;
  std::cout << " Number of channels : " << _Ckey.size() << std::endl;

  for (unsigned int i = 0; i < _Ckey.size(); i++) {
    std::cout << _Ckey[i] << " CAF (MA): " << _caf_MA[i] << " CAF (SA): " << _caf_SA[i];
    std::cout << " CFL (MA): " << _cfl_MA[i] << " CFL (SA): " << _cfl_SA[i] << std::endl;
  }

  std::cout<< "===================================================================" << std::endl;
}
}
