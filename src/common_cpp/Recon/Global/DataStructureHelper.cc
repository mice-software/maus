/* This file is part of MAUS: http://  micewww.pp.rl.ac.uk:8080/projects/maus
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
 * along with MAUS.  If not, see <http://  www.gnu.org/licenses/>.
 */

/* Author: Peter Lane
 */

#include <iostream>
#include <sstream>
#include <map>
#include <vector>

#include "TLorentzVector.h"

#include "DataStructure/GlobalEvent.hh"
#include "DataStructure/Primary.hh"
#include "DataStructure/ThreeVector.hh"
#include "DataStructure/Global/PrimaryChain.hh"
#include "DataStructure/Global/Track.hh"
#include "DataStructure/Global/TrackPoint.hh"
#include "DataStructure/Global/ReconEnums.hh"
#include "JsonCppProcessors/GlobalEventProcessor.hh"
#include "Recon/Global/DataStructureHelper.hh"
#include "Recon/Global/Detector.hh"
#include "Recon/Global/Particle.hh"
#include "src/common_cpp/Simulation/MAUSPrimaryGeneratorAction.hh"
#include "Utils/Globals.hh"
#include "Config/MiceModule.hh"

// legacy classes outside the MAUS namespace
namespace MAUS {
namespace recon {
namespace global {

using MAUS::PhaseSpaceVector;
using MAUS::DataStructure::Global::DetectorPoint;
using MAUS::DataStructure::Global::PID;
using MAUS::DataStructure::Global::PrimaryChain;
using MAUS::DataStructure::Global::Track;
using MAUS::DataStructure::Global::TrackPoint;
using MAUS::recon::global::Detector;
using MAUS::recon::global::Particle;

std::map<std::string, GlobalDS::DetectorPoint> DataStructureHelper::_names_to_points
                             = std::map<std::string, GlobalDS::DetectorPoint>();
std::map<GlobalDS::DetectorPoint, std::string> DataStructureHelper::_points_to_names
                             = std::map<GlobalDS::DetectorPoint, std::string>();

const DataStructureHelper& DataStructureHelper::GetInstance() {
  static DataStructureHelper instance;
  return instance;
}

// This differes from the function in MiceModule in that it searches based on
// just the local name, not the full node path name. The former is just a lazy
// way of doing a tree traversal without parsing the node path name. This
// function has no knowledge of where the node is located and will return nodes
// of the same local name under different mother nodes.
std::vector<const MiceModule *> DataStructureHelper::FindModulesByName(
    const MiceModule * module,
    std::string name) const {
  std::vector<const MiceModule*> modules;

  if (module->name() == name)
    modules.push_back(module);

  for (int index = 0; index < module->daughters(); ++index) {
    std::vector<const MiceModule *> tmp
      = FindModulesByName(module->daughter(index), name);
    std::vector<const MiceModule *>::iterator daughter_module;
    for (daughter_module = tmp.begin();
         daughter_module != tmp.end();
         ++daughter_module) {
      modules.push_back(*daughter_module);
    }
  }

  return modules;
}

double DataStructureHelper::GetDetectorZPosition(
    const GlobalDS::DetectorPoint detector_id) const {
  MiceModule const * const geometry
    = Globals::GetInstance()->GetReconstructionMiceModules();
  std::vector<const MiceModule *> modules;

  std::stringstream detector_name;
  std::cout << "Root MiceModule " << geometry->fullName() << " has "
            << geometry->daughters() << " daughters." << std::endl;
  std::cout << "Selecting on detector " << detector_id << std::endl;
  switch (detector_id) {
    case GlobalDS::kTOF0: {
      detector_name << "TOF0.dat";
      modules = FindModulesByName(geometry, detector_name.str());
      break;
    }
    case GlobalDS::kTOF1: {
      detector_name << "TOF1Detector.dat";
      modules = FindModulesByName(geometry, detector_name.str());
      break;
    }
    case GlobalDS::kTOF2: {
      detector_name << "TOF2Detector.dat";
      modules = FindModulesByName(geometry, detector_name.str());
      break;
    }
    case GlobalDS::kTracker0_1:
    case GlobalDS::kTracker0_2:
    case GlobalDS::kTracker0_3:
    case GlobalDS::kTracker0_4:
    case GlobalDS::kTracker0_5:
    case GlobalDS::kTracker1_1:
    case GlobalDS::kTracker1_2:
    case GlobalDS::kTracker1_3:
    case GlobalDS::kTracker1_4:
    case GlobalDS::kTracker1_5: {
      GlobalDS::DetectorPoint station = GlobalDS::DetectorPoint(
        detector_id - GlobalDS::kTracker0);
      if (station > 5) {  // 5 stations per tracker
        station = GlobalDS::DetectorPoint(detector_id - GlobalDS::kTracker1);
        detector_name << "Tracker1Station";
      } else {
        detector_name << "TrackerStation";
      }
      detector_name << station << ".dat";
      std::vector<const MiceModule *> mothers
        = FindModulesByName(geometry, detector_name.str());
      if (mothers.size() == 1) {
        std::string view_name = "TrackerViewW.dat";
        if (detector_id == GlobalDS::kTracker0_5) {
          view_name = "Tracker0Station5ViewW.dat";
        }
        modules = FindModulesByName(mothers[0], view_name);
        detector_name << "/" << view_name;  // for exception message if needed
      } else if (mothers.size() > 1) {
        std::stringstream message;
        message << "Found multiple detector geometry modules named \""
                << detector_name.str() << "\".";
        throw(Exceptions::Exception(Exceptions::recoverable,
                      message.str(),
                      "DataStructureHelper::GetDetectorZPosition()"));
      } else {
        std::stringstream message;
        message << "Couldn't find detector geometry module \""
                << detector_name.str() << "\".";
        throw(Exceptions::Exception(Exceptions::recoverable,
                      message.str(),
                      "DataStructureHelper::GetDetectorZPosition()"));
      }
      break;
    }
    default: detector_name << "unknown";
  }

  std::cout << "Detector name: " << detector_name.str() << std::endl;

  if (modules.size() == 0) {
    std::stringstream message;
    message << "Couldn't find reconstruction mapping detector \""
            << detector_name.str() << "\".";
    throw(Exceptions::Exception(Exceptions::recoverable,
                  message.str(),
                  "DataStructureHelper::GetDetectorZPosition()"));
  } else if (modules.size() > 1) {
    std::stringstream message;
    message << "Found multiple reconstruction mapping detectors named \""
            << detector_name.str() << "\".";
    throw(Exceptions::Exception(Exceptions::recoverable,
                  message.str(),
                  "DataStructureHelper::GetDetectorZPosition()"));
  }

  std::cout << "Detector z position: " << modules[0]->globalPosition().z() << std::endl;

  return modules[0]->globalPosition().z();
}

void DataStructureHelper::GetDetectorAttributes(
    const Json::Value& json_document,
    std::map<DetectorPoint, Detector>& detectors) const {
  // FIXME(plane1@hawk.iit.edu) Once the detector groups provide this
  // information this will need to be changed
  Json::Value detector_attributes_json = JsonWrapper::GetProperty(
      json_document, "global_recon_detector_attributes",
      JsonWrapper::arrayValue);

  // *** Get detector info ***
  const Json::Value::UInt detector_count = detector_attributes_json.size();
  for (Json::Value::UInt index = 0; index < detector_count; ++index) {
    const Json::Value detector_json = detector_attributes_json[index];
    const Json::Value id_json = JsonWrapper::GetProperty(
        detector_json, "id", JsonWrapper::intValue);
    const DetectorPoint id = DetectorPoint(id_json.asInt());

    const Json::Value uncertainties_json = JsonWrapper::GetProperty(
        detector_json, "uncertainties", JsonWrapper::arrayValue);
    const CovarianceMatrix uncertainties
        = GetJsonCovarianceMatrix(uncertainties_json);

    const Detector detector(id, uncertainties);
    detectors.insert(std::pair<DetectorPoint, Detector>(id, detector));
  }
}

PhaseSpaceVector DataStructureHelper::TrackPoint2PhaseSpaceVector(
    const TrackPoint& track_point) const {
  TLorentzVector position = track_point.get_position();
  TLorentzVector momentum = track_point.get_momentum();
  return MAUS::PhaseSpaceVector(
    position.T(), momentum.E(),
    position.X(), momentum.Px(),
    position.Y(), momentum.Py());
}

TrackPoint DataStructureHelper::PhaseSpaceVector2TrackPoint(
      const PhaseSpaceVector& vector,
      const double z,
      const PID particle_id) const {
    TrackPoint track_point;
    const TLorentzVector position(vector.x(), vector.y(), z, vector.t());
    track_point.set_position(position);

    const double energy = vector.E();
    const double px = vector.Px();
    const double py = vector.Py();
    const double mass = Particle::GetInstance().GetMass(particle_id);
    double pz = ::sqrt(energy*energy - mass*mass - px*px - py*py);
    std::cout << "DEBUG DataStructureHelper::PhaseSpaceVector2TrackPoint: "
              << "mass: " << mass
              << "\t: " << vector.t() << "\tE: " << energy
              << "\tx: " << vector.x() << "\tPx: " << px
              << "\ty: " << vector.y() << "\tPy: " << py
              << "\tz: " << z << "\tPz: " << pz << std::endl;
    if (pz != pz) {
      pz = 0.;
    }

    const TLorentzVector momentum(px, py, pz, energy);
    track_point.set_momentum(momentum);

    track_point.set_detector(MAUS::DataStructure::Global::kUndefined);
    return track_point;
}

std::vector<PrimaryChain*>* DataStructureHelper::GetPrimaryChains(
    const Json::Value& recon_event) const {
  Json::Value global_event_json = JsonWrapper::GetProperty(
      recon_event, "global_event", JsonWrapper::objectValue);
  MAUS::GlobalEventProcessor deserializer;
  GlobalEvent * global_event = deserializer.JsonToCpp(global_event_json);

  return global_event->get_primary_chains();
}

CovarianceMatrix DataStructureHelper::GetJsonCovarianceMatrix(
    const Json::Value& value) const {
  if (value.size() < static_cast<Json::Value::UInt>(6)) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Not enough row elements to convert JSON to CovarianceMatrix",
                 "DataStructureHelper::GetJsonCovarianceMatrix()"));
  }

  const size_t size = 6;
  double matrix_data[size*size];
  for (int row = 0; row < 6; ++row) {
    const Json::Value row_json = value[row];
    if (row_json.size() < static_cast<Json::Value::UInt>(6)) {
      throw(Exceptions::Exception(
          Exceptions::recoverable,
          "Not enough column elements to convert JSON to CovarianceMatrix",
          "DataStructureHelper::GetJsonCovarianceMatrix()"));
    }
    for (int column = 0; column < 6; ++column) {
      const Json::Value element_json = row_json[column];
      matrix_data[row*size+column] = element_json.asDouble();
    }
  }

  CovarianceMatrix matrix(matrix_data);
  return matrix;
}

void DataStructureHelper::InitialiseNames() {
    namespace Global = MAUS::DataStructure::Global;
    _names_to_points["Undefined"] = Global::kUndefined;
    _names_to_points["Virtual"] = Global::kVirtual;
    _names_to_points["TOF0"] = Global::kTOF0;
    _names_to_points["TOF0_1"] = Global::kTOF0_1;
    _names_to_points["TOF0_2"] = Global::kTOF0_2;
    _names_to_points["CherenkovA"] = Global::kCherenkovA;
    _names_to_points["CherenkovB"] = Global::kCherenkovB;
    _names_to_points["TOF1"] = Global::kTOF1;
    _names_to_points["TOF1_1"] = Global::kTOF1_1;
    _names_to_points["TOF1_2"] = Global::kTOF1_2;
    _names_to_points["Tracker0"] = Global::kTracker0;
    _names_to_points["Tracker0_1"] = Global::kTracker0_1;
    _names_to_points["Tracker0_2"] = Global::kTracker0_2;
    _names_to_points["Tracker0_3"] = Global::kTracker0_3;
    _names_to_points["Tracker0_4"] = Global::kTracker0_4;
    _names_to_points["Tracker0_5"] = Global::kTracker0_5;
    _names_to_points["Tracker1"] = Global::kTracker1;
    _names_to_points["Tracker1_1"] = Global::kTracker1_1;
    _names_to_points["Tracker1_2"] = Global::kTracker1_2;
    _names_to_points["Tracker1_3"] = Global::kTracker1_3;
    _names_to_points["Tracker1_4"] = Global::kTracker1_4;
    _names_to_points["Tracker1_5"] = Global::kTracker1_5;
    _names_to_points["TOF2"] = Global::kTOF2;
    _names_to_points["TOF2_1"] = Global::kTOF2_1;
    _names_to_points["TOF2_2"] = Global::kTOF2_2;
    _names_to_points["Calorimeter"] = Global::kCalorimeter;
    _names_to_points["EMR"] = Global::kEMR;
    typedef std::map<std::string, GlobalDS::DetectorPoint>::iterator names_it;
    for (names_it it = _names_to_points.begin(); it != _names_to_points.end(); ++it) {
        _points_to_names[it->second] = it->first;
    }
}


}  // namespace global
}  // namespace recon
}  // namespace MAUS

