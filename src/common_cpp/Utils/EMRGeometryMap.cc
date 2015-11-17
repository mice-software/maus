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

#include "Utils/EMRGeometryMap.hh"
#include "Utils/EMRChannelMap.hh"

namespace MAUS {

EMRGeometryMap::EMRGeometryMap() {
}

EMRGeometryMap::~EMRGeometryMap() {
  _Ckey.clear();
  _Plkey.clear();
  _Pgkey.clear();
}

bool EMRGeometryMap::InitializeFromCards(Json::Value configJSON) {
  // Fetch variables
  _number_of_planes = configJSON["EMRnumberOfPlanes"].asInt();
  _number_of_bars = configJSON["EMRnumberOfBars"].asInt();

  _bar_width = configJSON["EMRbarWidth"].asDouble();
  _bar_height = configJSON["EMRbarHeight"].asDouble();
  _bar_length = configJSON["EMRbarLength"].asDouble()*10;
  _gap = configJSON["EMRGap"].asDouble();

  // Fill the vector containing all EMR channel keys.
  this->MakeEMRChannelKeys();

  // Find geometry file
  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
  if (!pMAUS_ROOT_DIR) {
    Squeak::mout(Squeak::error)
    << "Could not find the $MAUS_ROOT_DIR environmental variable." << std::endl;
    Squeak::mout(Squeak::error) << "Did you try running: source env.sh ?" << std::endl;
    return false;
  }

  std::string geometryFile = configJSON["reconstruction_geometry_filename"].asString();

  // Load geometry file
  bool loaded;
  loaded = this->Initialize(geometryFile);

  if (!loaded)
    return false;

  return true;
}

bool EMRGeometryMap::Initialize(std::string geomtetryFile) {

  bool status = LoadGeometryFile(geomtetryFile);
  return status;
}

int EMRGeometryMap::MakeEMRChannelKeys() {
  for (int iPlane = 0; iPlane < _number_of_planes; iPlane++)
    for (int iBar = 1; iBar < _number_of_bars; iBar++)
      _Ckey.push_back(EMRChannelKey(iPlane, iPlane%2, iBar, "emr"));

  int nChannels = _Ckey.size();
  _Plkey.resize(nChannels);
  _Pgkey.resize(nChannels);

  return nChannels;
}

bool EMRGeometryMap::LoadGeometryFile(std::string geometryFile) {

  MiceModule* geo_module = new MiceModule(geometryFile);
  std::vector<const MiceModule*> emr_modules =
      geo_module->findModulesByPropertyString("SensitiveDetector", "EMR");

  if ( emr_modules.size() ) {

    // There is only one parent module for the EMR
    Hep3Vector globalPosEmr = emr_modules[0]->globalPosition();
    HepRotation globalRotEmr = emr_modules[0]->globalRotation();
    double bar_width = emr_modules[0]->propertyDouble("BarWidth"); // mm
    double bar_height = emr_modules[0]->propertyDouble("BarHeight"); // mm
    double bar_length = emr_modules[0]->propertyDouble("BarLength"); // mm
    double gap = emr_modules[0]->propertyDouble("Gap"); // mm

    Hep3Vector localStart(-(_number_of_bars/2)*(.5*bar_width+gap),
			  -(_number_of_bars/2)*(.5*bar_width+gap),
			  -(_number_of_planes/2-.5)*(bar_height+gap));

    for (int iPlane = 0; iPlane < _number_of_planes; iPlane++) {
      for (int iBar = 1; iBar < _number_of_bars; iBar++) {
        double x = ((iPlane+1)%2)*(localStart.x()+iBar*(.5*bar_width+gap));
        double y = (iPlane%2)*(localStart.y()+iBar*(.5*bar_width+gap));
        double z = localStart.z()+iPlane*(bar_height+gap);

        Hep3Vector localPos(x, y, z);
        Hep3Vector globalPos(globalPosEmr.x()+x,
			     globalPosEmr.y()+y,
			     globalPosEmr.z()+z);
        globalPos.transform(globalRotEmr);

        _Plkey.push_back(localPos);
        _Pgkey.push_back(globalPos);
      }

      Hep3Vector dimensions(bar_width, bar_height, bar_length);
      _D = dimensions;
    }
  } else {

    // Warn that there will not be any global positions
    Squeak::mout(Squeak::warning)
    << "WARNING in EMRGeometryMap::LoadGeometryFile. The EMR is not included"
    << "as a sensitive detector !!! Global positions will not be reconstructed."
    << std::endl;

    // Fill using the configuration defaults
    Hep3Vector localStart(-(_number_of_bars/2)*(.5*_bar_width+_gap),
			  -(_number_of_bars/2)*(.5*_bar_width+_gap),
			  -(_number_of_planes/2-.5)*(_bar_height+_gap));

    for (int iPlane = 0; iPlane < _number_of_planes; iPlane++) {
      for (int iBar = 1; iBar < _number_of_bars; iBar++) {
        double x = ((iPlane+1)%2)*(localStart.x()+iBar*(.5*_bar_width+_gap));
        double y = (iPlane%2)*(localStart.y()+iBar*(.5*_bar_width+_gap));
        double z = localStart.z()+iPlane*(_bar_height+_gap);

        Hep3Vector localPos(x, y, z);
        Hep3Vector globalPos(NOGEO, NOGEO, NOGEO);

        _Plkey.push_back(localPos);
        _Pgkey.push_back(globalPos);
      }

      Hep3Vector dimensions(_bar_width, _bar_height, _bar_length);
      _D = dimensions;
    }
  }

  return true;
}

int EMRGeometryMap::FindEMRChannelKey(EMRChannelKey key) const {
  for (unsigned int i = 0; i < _Ckey.size(); ++i )
    if (_Ckey.at(i) == key)
      return i;

  return NOGEO;
}

Hep3Vector EMRGeometryMap::LocalPosition(EMRChannelKey key) const {

  int n = FindEMRChannelKey(key);

  Hep3Vector pos;
  if (n != NOGEO) {
    pos = _Plkey[n];
    return pos;
  }
  return Hep3Vector(NOGEO, NOGEO, NOGEO);
}

Hep3Vector EMRGeometryMap::GlobalPosition(EMRChannelKey key) const {

  int n = FindEMRChannelKey(key);

  Hep3Vector pos;
  if (n != NOGEO) {
    pos = _Pgkey[n];
    return pos;
  }
  return Hep3Vector(NOGEO, NOGEO, NOGEO);
}

Hep3Vector EMRGeometryMap::Dimensions() const {

  return _D;
}

void EMRGeometryMap::Print() {
  std::cout << "====================== EMRGeometryMap =========================" << std::endl;
  std::cout << " Number of channels : " << _Ckey.size() << std::endl;

  for (unsigned int i = 0; i < _Ckey.size(); i++) {
    std::cout << "Channel: " << _Ckey[i] << std::endl
	      << "Local Pos.: " << _Plkey[i].x() << "  " << _Plkey[i].y()
	      << "  " << _Plkey[i].z() << std::endl
	      << "Global Pos.: " << _Pgkey[i].x() << "  " << _Pgkey[i].y()
	      << "  " << _Pgkey[i].z() << std::endl
	      << "Dimensions: " << _D.x() << "  " << _D.y()
	      << "  " << _D.z() << std::endl;
  }

  std::cout<< "===================================================================" << std::endl;
}
}
