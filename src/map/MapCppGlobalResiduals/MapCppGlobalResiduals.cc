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
#include "Interface/Exception.hh"
#include "Interface/dataCards.hh"

// MAUS
#include "API/MapBase.hh"
#include "DataStructure/SciFiSpacePoint.hh"
#include "DataStructure/Spill.hh"
#include "DataStructure/TOFEventSpacePoint.hh"
#include "DataStructure/ThreeVector.hh"
#include "DataStructure/Global/ReconEnums.hh"
#include "DataStructure/Global/Track.hh"
#include "DataStructure/Global/TrackPoint.hh"
#include "src/common_cpp/Optics/CovarianceMatrix.hh"
#include "Recon/Global/DataStructureHelper.hh"
#include "Recon/Global/Detector.hh"
#include "Recon/Global/Particle.hh"
#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"
#include "Utils/Globals.hh"
#include "Utils/JsonWrapper.hh"
#include "Utils/CppErrorHandler.hh"

namespace MAUS {

using MAUS::recon::global::DataStructureHelper;
using MAUS::recon::global::Detector;
using MAUS::recon::global::Particle;
namespace GlobalDS = MAUS::DataStructure::Global;

MapCppGlobalResiduals::MapCppGlobalResiduals() {
}

MapCppGlobalResiduals::~MapCppGlobalResiduals() {
}

bool MapCppGlobalResiduals::birth(std::string configuration) {
  return true;  // Sucessful parsing
}

std::string MapCppGlobalResiduals::process(std::string run_data_string) {
  // parse the JSON document.
  Json::Value run_data_json
    = Json::Value(JsonWrapper::StringToJson(run_data_string));
  if (run_data_json.isNull() || run_data_json.empty()) {
    return std::string("{\"errors\":{\"bad_json_document\":"
                        "\"Failed to parse input document\"}}");
  }

  JsonCppSpillConverter deserialize;
  MAUS::Data * run_data = deserialize(&run_data_json);
  if (!run_data) {
    return std::string("{\"errors\":{\"failed_json_cpp_conversion\":"
                        "\"Failed to convert Json to C++ Data object\"}}");
  }

  const MAUS::Spill * spill = run_data->GetSpill();

  ReconEventPArray * recon_events = spill->GetReconEvents();
  if (!recon_events) {
    return run_data_string;
  }

  ReconEventPArray::const_iterator recon_event;
  for (recon_event = recon_events->begin();
       recon_event < recon_events->end();
       ++recon_event) {
    GlobalEvent * const global_event = (*recon_event)->GetGlobalEvent();

    GlobalDS::TrackPArray tracks;
    LoadReconstructedTracks(global_event, tracks);

    GenerateResidualTrackPoints(global_event, tracks);
  }

  // Serialize the Spill for passing on to the next map in the workflow
  CppJsonSpillConverter serialize;
  Json::FastWriter writer;
  std::string output = writer.write(*serialize(run_data));
  std::cout << "DEBUG MapCppGlobalRawTracks::process(): "
            << "Output: " << std::endl
            << output << std::endl;

  delete run_data;

  return output;
}

bool MapCppGlobalResiduals::death() {
  return true;  // successful
}

void MapCppGlobalResiduals::LoadReconstructedTracks(
    GlobalEvent const * const global_event,
    GlobalDS::TrackPArray & tracks) const {
  GlobalDS::TrackPArray * global_tracks = global_event->get_tracks();
  GlobalDS::TrackPArray::iterator global_track;
  const std::string recon_mapper_name("MapCppGlobalTrackReconstructor");
  for (global_track = global_tracks->begin();
       global_track < global_tracks->end();
       ++global_track) {
    if ((*global_track)->get_mapper_name() == recon_mapper_name) {
      tracks.push_back(*global_track);
    }
  }
}

void MapCppGlobalResiduals::GenerateResidualTrackPoints(
    GlobalEvent * const global_event,
    const GlobalDS::TrackPArray & tracks) const {
  // for each track
  //    1) load constituent tracks with mapper name "MapCppGlobalRawTracks"
  //    2) create TrackPoint residual for each pair of matching TrackPoints
  //       in reconstructed and raw tracks
  //    3) add residual TrackPoint to global_event with mapper name
  //       "MapCppGlobalResiduals"
  GlobalDS::TrackPArray::const_iterator track;
  const std::string raw_mapper_name("MapCppGlobalRawTracks");
  for (track = tracks.begin();
       track < tracks.end();
       ++track) {
    // Find the raw track this reconstructed track is based on
    GlobalDS::ConstTrackPArray raw_tracks = (*track)->GetConstituentTracks();
    GlobalDS::ConstTrackPArray::const_iterator raw_track;
    for (raw_track = raw_tracks.begin();
          raw_track < raw_tracks.end();
          ++raw_track) {
      if ((*raw_track)->get_mapper_name() == raw_mapper_name) {
        break;
      }
    }

    GlobalDS::ConstTrackPointPArray recon_track_points
      = (*track)->GetTrackPoints();
    GlobalDS::ConstTrackPointPArray::const_iterator recon_track_point
      = recon_track_points.begin();
    GlobalDS::ConstTrackPointPArray raw_track_points
      = (*raw_track)->GetTrackPoints();
    GlobalDS::ConstTrackPointPArray::const_iterator raw_track_point
      = raw_track_points.begin();
    // FIXME(Lane) eventually will have to take into account track points
    // without parent track points created between detectors
    while (recon_track_point < recon_track_points.end()) {
      GlobalDS::TrackPoint * residual
        = new GlobalDS::TrackPoint(**recon_track_point);
      residual->set_mapper_name(kClassname);

      TLorentzVector recon_position = (*recon_track_point)->get_position();
      TLorentzVector raw_position = (*raw_track_point)->get_position();
      residual->set_position(recon_position - raw_position);

      TLorentzVector recon_momentum = (*recon_track_point)->get_momentum();
      TLorentzVector raw_momentum = (*raw_track_point)->get_momentum();
      residual->set_momentum(recon_momentum - raw_momentum);

      global_event->add_track_point(residual);

      ++recon_track_point;
    }
  }
}

const std::string MapCppGlobalResiduals::kClassname
  = "MapCppGlobalResiduals";

}  // namespace MAUS

