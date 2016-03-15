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
}

bool EMRGeometryMap::InitializeFromCards(Json::Value configJSON) {

  // Fetch default variables
  _number_of_planes = configJSON["EMRnumberOfPlanes"].asInt();
  _number_of_bars = configJSON["EMRnumberOfBars"].asInt();

  _bar_width = configJSON["EMRbarWidth"].asDouble();
  _bar_height = configJSON["EMRbarHeight"].asDouble();
  _bar_length = configJSON["EMRbarLength"].asDouble();
  _gap = configJSON["EMRgap"].asDouble();

  _globalPosEMR = Hep3Vector(0., 0., 0.);
  _globalRotEMR = HepRotation(0., 0., 0.);

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

bool EMRGeometryMap::LoadGeometryFile(std::string geometryFile) {

  MiceModule* geo_module = new MiceModule(geometryFile);
  std::vector<const MiceModule*> emr_modules =
      geo_module->findModulesByPropertyString("SensitiveDetector", "EMR");

  if ( emr_modules.size() ) {

    // There is only one parent module for the EMR
    // Set the global position and rotation of the EMR mother volume
    _globalPosEMR = emr_modules[0]->globalPosition();
    _globalRotEMR = (emr_modules[0]->globalRotation()).inverse();

    // Set the dimensions of EMR bars and the size of the gaps
    _bar_width = emr_modules[0]->propertyDouble("BarWidth"); // mm
    _bar_height = emr_modules[0]->propertyDouble("BarHeight"); // mm
    _bar_length = emr_modules[0]->propertyDouble("BarLength"); // mm
    _D = ThreeVector(_bar_length, _bar_width, _bar_height);

    _gap = emr_modules[0]->propertyDouble("Gap"); // mm

    // Set the limits of the fiducial volume of the EMR
    _localStartEMR = ThreeVector(-(_number_of_bars/2)*(.5*_bar_width+_gap),
		     		 -(_number_of_bars/2)*(.5*_bar_width+_gap),
		     		 -(_number_of_planes/2)*(_bar_height+_gap));
    _localEndEMR = -_localStartEMR;
  } else {

    // Warn that there will not be any global positions
    Squeak::mout(Squeak::warning)
    << "WARNING in EMRGeometryMap::LoadGeometryFile. The EMR is not included"
    << " as a sensitive detector !!! Global positions will not be reconstructed."
    << std::endl;

    // Set the dimensions of EMR bars and the size of the gaps
    _D = ThreeVector(_bar_length, _bar_width, _bar_height);

    // Set the limits of the fiducial volume of the EMR
    _localStartEMR = ThreeVector(-(_number_of_bars/2)*(.5*_bar_width+_gap),
			   	 -(_number_of_bars/2)*(.5*_bar_width+_gap),
			   	 -(_number_of_planes/2)*(_bar_height+_gap));
    _localEndEMR = -_localStartEMR;
  }

  return true;
}

ThreeVector EMRGeometryMap::LocalPosition(EMRChannelKey key) const {

  int xPlane = key.GetPlane();
  int xBar = key.GetBar();

  double x = ((xPlane+1)%2)*(_localStartEMR.x()+xBar*(.5*_bar_width+_gap));
  double y = (xPlane%2)*(_localStartEMR.y()+xBar*(.5*_bar_width+_gap));
  double z = _localStartEMR.z()+(xPlane+(1.+xBar%2)/3)*(_bar_height+_gap);

  return ThreeVector(x, y, z);
}

ThreeVector EMRGeometryMap::GlobalPosition(EMRChannelKey key) const {

  ThreeVector lPos = LocalPosition(key);

  return MakeGlobal(lPos);
}

ThreeVector EMRGeometryMap::MakeGlobal(ThreeVector pos) const {

  Hep3Vector hepPos(pos.x(), pos.y(), pos.z());
  hepPos.transform(_globalRotEMR);
  hepPos += _globalPosEMR;

  return ThreeVector(hepPos.x(), hepPos.y(), hepPos.z());
}
} // namespace MAUS
