/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributedTrackReconstructor in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
 /* Author: Peter Lane
 */

#include "src/map/MapCppGlobalResiduals/MapCppGlobalResiduals.hh"

// C++
#include <algorithm>
#include <cmath>
#include <map>
#include <string>
#include <vector>

// External
#include "TMinuit.h"
#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "json/json.h"

// Legacy/G4MICE
#include "Interface/Squeal.hh"
#include "Interface/dataCards.hh"

// MAUS
#include "src/common_cpp/DataStructure/GlobalRawTrack.hh"
#include "src/common_cpp/DataStructure/GlobalTrackPoint.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/TOFEventSpacePoint.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"
#include "src/common_cpp/JsonCppProcessors/GlobalTrackPointProcessor.hh"
#include "src/common_cpp/Optics/CovarianceMatrix.hh"
#include "src/common_cpp/Recon/Global/DataStructureHelper.hh"
#include "src/common_cpp/Recon/Global/Detector.hh"
#include "src/common_cpp/Recon/Global/Particle.hh"
#include "src/common_cpp/Recon/Global/Track.hh"
#include "src/common_cpp/Recon/Global/TrackPoint.hh"
#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"

namespace MAUS {

using MAUS::recon::global::DataStructureHelper;
using MAUS::recon::global::Detector;
using MAUS::recon::global::Particle;
using MAUS::recon::global::Track;
using MAUS::recon::global::TrackPoint;

MapCppGlobalResiduals::MapCppGlobalResiduals() {
}

MapCppGlobalResiduals::~MapCppGlobalResiduals() {
}

bool MapCppGlobalResiduals::birth(std::string configuration) {
  // parse the JSON document.
  try {
    configuration_ = JsonWrapper::StringToJson(configuration);
std::cout << "DEBUG MapCppGlobalResiduals::birth(): CHECKPOINT 1" << std::endl;
    DataStructureHelper::GetInstance().GetDetectorAttributes(
        configuration_, detectors_);
std::cout << "DEBUG MapCppGlobalResiduals::birth(): CHECKPOINT 2" << std::endl;
  } catch(Squeal& squee) {
    MAUS::CppErrorHandler::getInstance()->HandleSquealNoJson(
      squee, MapCppGlobalResiduals::kClassname);
    return false;
  } catch(std::exception& exc) {
    MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(
      exc, MapCppGlobalResiduals::kClassname);
    return false;
  }

  return true;  // Sucessful parsing
}

std::string MapCppGlobalResiduals::process(std::string run_data) {
  // parse the JSON document.
  try {
    run_data_ = Json::Value(JsonWrapper::StringToJson(run_data));

    DataStructureHelper::GetInstance().GetGlobalRawTracks(run_data_,
                                                          detectors_,
                                                          raw_tracks_);
    DataStructureHelper::GetInstance().GetGlobalTracks(run_data_,
                                                       detectors_,
                                                       tracks_);
  } catch(Squeal& squee) {
    MAUS::CppErrorHandler::getInstance()->HandleSquealNoJson(squee, kClassname);
  } catch(std::exception& exc) {
    MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, kClassname);
  }

  GenerateGlobalResiduals();

  // pass on the updated run data to the next map in the workflow
  Json::FastWriter writer;
  std::string output = writer.write(run_data_);

  return output;
}

bool MapCppGlobalResiduals::death() {
  return true;  // successful
}

void MapCppGlobalResiduals::GenerateGlobalResiduals() {
  GlobalTrackPointProcessor serializer;
  Json::Value json_residuals;
std::cout << "DEBUG MapCppGlobalResiduals::GenerateGlobalResiduals(): " << std::endl
          << "Processing " << raw_tracks_.size() << " tracks..." << std::endl;
  // FIXME(Lane) When the global recon data structure is available we should be
  // matching raw tracks with reconstructed tracks by track ID
  for (std::vector<Track>::const_iterator raw_track = raw_tracks_.begin(),
       track = tracks_.begin();
       raw_track != raw_tracks_.end();
       ++raw_track, ++track) {
    for (Track::const_iterator raw_track_point = raw_track->begin();
         raw_track_point != raw_track->end();
         ++raw_track_point) {
      const int track_point_index
        = FindMatchingTrack(*raw_track_point, *track);
      if (track_point_index < 0) {
        throw(Squeal(Squeal::nonRecoverable,
                    "Unable to match raw tracks with reconstructed tracks.",
                    "MapCppGlobalResiduals::CalculateResiduals()"));
      }
      const TrackPoint& track_point = (*track)[track_point_index];
      residuals_.push_back(track_point - *raw_track_point);
      const GlobalTrackPoint residual
        = DataStructureHelper::GetInstance().TrackPointToGlobalTrackPoint(
          residuals_.back(), false);
      Json::Value * json_residual = serializer.CppToJson(residual, "");
      json_residuals.append(*json_residual);
      delete json_residual;
    }
  }

  run_data_["global_residuals"] = json_residuals;
}

int MapCppGlobalResiduals::FindMatchingTrack(const TrackPoint& raw_track_point,
                                             const Track& track) {
  const double raw_z = raw_track_point.z();
std::cout << "DEBUG MapCppGlobalResiduals::FindMatchingTrack(): " << std::endl
          << "raw z: " << raw_z << "\ttrack: " << track << std::endl;
  double z;
  // Do a binary search for the track element with the
  // same z coordinate as the raw track point
  size_t lower_index = 0;
  size_t upper_index = track.size() - 1;
  while (upper_index != lower_index) {
    const size_t index = (lower_index + upper_index) / 2. + 0.5;
    z = track[index].z();
std::cout << "DEBUG MapCppGlobalResiduals::FindMatchingTrack(): " << std::endl
          << "lower index: " << lower_index << "\tupper index: " << upper_index
          << std::endl << "index: " << index << "\tz: " << z << std::endl;
    if (z < raw_z) {
      lower_index = index;
    } else if (z > raw_z) {
      upper_index = index;
    } else {
      lower_index = index;
      upper_index = index;
    }
  }
  if (z != raw_z) {
std::cout << "DEBUG MapCppGlobalResiduals::FindMatchingTrack(): "
          << "raw z: " << raw_z << "\tz: " << z << std::endl;
    return -1;
  }
  return lower_index;
}

const std::string MapCppGlobalResiduals::kClassname
  = "MapCppGlobalResiduals";

}  // namespace MAUS

