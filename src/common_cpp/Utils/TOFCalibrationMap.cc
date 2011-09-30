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

#include "Utils/TOFCalibrationMap.hh"

TOFCalibrationMap::~TOFCalibrationMap() {
  _Pkey.clear();
  _Tkey.clear();
  _twPar.resize(0);
  _t0.resize(0);
  _reff.resize(0);
}

bool TOFCalibrationMap::InitializeFromCards(std::string json_configuration) {
  //  JsonCpp setup
  Json::Value configJSON = JsonWrapper::StringToJson(json_configuration);
  // this will contain the configuration

  this->MakeTOFChannelKeys();

  Json::Value t0_file = JsonWrapper::GetProperty(configJSON,
                                                 "TOF_T0_calibration_file",
                                                 JsonWrapper::stringValue);

  Json::Value tw_file = JsonWrapper::GetProperty(configJSON,
                                                 "TOF_TW_calibration_file",
                                                 JsonWrapper::stringValue);

  Json::Value trigger_file = JsonWrapper::GetProperty(configJSON,
                                                 "TOF_Trigger_calibration_file",
                                                 JsonWrapper::stringValue);

  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
  if(!pMAUS_ROOT_DIR) {
    Squeak::mout(Squeak::error)
    << "Could not find the $MAUS_ROOT_DIR environmental variable." << std::endl;
    Squeak::mout(Squeak::error) << "Did you try running: source env.sh ?" << std::endl;
    return false;
  }
  
  std::string xMapT0File = std::string(pMAUS_ROOT_DIR) + t0_file.asString();
  std::string xMapTWFile = std::string(pMAUS_ROOT_DIR) + tw_file.asString();
  std::string xMapTriggerFile = std::string(pMAUS_ROOT_DIR) + trigger_file.asString();
  
  bool loaded = this->Initialize(xMapT0File, xMapTWFile, xMapTriggerFile);
  if (!loaded)
    return false;

  return true;
}

bool TOFCalibrationMap::Initialize(std::string t0File,
                                   std::string twFile,
                                   std::string triggerFile) {
  bool status = LoadT0File(t0File);
  if (status)
    status = LoadTWFile(twFile);
  if (status)
    status = LoadTriggerFile(triggerFile);

  return status;
}

int TOFCalibrationMap::MakeTOFChannelKeys()
{
 /** Makes one TOFChannelKey for each channel of the TOF detector.
  * The size of _t0, _reff and _twPar is set here.
  * ATTENTION : The detector configuration is HARDCODED !!!!
  * TO BE IMPROVED !!!!
  */
  int nStation = 3;
  int nPlanes = 2;
  int nSlabs[] = {10,7,10};
  
  for(int st=0;st<nStation;st++) {
    stringstream detector;
    detector << "tof" << st;
    for(int pl=0;pl<nPlanes;pl++)
      for(int sl=0;sl<nSlabs[st];sl++)
        for(int pmt=0;pmt<2;pmt++)
          _Pkey.push_back(TOFChannelKey(st, pl, sl, pmt, detector.str()));
  }
  
  int nChannels = _Pkey.size();
  _t0.resize( nChannels );
  _reff.resize( nChannels );
  _twPar.resize( nChannels );

  return nChannels;
}

bool TOFCalibrationMap::LoadT0File(std::string t0File) {
  std::ifstream stream(t0File.c_str());
  if (!stream) {
    Squeak::mout(Squeak::error)
    << "Error in TOFCalibrationMap::LoadT0File. Can't open TOF calibration file."
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
  } catch(Squeal e) {
    Squeak::mout(Squeak::error)
    << "Error in TOFCalibrationMap::LoadT0File. Error during loading. " << std::endl
    << e.GetMessage() << std::endl;
    return false;
  }

  return true;
}

bool TOFCalibrationMap::LoadTWFile(std::string twFile) {
  std::ifstream stream(twFile.c_str());
  if (!stream) {
    Squeak::mout(Squeak::error)
    << "Error in TOFCalibrationMap::LoadTWFile. Can't open TOF calibration file. "
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
  } catch(Squeal e) {
    Squeak::mout(Squeak::error)
    << "Error in TOFCalibrationMap::LoadTWFile. Error during loading. " << std::endl
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
  } catch(Squeal e) {
    Squeak::mout(Squeak::error)
    << "Error in TOFCalibrationMap::LoadTriggerFile. Error during loading. " << std::endl
    << e.GetMessage() << std::endl;
    return false;
  }

  return true;
}

int TOFCalibrationMap::FindTOFChannelKey(TOFChannelKey key) {
  for ( unsigned int i=0; i<_Pkey.size(); ++i )
    if ( _Pkey.at(i) == key )
      return i;

  return NOCALIB;
}

int TOFCalibrationMap::FindTOFPixelKey(TOFPixelKey key) {
  for ( unsigned int i=0; i<_Tkey.size(); ++i )
    if  ( _Tkey.at(i) == key )
      return i;

  return NOCALIB;
}

double TOFCalibrationMap::T0(TOFChannelKey key ,int &r) {
  int n = FindTOFChannelKey( key );

  if ( n != NOCALIB) {
    r = _reff[n];
    if ( _t0[n] ) return _t0[n];
  }

  // std::cout << "TOFCalibrationMap -> No T0 calibration for " << key << std::endl;
  return NOCALIB;
}

double TOFCalibrationMap::TriggerT0(TOFPixelKey key) {
  int n = FindTOFPixelKey( key );
  if ( n != NOCALIB)
    return _Trt0[n];

  // std::cout << "TOFCalibrationMap -> No Trigger calibration for " << key << std::endl;
  return n;
}

double TOFCalibrationMap::TW( TOFChannelKey key, int adc ) {
  int n = FindTOFChannelKey( key );

  if ( n != NOCALIB) {
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

double TOFCalibrationMap::dT(TOFChannelKey Pkey, TOFPixelKey TrKey, int adc) {
  int reffSlab;
  double tw = TW(Pkey, adc);
  double t0 = T0(Pkey, reffSlab);
  double trt0 = TriggerT0(TrKey);
  // std::cout << "TOFCalibrationMap -> "<< Pkey << " " << TrKey << "  tw = " << tw;
  // std::cout << "  t0 = " << -t0 << "  trt0 = " << trt0 << std::endl;
  if (tw == NOCALIB || t0 == NOCALIB || trt0 == NOCALIB) {
    return NOCALIB;
  }

  double dt = tw - t0 + trt0;
   std::cout << "TOFCalibrationMap -> "<< Pkey << " "<< TrKey << "  tw = " << tw << "  t0 = ";
   std::cout << -t0 << "  trt0 = " << trt0 << " tot = " << dt << std::endl;

  if (Pkey.station() == TriggerStation) {
    if (Pkey.plane()==0) {
      TOFPixelKey refTr(TriggerStation, Pkey.slab(), reffSlab, Pkey.detector());
      if (TriggerT0(refTr) == NOCALIB)
        return NOCALIB;
      
      dt -= TriggerT0(refTr);
      // std::cout << refTr << "  dt = " << TriggerT0(refTr) << std::endl;
    }
    else {
      TOFPixelKey refTr(TriggerStation, reffSlab, Pkey.slab(), Pkey.detector());
      if (TriggerT0(refTr) == NOCALIB)
        return NOCALIB;
 
      dt -= TriggerT0(refTr);
       // std::cout << refTr << "  dt = " << TriggerT0(refTr) << std::endl;
    }
  }
  // std::cout << "TOFCalibrationMap -> dT = " << -dt << std::endl;
  return dt*1e-3;
}


TOFPixelKey::TOFPixelKey(string keyStr) throw(Squeal) {
  std::stringstream xConv;
  try {
    xConv << keyStr;
    xConv >> (*this);
  }catch(Squeal e) {
    throw(Squeal(Squeal::recoverable,
                 std::string("corrupted TOF Channel Key"),
                 "TOFPixelKey::TOFPixelKey(std::string)"));
  }
}

bool TOFPixelKey::operator==( TOFPixelKey const key ) {
  if ( _station == key._station &&
       _slabX == key._slabX &&
       _slabY == key._slabY &&
       _detector == key._detector) {
    return true;
  } else {
    return false;
  }
}

bool TOFPixelKey::operator!=( TOFPixelKey const key ) {
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

istream& operator>>( istream& stream, TOFPixelKey &key ) throw(Squeal) {
  string xLabel;
  stream >> xLabel >> key._station >> key._slabX >> key._slabY >> key._detector;

  if (xLabel != "TOFPixelKey") {
    throw(Squeal(Squeal::recoverable,
                 std::string("corrupted TOF Pixel Key"),
                 "istream& operator>>(istream& stream, TOFPixelKey)"));
  }

  return stream;
}

string TOFPixelKey::str() {
  stringstream xConv;
  xConv << (*this);
  return xConv.str();
}




