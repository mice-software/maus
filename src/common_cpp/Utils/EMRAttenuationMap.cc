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

  _CkeyCA.clear();
  _CkeyCFL.clear();
  _caf_MA.resize(0);
  _caf_SA.resize(0);
  _cfl_MA.resize(0);
  _cfl_SA.resize(0);
}

bool EMRAttenuationMap::InitializeFromCards(Json::Value configJSON) {

  // Fetch variables
  _number_of_planes = configJSON["EMRnumberOfPlanes"].asInt();
  _number_of_bars = configJSON["EMRnumberOfBars"].asInt();

  _bar_length = configJSON["EMRbarLength"].asDouble();
  _atten_WLSf = configJSON["EMRattenWLSf"].asDouble();
  _atten_CLRf = configJSON["EMRattenCLRf"].asDouble();

  // Fill the vector containing all EMR channel keys.
  this->MakeEMRChannelKeys();

  // Load calibration file, return false if it fails
  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
  if ( !pMAUS_ROOT_DIR ) {
    Squeak::mout(Squeak::error)
    << "Could not find the $MAUS_ROOT_DIR environmental variable." << std::endl;
    Squeak::mout(Squeak::error) << "Did you try running: source env.sh ?" << std::endl;
    return false;
  }

  std::string connectorAttenFile = std::string(pMAUS_ROOT_DIR)
			   	   + configJSON["EMR_connector_attenuation_map"].asString();
  std::string cfLengthFile = std::string(pMAUS_ROOT_DIR)
			     + configJSON["EMR_clear_fiber_length_map"].asString();

  return this->Initialize(connectorAttenFile, cfLengthFile);
}

bool EMRAttenuationMap::Initialize(std::string connectorAttenFile, std::string cfLengthFile) {

  return LoadConnectorAttenFile(connectorAttenFile) &&
         LoadCfLengthFile(cfLengthFile);
}

int EMRAttenuationMap::MakeEMRChannelKeys() {

  for (int iPlane = 0; iPlane < _number_of_planes; iPlane++)
    for (int iBar = 1; iBar < _number_of_bars; iBar++)
      _CkeyCA.push_back(EMRChannelKey(iPlane, iPlane%2, iBar, "emr"));

  for (int iBar = 1; iBar < _number_of_bars; iBar++)
    _CkeyCFL.push_back(EMRChannelKey(-1, 0, iBar, "emr"));

  int nChannels = _CkeyCA.size();
  _caf_MA.resize(nChannels);
  _caf_SA.resize(nChannels);

  nChannels = _CkeyCFL.size();
  _cfl_MA.resize(nChannels);
  _cfl_SA.resize(nChannels);

  return nChannels;
}

bool EMRAttenuationMap::LoadConnectorAttenFile(std::string connectorAttenFile) {

  // Check the calibration file
  std::ifstream file(connectorAttenFile.c_str());
  if ( !file ) {
    throw(Exception(Exception::recoverable,
          "Could not find EMR connector attenuation map",
          "EMRAttenuationMap::LoadConnectorAtten"));
  }

  // Fill the arrays of correction factors
  try {
    while ( !file.eof() ) {
      char pmt[10], fom[10];
      int plane(-9), bar(-9);
      double att(-1.0);
      file >> pmt >> fom >> plane >> bar >> att;

      int n = FindEMRChannelKey(EMRChannelKey(plane, plane%2, bar, "emr"));
      if ( strcmp(fom, "mean") == 0 && n != NOATTEN ) {
        if (strcmp(pmt, "MA") == 0) {
	  _caf_MA[n] = att;
        } else if (strcmp(pmt, "SA") == 0) {
	  _caf_SA[n] = att;
        }
      }
    }
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
  ifstream file(cfLengthFile.c_str());
  if ( !file ) {
    throw(Exception(Exception::recoverable,
          "Could not find EMR clear fibre length",
          "EMRAttenuationMap::LoadCfLength"));
  }

  // Fill the arrays of correction factors
  try {
    while ( !file.eof() ) {
      char pmt[10];
      int plane(-9), bar(-9);
      double len(-1.0);
      file >> pmt  >> plane >> bar >> len;

      int n = FindEMRBarKey(EMRChannelKey(plane, plane%2, bar, "emr"));
      if ( n != NOATTEN ) {
        if (strcmp(pmt, "MA") == 0) {
	  _cfl_MA[n] = len;
        } else if (strcmp(pmt, "SA") == 0) {
	  _cfl_SA[n] = len;
        }
      }
    }
  } catch (MAUS::Exception e) {
    Squeak::mout(Squeak::error)
    << "Error in EMRAttenuationMap::LoadCfLength : Error during loading. " << std::endl
    << e.GetMessage() << std::endl;
    return false;
  }

  return true;
}

int EMRAttenuationMap::FindEMRChannelKey(EMRChannelKey key) const {

  int xPlane = key.GetPlane();
  int xBar = key.GetBar();
  if ( xPlane > -1 && xPlane < _number_of_planes &&
       xBar > 0 && xBar < _number_of_bars)
      return xPlane*(_number_of_bars-1)+(xBar-1);

  return NOATTEN;
}

int EMRAttenuationMap::FindEMRBarKey(EMRChannelKey key) const {

  int xBar = key.GetBar();
  if ( xBar > 0 && xBar < _number_of_bars )
      return xBar - 1;

  return NOATTEN;
}

double EMRAttenuationMap::connectorAtten(EMRChannelKey key, const char *pmt) const {

  int n = FindEMRChannelKey(key);
  double att(-1.);

  if ( n != NOATTEN ) {
    if ( strcmp(pmt, "MA") == 0 ) {
      att = _caf_MA[n];
    } else if ( strcmp(pmt, "SA") == 0 ) {
      att = _caf_SA[n];
    } else {
      Squeak::mout(Squeak::error)
	<< "Wrong PMT ID in EMRAttenuationMap::connectorAtten" << std::endl;
    }
    if ( att > 0 )
	return att;
  }

  return NOATTEN;
}

double EMRAttenuationMap::cfLength(EMRChannelKey key, const char *pmt) const {

  int n = FindEMRBarKey(key);
  double len(-1.);

  if ( n != NOATTEN ) {
    if ( strcmp(pmt, "MA") == 0 ) {
      len = _cfl_MA[n];
    } else if (strcmp(pmt, "SA") == 0) {
      len = _cfl_SA[n];
    } else {
      Squeak::mout(Squeak::error)
	<< "Wrong PMT ID in EMRAttenuationMap::cfLength" << std::endl;
    }
    if ( len > 0 )
	return len;
  }

  return NOATTEN;
}

double EMRAttenuationMap::fibreAtten(EMRChannelKey key, double x, double y, const char *pmt) const {

  /*------------ WLS fiber ---------*/
  double WLSf_length(0.); // [m]
  if ( !key.GetOrientation() ) { // X plane - light travels along Y direction
    if ( strcmp(pmt, "MA") == 0 ) {
      WLSf_length = (_bar_length/2 - y)/pow(10, 3);
    } else {
      WLSf_length = (_bar_length/2 + y)/pow(10, 3);
    }
  } else { // Y plane - light travels along X direction
    if ( strcmp(pmt, "MA") == 0 ) {
      WLSf_length = (_bar_length/2 + x)/pow(10, 3);
    } else {
      WLSf_length = (_bar_length/2 - x)/pow(10, 3);
    }
  }
  double atten_wlsf = pow(10, -_atten_WLSf*WLSf_length/10.);

  /*---------- Clear fiber ---------*/
  double clearf_length = cfLength(key, pmt)/pow(10, 3); // [m]
  double atten_clrf = pow(10, -_atten_CLRf*clearf_length/10);

  return atten_wlsf*atten_clrf;
}

double EMRAttenuationMap::fibreDelay(EMRChannelKey key, double x, double y, const char *pmt) const {

  int c = 299792458; 	// Speed of light in vacuum [m/s]
  double n = 1.59; 	// Polystyrene refractive index

  /*------------ WLS fiber ---------*/
  double WLSf_length(0.); // [m]
  if ( !key.GetOrientation() ) { // X plane - light travels along Y direction
    if ( strcmp(pmt, "MA") == 0 ) {
      WLSf_length = (_bar_length/2 - y)/pow(10, 3);
    } else {
      WLSf_length = (_bar_length/2 + y)/pow(10, 3);
    }
  } else { // Y plane - light travels along X direction
    if ( strcmp(pmt, "MA") == 0 ) {
      WLSf_length = (_bar_length/2 + x)/pow(10, 3);
    } else {
      WLSf_length = (_bar_length/2 - x)/pow(10, 3);
    }
  }
  double delay_wlsf = WLSf_length*n*pow(10, 9)/c; 	// [ns]

  /*---------- Clear fiber ---------*/
  double clearf_length = cfLength(key, pmt)/pow(10, 3);	// [m]
  double delay_clrf = clearf_length*n*pow(10, 9)/c; 	// [ns]

  return delay_wlsf+delay_clrf;
}

void EMRAttenuationMap::Print() {

  std::cerr << "====================== EMRAttenuationMap =========================" << std::endl;
  std::cerr << "Connector attenuation map" << std::endl;
  std::cerr << "Number of channels : " << _CkeyCA.size() << std::endl;

  for (unsigned int i = 0; i < _CkeyCA.size(); i++)
    std::cerr << _CkeyCA[i] << " CAF (MA): " << _caf_MA[i]
	      << ", CAF (SA): " << _caf_SA[i] << std::endl;

  std::cerr << "\nClear fibre length map" << std::endl;
  std::cerr << "Number of fibres : " << _CkeyCFL.size() << std::endl;
  for (unsigned int i = 0; i < _CkeyCFL.size(); i++)
    std::cerr << _CkeyCFL[i] << " CFL (MA): " << _cfl_MA[i]
	      << ", CFL (SA): " << _cfl_SA[i] << std::endl;
  std::cerr<< "===================================================================" << std::endl;
}
} // namespace MAUS
