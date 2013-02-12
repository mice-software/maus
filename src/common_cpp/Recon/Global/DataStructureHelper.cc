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

#include "src/common_cpp/DataStructure/GlobalTrack.hh"
#include "src/common_cpp/DataStructure/GlobalTrackPoint.hh"
#include "src/common_cpp/JsonCppProcessors/GlobalTrackProcessor.hh"
#include "src/common_cpp/Recon/Global/DataStructureHelper.hh"
#include "src/common_cpp/Recon/Global/Detector.hh"
#include "src/common_cpp/Recon/Global/Track.hh"

namespace MAUS {
namespace recon {
namespace global {

using MAUS::recon::global::Detector;
using MAUS::recon::global::Track;

DataStructureHelper& DataStructureHelper::GetInstance() {
  static DataStructureHelper instance;
  return instance;
}

void DataStructureHelper::GetDetectorAttributes(
    const Json::Value& json_document,
    std::map<Detector::ID, Detector>& detectors) const {
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
    const Detector::ID id = Detector::ID(id_json.asInt());

    const Json::Value plane_json = JsonWrapper::GetProperty(
        detector_json, "plane", JsonWrapper::realValue);
    const double plane = plane_json.asDouble();

    const Json::Value uncertainties_json = JsonWrapper::GetProperty(
        detector_json, "uncertainties", JsonWrapper::arrayValue);
    const CovarianceMatrix uncertainties
        = GetJsonCovarianceMatrix(uncertainties_json);

    const Detector detector(id, plane, uncertainties);
    detectors.insert(std::pair<Detector::ID, Detector>(id, detector));
  }
}

void DataStructureHelper::GetGlobalRawTracks(
    const Json::Value& recon_event,
    const std::map<Detector::ID, Detector>& detectors,
    std::vector<Track>& raw_tracks) {
  GetGlobalTracks(recon_event, "raw_tracks", detectors, raw_tracks);
}

void DataStructureHelper::GetGlobalTracks(
    const Json::Value& recon_event,
    const std::map<Detector::ID, Detector>& detectors,
    std::vector<Track>& tracks) {
  GetGlobalTracks(recon_event, "tracks", detectors, tracks);
}

void DataStructureHelper::GetGlobalTracks(
    const Json::Value& recon_event,
    const std::string& json_node_name,
    const std::map<Detector::ID, Detector>& detectors,
    std::vector<Track>& raw_tracks) {
  Json::Value global_event = JsonWrapper::GetProperty(
      recon_event, "global_event", JsonWrapper::objectValue);
  Json::Value global_raw_tracks = JsonWrapper::GetProperty(
      global_event, json_node_name, JsonWrapper::arrayValue);
  size_t num_tracks = global_raw_tracks.size();
  for (size_t track_index = 0; track_index < num_tracks; ++track_index) {
    GlobalTrackProcessor deserializer;
std::cout << "DEBUG DataStructureHelper::GetGlobalTracks(): CHECKPOINT 1" << std::endl;
    GlobalTrack * const global_raw_track = deserializer.JsonToCpp(
      global_raw_tracks[track_index]);
std::cout << "DEBUG DataStructureHelper::GetGlobalTracks(): CHECKPOINT 2" << std::endl;
    GlobalTrackPointArray global_track_points
      = global_raw_track->track_points();
std::cout << "DEBUG DataStructureHelper::GetGlobalTracks(): CHECKPOINT 3" << std::endl;
    delete global_raw_track;

    Track raw_track;
    for (GlobalTrackPointArray::const_iterator global_track_point
            = global_track_points.begin();
         global_track_point < global_track_points.end();
         ++global_track_point) {
      ThreeVector position = global_track_point->position();
      ThreeVector momentum = global_track_point->momentum();
      Particle::ID particle_id
        = Particle::ID(global_track_point->particle_id());
      TrackPoint track_point(global_track_point->time(),
                             global_track_point->energy(),
                             position.x(), momentum.x(),
                             position.y(), momentum.y(),
                             particle_id, position.z());
      const DetectorMap::const_iterator detector_map_entry = detectors.find(
        Detector::ID(global_track_point->detector_id()));
      if (detector_map_entry != detectors.end()) {
        track_point.set_detector_id(detector_map_entry->second.id());
      }

      raw_track.push_back(track_point);
    }
    raw_tracks.push_back(raw_track);
  }
}

Json::Value DataStructureHelper::TrackToJson(const Track & track) {
  GlobalTrackPointArray global_track_points;

  for (Track::const_iterator track_point = track.begin();
       track_point < track.end();
       ++track_point) {
    GlobalTrackPoint global_track_point
      = TrackPointToGlobalTrackPoint(*track_point);
    global_track_points.push_back(global_track_point);
  }

  GlobalTrack global_track;
  global_track.set_track_points(global_track_points);

  GlobalTrackProcessor serializer;
  Json::Value * json_pointer = serializer.CppToJson(global_track, "");
  Json::Value json = *json_pointer;
  delete json_pointer;
  return json;
}

GlobalTrackPoint DataStructureHelper::TrackPointToGlobalTrackPoint(
    const MAUS::recon::global::TrackPoint& track_point) {
  return TrackPointToGlobalTrackPoint(track_point, true);
}

GlobalTrackPoint DataStructureHelper::TrackPointToGlobalTrackPoint(
    const MAUS::recon::global::TrackPoint& track_point,
    const bool on_mass_shell) {
  GlobalTrackPoint global_track_point;
  global_track_point.set_time(track_point.time());
  global_track_point.set_energy(track_point.energy());
  ThreeVector position(track_point.x(), track_point.y(), track_point.z());
  global_track_point.set_position(position);
  double pz = 0;
  if (on_mass_shell) {
    pz = track_point.Pz();
  }
  ThreeVector momentum(track_point.Px(),
                        track_point.Py(),
                        pz);
  global_track_point.set_momentum(momentum);
  global_track_point.set_detector_id(track_point.detector_id());
  global_track_point.set_particle_id(track_point.particle_id());
  return global_track_point;
}

CovarianceMatrix DataStructureHelper::GetJsonCovarianceMatrix(
    const Json::Value& value) const {
  if (value.size() < static_cast<Json::Value::UInt>(6)) {
    throw(Squeal(Squeal::recoverable,
                 "Not enough row elements to convert JSON to CovarianceMatrix",
                 "DataStructureHelper::GetJsonCovarianceMatrix()"));
  }

  const size_t size = 6;
  double matrix_data[size*size];
  for (size_t row = 0; row < 6; ++row) {
    const Json::Value row_json = value[row];
    if (row_json.size() < static_cast<Json::Value::UInt>(6)) {
      throw(Squeal(
          Squeal::recoverable,
          "Not enough column elements to convert JSON to CovarianceMatrix",
          "DataStructureHelper::GetJsonCovarianceMatrix()"));
    }
    for (size_t column = 0; column < 6; ++column) {
      const Json::Value element_json = row_json[column];
      matrix_data[row*size+column] = element_json.asDouble();
    }
  }

  return CovarianceMatrix(matrix_data);
}

}  // namespace global
}  // namespace recon
}  // namespace MAUS

