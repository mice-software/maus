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
  _bar_length = configJSON["EMRbarLength"].asDouble();
  _gap = configJSON["EMRgap"].asDouble();

  _globalPosEMR = Hep3Vector(0., 0., 0.);
  _globalRotEMR = HepRotation(0., 0., 0.);

  // Fill the vector containing all EMR channel keys.
  this->MakeEMRChannelKeys();

  // Find and load geometry file, return false if it fails
  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
  if ( !pMAUS_ROOT_DIR ) {
    Squeak::mout(Squeak::error)
    << "Could not find the $MAUS_ROOT_DIR environmental variable." << std::endl;
    Squeak::mout(Squeak::error) << "Did you try running: source env.sh ?" << std::endl;
    return false;
  }
  std::string geometryFile = configJSON["reconstruction_geometry_filename"].asString();

  return this->Initialize(geometryFile);
}

bool EMRGeometryMap::Initialize(std::string geomtetryFile) {

  return this->LoadGeometryFile(geomtetryFile);
}

int EMRGeometryMap::MakeEMRChannelKeys() {

  for (int iPlane = 0; iPlane < _number_of_planes; iPlane++)
    for (int iBar = 1; iBar < _number_of_bars; iBar++)
      _Ckey.push_back(EMRChannelKey(iPlane, iPlane%2, iBar, "emr"));

  return _Ckey.size();
}

bool EMRGeometryMap::LoadGeometryFile(std::string geometryFile) {

  MiceModule* geo_module = new MiceModule(geometryFile);
  std::vector<const MiceModule*> emr_modules =
      geo_module->findModulesByPropertyString("SensitiveDetector", "EMR");

  if ( emr_modules.size() ) {

    // There is only one parent module for the EMR
    _globalPosEMR = emr_modules[0]->globalPosition();
    _globalRotEMR = emr_modules[0]->globalRotation();

    double bar_width = emr_modules[0]->propertyDouble("BarWidth"); // mm
    double bar_height = emr_modules[0]->propertyDouble("BarHeight"); // mm
    double bar_length = emr_modules[0]->propertyDouble("BarLength"); // mm
    _gap = emr_modules[0]->propertyDouble("Gap"); // mm

    _localStartEMR = ThreeVector(-(_number_of_bars/2)*(.5*bar_width+_gap),
		     		 -(_number_of_bars/2)*(.5*bar_width+_gap),
		     		 -(_number_of_planes/2)*(bar_height+_gap));
    _localEndEMR = -_localStartEMR;

    // Set the positions of the bar in local and global coordinates
    for (int iPlane = 0; iPlane < _number_of_planes; iPlane++) {
      for (int iBar = 1; iBar < _number_of_bars; iBar++) {
        double x = ((iPlane+1)%2)*(_localStartEMR.x()+iBar*(.5*bar_width+_gap));
        double y = (iPlane%2)*(_localStartEMR.y()+iBar*(.5*bar_width+_gap));
        double z = _localStartEMR.z()+(iPlane+(1.+iBar%2)/3)*(bar_height+_gap);

        ThreeVector localPos(x, y, z);
        _Plkey.push_back(localPos);

        Hep3Vector gPos(x, y, z);
        gPos.transform(_globalRotEMR);
	gPos += _globalPosEMR;
        ThreeVector globalPos(gPos.x(), gPos.y(), gPos.z());
        _Pgkey.push_back(globalPos);
      }

      ThreeVector dimensions(bar_length, bar_width, bar_height);
      _D = dimensions;
    }
  } else {

    // Warn that there will not be any global positions
    Squeak::mout(Squeak::warning)
    << "WARNING in EMRGeometryMap::LoadGeometryFile. The EMR is not included"
    << "as a sensitive detector !!! Global positions will not be reconstructed."
    << std::endl;

    // Fill using the configuration defaults
    _localStartEMR = ThreeVector(-(_number_of_bars/2)*(.5*_bar_width+_gap),
			   	 -(_number_of_bars/2)*(.5*_bar_width+_gap),
			   	 -(_number_of_planes/2)*(_bar_height+_gap));
    _localEndEMR = -_localStartEMR;

    for (int iPlane = 0; iPlane < _number_of_planes; iPlane++) {
      for (int iBar = 1; iBar < _number_of_bars; iBar++) {
        double x = ((iPlane+1)%2)*(_localStartEMR.x()+iBar*(.5*_bar_width+_gap));
        double y = (iPlane%2)*(_localStartEMR.y()+iBar*(.5*_bar_width+_gap));
        double z = _localStartEMR.z()+(iPlane+(1.+iBar%2)/3)*(_bar_height+_gap);

        ThreeVector localPos(x, y, z);
        ThreeVector globalPos(NOGEO, NOGEO, NOGEO);

        _Plkey.push_back(localPos);
        _Pgkey.push_back(globalPos);
      }

      ThreeVector dimensions(_bar_length, _bar_width, _bar_height);
      _D = dimensions;
    }
  }

  return true;
}

int EMRGeometryMap::FindEMRChannelKey(EMRChannelKey key) const {
  for (unsigned int i = 0; i < _Ckey.size(); ++i )
    if ( _Ckey.at(i) == key )
      return i;

  return NOGEO;
}

ThreeVector EMRGeometryMap::LocalPosition(EMRChannelKey key) const {

  int n = FindEMRChannelKey(key);
  ThreeVector pos;
  if ( n != NOGEO )
    return _Plkey[n];

  return ThreeVector(NOGEO, NOGEO, NOGEO);
}

ThreeVector EMRGeometryMap::GlobalPosition(EMRChannelKey key) const {

  int n = FindEMRChannelKey(key);

  ThreeVector pos;
  if (n != NOGEO)
    return _Pgkey[n];

  return ThreeVector(NOGEO, NOGEO, NOGEO);
}

ThreeVector EMRGeometryMap::MakeGlobal(ThreeVector pos) const {

  Hep3Vector hepPos(pos.x(), pos.y(), pos.z());
  hepPos.transform(_globalRotEMR);
  hepPos += _globalPosEMR;

  return ThreeVector(hepPos.x(), hepPos.y(), hepPos.z());
}

void EMRGeometryMap::Print() {
  std::cerr << "====================== EMRGeometryMap =========================" << std::endl;
  std::cerr << " Number of channels : " << _Ckey.size() << std::endl;

  for (unsigned int i = 0; i < _Ckey.size(); i++) {
    std::cerr << "Channel: " << _Ckey[i] << std::endl
	      << "Local Pos.: " << _Plkey[i].x() << "  " << _Plkey[i].y()
	      << "  " << _Plkey[i].z() << std::endl
	      << "Global Pos.: " << _Pgkey[i].x() << "  " << _Pgkey[i].y()
	      << "  " << _Pgkey[i].z() << std::endl
	      << "Dimensions: " << _D.x() << "  " << _D.y()
	      << "  " << _D.z() << std::endl;
  }

  std::cerr << "===================================================================" << std::endl;
}
} // namespace MAUS
