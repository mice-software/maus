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


#include "src/common_cpp/Simulation/GeometryNavigator.hh"

#include <string>

#include "Geant4/G4RunManager.hh"
#include "CLHEP/Units/SystemOfUnits.h"
#include "src/common_cpp/Utils/Exception.hh"

namespace MAUS {

  GeometryNavigator::GeometryNavigator() :
    _global_volume(NULL),
    _navigator(NULL),
    _touchable_history(NULL),
    _current_position(),
    _current_volume(NULL),
    _current_material(NULL) {
    }


  GeometryNavigator::~GeometryNavigator() {
    if (_touchable_history) {
      delete _touchable_history;
      _touchable_history = NULL;
      _current_volume = NULL;
      _current_material = NULL;
    }
    if (_navigator) {
      delete _navigator;
      _navigator = NULL;
    }
    _global_volume = NULL; // Not ours so don't delete
  }


  void GeometryNavigator::Initialise(G4VPhysicalVolume* phys_vol) {
    if (!phys_vol) {
      throw(Exceptions::Exception(Exceptions::recoverable,
                 std::string("No physical volume could be found "),
                 "GeometryNavigator::GeometryNavigator()"));
    }
    _global_volume = phys_vol;
    if (_navigator != NULL) {
      delete _navigator;
      _navigator = NULL;
    }
    _navigator = new G4Navigator();
    _navigator->SetWorldVolume(this->_global_volume);
    // Needed so Geant4 doesn't segfault when setting up a point
    G4RunManager::GetRunManager()->BeamOn(0);
    this->_setPoint(G4ThreeVector(0.0, 0.0, 0.0));
    // Ack! G4 does some setup at BeamOn time
    G4RunManager::GetRunManager()->BeamOn(0);
  }


  void GeometryNavigator::SetPoint(ThreeVector point) {
    G4ThreeVector pos = ToG4Vec(point);
    this->_setPoint(pos);
  }

  double GeometryNavigator::ComputeStep(ThreeVector point,
                                        ThreeVector momentum,
                                        double step) {
    G4ThreeVector pos = ToG4Vec(point);
    ThreeVector direction = momentum/momentum.mag();
    G4ThreeVector dir = ToG4Vec(direction);
    _setPoint(pos);
    double new_step = 0.;
    _current_position = pos;
    //_navigator->ResetHierarchyAndLocate(pos,
    //                                    dir,
    //                                    *_touchable_history);
    _navigator->LocateGlobalPointAndSetup(pos, &dir, false, false);
    //std::cerr << "ComputeStep pos: " << pos << " dir: " << dir << " step: " << step << std::endl;
    _navigator->ComputeStep(pos, dir, step, new_step);
    //std::cerr << "            new_step: " << new_step << std::endl;
    if (_touchable_history) {
      delete _touchable_history;
      _touchable_history = NULL;
    }
    _touchable_history = _navigator->CreateTouchableHistory();
    _current_volume = _touchable_history->GetVolume();
    _current_material = _current_volume->GetLogicalVolume()->GetMaterial();

    return new_step;
  }

  ThreeVector GeometryNavigator::Step(ThreeVector displacement) {
    G4ThreeVector disp = ToG4Vec(displacement);
    return ToMAUSVec(this->_step(disp));
  }


  void GeometryNavigator::_setPoint(G4ThreeVector pos) {
    if (!_navigator) {
      throw(Exceptions::Exception(Exceptions::recoverable,
                 std::string("Navigator not correctly set up. ")+
                 std::string("Physical volume required"),
                 "GeometryNavigator::_setPoint(G4ThreeVector)"));
    }
    _current_position = pos;
    _navigator->LocateGlobalPointAndSetup(_current_position);
    if (_touchable_history) {
      delete _touchable_history;
      _touchable_history = NULL;
    }
    _touchable_history = _navigator->CreateTouchableHistory();
    _current_volume = _touchable_history->GetVolume();
    _current_material = _current_volume->GetLogicalVolume()->GetMaterial();
  }

  G4ThreeVector GeometryNavigator::_step(G4ThreeVector dir) {
    _current_position = _current_position + dir;
    _navigator->LocateGlobalPointAndUpdateTouchable(
                                   _current_position, dir, _touchable_history);
    _current_volume = _touchable_history->GetVolume();
    _current_material = _current_volume->GetLogicalVolume()->GetMaterial();
    return _current_position;
  }

  std::string GeometryNavigator::GetMaterialName() const {
    return _current_material->GetName();
  }


  bool GeometryNavigator::IsMixture() const {
    if (_current_material->GetNumberOfElements() > 1) {
      return true;
    } else {
      return false;
    }
  }

  G4Material* GeometryNavigator::GetMaterial() const {
    return _current_material;
  }

  double GeometryNavigator::GetNumberOfElements() const {
    return _current_material->GetNumberOfElements();
  }

  double GeometryNavigator::GetFraction(size_t element) const {
    if (element > _current_material->GetNumberOfElements()) {
        throw MAUS::Exception(Exception::recoverable,
                              "Lookup element was > number of elements",
                              "GeometryNavigator::GetFraction()");
    }
    return _current_material->GetFractionVector()[element];
  }

  double GeometryNavigator::GetA(size_t element) const {
    if (element > _current_material->GetNumberOfElements()) {
        throw MAUS::Exception(Exception::recoverable,
                              "Lookup element was > number of elements",
                              "GeometryNavigator::GetA()");
    }
    return _current_material->GetElement(element)->GetA()/(g/mole);
  }


  double GeometryNavigator::GetZ(size_t element) const {
    if (element > _current_material->GetNumberOfElements()) {
        throw MAUS::Exception(Exception::recoverable,
                              "Lookup element was > number of elements",
                              "GeometryNavigator::GetZ()");
    }
    return _current_material->GetElement(element)->GetZ();
  }


  double GeometryNavigator::GetNuclearInteractionLength() const {
//    return _current_material->GetNuclearInterLength()/(g/(cm*cm));
    return _current_material->GetNuclearInterLength()/cm;
  }


  double GeometryNavigator::GetRadiationLength() const {
//    return _current_material->GetRadlen()/(g/(cm*cm));
    return _current_material->GetRadlen()/cm;
  }


  double GeometryNavigator::GetDensity() const {
    return _current_material->GetDensity()/(g/(cm*cm*cm));
  }
}
