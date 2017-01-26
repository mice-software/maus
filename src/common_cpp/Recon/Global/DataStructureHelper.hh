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

#ifndef COMMON_CPP_DATASTRUCTUREHELPER_HH
#define COMMON_CPP_DATASTRUCTUREHELPER_HH

#include <map>
#include <string>
#include <vector>

#include "TLorentzVector.h"

#include "src/common_cpp/Recon/Global/Detector.hh"
#include "src/common_cpp/DataStructure/Primary.hh"
#include "src/common_cpp/DataStructure/Global/PrimaryChain.hh"
#include "src/common_cpp/DataStructure/Global/ReconEnums.hh"
#include "src/common_cpp/DataStructure/Global/TrackPoint.hh"
#include "src/common_cpp/Optics/PhaseSpaceVector.hh"
#include "src/common_cpp/Simulation/MAUSPrimaryGeneratorAction.hh"

class MiceModule;

namespace MAUS {
namespace GlobalDS = ::MAUS::DataStructure::Global;

namespace recon {
namespace global {

class DataStructureHelper {
 public:
  ~DataStructureHelper() {}
  static const DataStructureHelper& GetInstance();
  std::vector<const MiceModule *> FindModulesByName(const MiceModule * module,
                                                    std::string name) const;
  double GetDetectorZPosition(const GlobalDS::DetectorPoint detector_id) const;
  void GetDetectorAttributes(const Json::Value& json_document,
                             DetectorMap& detectors)
                                      const;
  std::vector<MAUS::DataStructure::Global::PrimaryChain*>*
  GetPrimaryChains(const Json::Value& recon_event) const;
  MAUS::PhaseSpaceVector TrackPoint2PhaseSpaceVector(
      const MAUS::DataStructure::Global::TrackPoint& track_point) const;
  MAUS::DataStructure::Global::TrackPoint PhaseSpaceVector2TrackPoint(
      const MAUS::PhaseSpaceVector& vector,
      const double z,
      const MAUS::DataStructure::Global::PID particle_id) const;


  /** Convert from a string name to a Detector type
   *  - name: string name for a given detector
   *  Returns a DetectorPoint corresponding to the given name. Throws a
   *  MAUS::Exception if name was not recognised.
   */
  inline GlobalDS::DetectorPoint StringToDetectorPoint(std::string name) const;

  /** Convert from a Detector type to a string name
   *  - point: detector type
   *  Returns a string name corresponding to the given type. Throws a
   *  MAUS::Exception if name was not recognised.
   */
  inline std::string DetectorPointToString(GlobalDS::DetectorPoint point) const;
 protected:
  void InitialiseNames();
  DataStructureHelper() {InitialiseNames();}
  CovarianceMatrix GetJsonCovarianceMatrix(
      const Json::Value& value) const;
  // These have to be static because std::map does not support key lookup on
  // const std::map (not sure why)
  static std::map<std::string, GlobalDS::DetectorPoint> _names_to_points;
  static std::map<GlobalDS::DetectorPoint, std::string> _points_to_names;
};

GlobalDS::DetectorPoint DataStructureHelper::StringToDetectorPoint(std::string name) const {
    return _names_to_points[name];
}

std::string DataStructureHelper::DetectorPointToString(GlobalDS::DetectorPoint point) const {
    return _points_to_names[point];
}

}  // namespace global
}  // namespace recon
}  // namespace MAUS

#endif
