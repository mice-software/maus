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
  ~DataStructureHelper() { }
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
  MAUS::Primary PGParticle2Primary(
      MAUS::MAUSPrimaryGeneratorAction::PGParticle& pgparticle) const;
  
  /*
  void GetGlobalRawTracks(const Json::Value& recon_event,
                          const DetectorMap& detectors,
                          std::vector<MAUS::recon::global::Track>& raw_tracks);
  void GetGlobalTracks(const Json::Value& recon_event,
                       const DetectorMap& detectors,
                       std::vector<MAUS::recon::global::Track>& raw_tracks);
  void GetGlobalTracks(const Json::Value& recon_event,
                       const std::string& json_node_name,
                       const DetectorMap& detectors,
                       std::vector<MAUS::recon::global::Track>& raw_tracks);

  Json::Value TrackToJson(const MAUS::recon::global::Track& track);
  GlobalTrackPoint TrackPointToGlobalTrackPoint(
      const MAUS::recon::global::TrackPoint& track_point);
  GlobalTrackPoint TrackPointToGlobalTrackPoint(
      const MAUS::recon::global::TrackPoint& track_point,
      const bool on_mass_shell);
  */
 protected:
  DataStructureHelper() { };
  CovarianceMatrix GetJsonCovarianceMatrix(
      const Json::Value& value) const;
};

}  // namespace global
}  // namespace recon
}  // namespace MAUS

#endif
