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
 /* Author: Peter Lane
 */

#include "src/map/MapCppGlobalTrackReconstructor/MapCppGlobalTrackReconstructor.hh"

// C++
#include <algorithm>
#include <map>
#include <vector>
#include <string>

// External
#include "TLorentzVector.h"
#include "TMinuit.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "json/json.h"

// Legacy/G4MICE
#include "Utils/Exception.hh"

// MAUS
#include "Converter/DataConverters/JsonCppSpillConverter.hh"
#include "Converter/DataConverters/CppJsonSpillConverter.hh"
#include "DataStructure/GlobalEvent.hh"
#include "DataStructure/ReconEvent.hh"
#include "DataStructure/Spill.hh"
#include "DataStructure/Global/Track.hh"
#include "DataStructure/Global/TrackPoint.hh"
#include "DataStructure/ThreeVector.hh"
#include "src/common_cpp/Optics/CovarianceMatrix.hh"
#include "src/common_cpp/Optics/LinearApproximationOpticsModel.hh"
#include "src/common_cpp/Optics/PolynomialOpticsModel.hh"
#include "Recon/Global/DataStructureHelper.hh"
#include "Recon/Global/Detector.hh"
#include "Recon/Global/MinuitTrackFitter.hh"
#include "Recon/Global/Particle.hh"
#include "Recon/Global/TrackFitter.hh"
#include "Simulation/MAUSGeant4Manager.hh"
#include "Utils/JsonWrapper.hh"
#include "Utils/CppErrorHandler.hh"

namespace MAUS {

using MAUS::recon::global::DataStructureHelper;
using MAUS::recon::global::Detector;
using MAUS::recon::global::MinuitTrackFitter;
using MAUS::recon::global::Particle;
using MAUS::recon::global::TrackFitter;
namespace GlobalDS = MAUS::DataStructure::Global;

MapCppGlobalTrackReconstructor::MapCppGlobalTrackReconstructor()
    : optics_model_(NULL), track_fitter_(NULL) {
}

MapCppGlobalTrackReconstructor::~MapCppGlobalTrackReconstructor() {
  if (optics_model_ != NULL) {
    delete optics_model_;
  }

  if (track_fitter_ != NULL) {
    delete track_fitter_;
  }
}

bool MapCppGlobalTrackReconstructor::birth(std::string configuration_string) {
  // parse the JSON document.
  try {
    configuration_ = JsonWrapper::StringToJson(configuration_string);
    Json::Value physics_processes = configuration_["physics_processes"];
    if ((physics_processes != Json::Value("mean_energy_loss"))
        && (physics_processes != Json::Value("none"))) {
      throw(Exception(Exception::nonRecoverable,
                  "The \"physics_processes\" configuration variable should be "
                  "set to \"mean_energy_loss\" or \"none\" to avoid collisions "
                  "of the test particles with walls.",
                  "MAUS::MapCppGlobalTrackReconstructor::birth()"));
    }
    DataStructureHelper::GetInstance().GetDetectorAttributes(
        configuration_, detectors_);
    SetupOpticsModel();
    SetupTrackFitter();
  } catch (Exception& exc) {
    MAUS::CppErrorHandler::getInstance()->HandleExceptionNoJson(
      exc, MapCppGlobalTrackReconstructor::kClassname);
    return false;
  } catch (std::exception& exc) {
    MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(
      exc, MapCppGlobalTrackReconstructor::kClassname);
    return false;
  }

  return true;  // Sucessful parsing
}

std::string MapCppGlobalTrackReconstructor::process(
    std::string run_data_string) {
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
  MAUS::ReconEventPArray * recon_events = spill->GetReconEvents();
  if (!recon_events) {
    return run_data_string;
  }

  // FIXME(Lane) remove once particle identification is working
  // ===========================================================================
  MAUSGeant4Manager * const simulator = MAUSGeant4Manager::GetInstance();
  MAUSPrimaryGeneratorAction::PGParticle reference_pgparticle
    = simulator->GetReferenceParticle();
  const GlobalDS::PID particle_id = GlobalDS::PID(reference_pgparticle.pid);
  // ===========================================================================

  MAUS::ReconEventPArray::const_iterator recon_event;
  for (recon_event = recon_events->begin();
      recon_event != recon_events->end();
      ++recon_event) {
    MAUS::GlobalEvent * const global_event = (*recon_event)->GetGlobalEvent();

    GlobalDS::TrackPArray raw_tracks;
    LoadRawTracks(global_event, raw_tracks);
    std::cerr << "Loaded " << raw_tracks.size() << " raw tracks." << std::endl;
    // Fit each raw track and store best fit track in global_event
    // FIXME(Lane) make const_iterator once particle identification is working
    GlobalDS::TrackPArray::iterator raw_track;
    for (raw_track = raw_tracks.begin();
         raw_track != raw_tracks.end();
         ++raw_track) {
      GlobalDS::Track * best_fit_track = new GlobalDS::Track();
      best_fit_track->set_mapper_name(kClassname);
      best_fit_track->AddTrack(*raw_track);

      // FIXME(Lane) remove once particle identification is working
      // =======================================================================
      (*raw_track)->set_pid(particle_id);
      // =======================================================================
      std::cout << "DEBUG MapCppGlobalTrackReconstructor::process: "
                << "raw track PID: " << (*raw_track)->get_pid() << std::endl;
      track_fitter_->Fit(*raw_track, best_fit_track, kClassname);

      InsertIntermediateTrackPoints(best_fit_track);

      global_event->add_track_recursive(best_fit_track);
    }

    std::vector<MAUS::DataStructure::Global::Track*>* tracks
      = global_event->get_tracks();
    std::cout << "Added " << (tracks->size()-1)<< " reconstructed tracks:"
              << std::endl;
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

bool MapCppGlobalTrackReconstructor::death() {
  return true;  // successful
}

void MapCppGlobalTrackReconstructor::SetupOpticsModel() {
fprintf(stdout, "CHECKPOINT: SetupOpticsModel() 0\n");
fflush(stdout);
  Json::Value optics_model_names = JsonWrapper::GetProperty(
      configuration_,
      "global_recon_optics_models",
      JsonWrapper::arrayValue);
  Json::Value optics_model_name = JsonWrapper::GetProperty(
      configuration_,
      "global_recon_optics_model",
      JsonWrapper::stringValue);
  size_t model;
  for (model = 0; model < optics_model_names.size(); ++model) {
    if (optics_model_name == optics_model_names[model]) {
      break;  // leave the current index into optics_model_names in model
    }
  }

fprintf(stdout, "CHECKPOINT: SetupOpticsModel() 1\n");
fflush(stdout);
  switch (model) {
    case 0: {
      // "Differentiating"
      /* TODO(plane1@hawk.iit.edu)
      optics_model_ = new DifferentiatingOpticsModel();
      */
      throw(Exception(Exception::nonRecoverable,
                   "DifferentiatingOpticsModel is not yet implemented.",
                   "MapCppGlobalTrackReconstructor::SetupOpticsModel()"));
      break;
    }
    case 1: {
      // "Integrating"
      /* TODO(plane1@hawk.iit.edu)
      optics_model_ = new IntegratingOpticsModel();
      */
      throw(Exception(Exception::nonRecoverable,
                   "IntegratingOpticsModel is not yet implemented.",
                   "MapCppGlobalTrackReconstructor::SetupOpticsModel()()"));
      break;
    }
    case 2: {
fprintf(stdout, "CHECKPOINT SetupOpticsModel() 1.1c\n");
fflush(stdout);
     optics_model_ = new PolynomialOpticsModel(&configuration_);
      break;
    }
    case 3: {
      // "Runge Kutta"
      /* TODO(plane1@hawk.iit.edu)
      optics_model_ = new RungeKuttaOpticsModel();
      */
      throw(Exception(Exception::nonRecoverable,
                   "RungeKuttaOpticsModel is not yet implemented.",
                   "MapCppGlobalTrackReconstructor::SetupOpticsModel()()"));
      break;
    }
    case 4: {
      // "Linear Approximation"
fprintf(stdout, "CHECKPOINT SetupOpticsModel() 1.1e\n");
fflush(stdout);
     optics_model_ = new LinearApproximationOpticsModel(&configuration_);
      break;
    }
    default: {
      std::string message("Unsupported optics model \"");
      message += optics_model_name.asString();
      message += std::string("\" in recon configuration.");
      throw(Exception(Exception::nonRecoverable,
                   message.c_str(),
                   "MapCppGlobalTrackReconstructor::SetupOpticsModel()()"));
    }
  }
fprintf(stdout, "CHECKPOINT: SetupOpticsModel() 2\n");
fflush(stdout);
  optics_model_->Build();

fprintf(stdout, "CHECKPOINT: SetupOpticsModel() 3\n");
fflush(stdout);
}

void MapCppGlobalTrackReconstructor::SetupTrackFitter() {
  Json::Value fitter_names = JsonWrapper::GetProperty(
      configuration_,
      "global_recon_track_fitters",
      JsonWrapper::arrayValue);
  Json::Value fitter_name = JsonWrapper::GetProperty(
      configuration_,
      "global_recon_track_fitter",
      JsonWrapper::stringValue);
  size_t fitter;
  for (fitter = 0; fitter < fitter_names.size(); ++fitter) {
    if (fitter_name == fitter_names[fitter]) {
      break;  // leave the current index into fitter_names in fitter
    }
  }

  switch (fitter) {
    case 0: {
      // Minuit
      double start_plane = optics_model_->primary_plane();
      track_fitter_ = new MinuitTrackFitter(optics_model_, start_plane);
      break;
    }
    case 1: {
      // Kalman Filter
      /* TODO(plane1@hawk.iit.edu)
      track_fitter_ = new KalmanFilterTrackFitter(optics_model_);
      */
      throw(Exception(Exception::nonRecoverable,
                   "KalmanFilterTrackFitter is not yet implemented.",
                   "MapCppGlobalTrackReconstructor::SetupTrackFitter()"));
      break;
    }
    case 2: {
      // TOF
      /* TODO(plane1@hawk.iit.edu)
      track_fitter_ = new TOFTrackFitter(optics_model_);
      */
      throw(Exception(Exception::nonRecoverable,
                   "TOFTrackFitter is not yet implemented.",
                   "MapCppGlobalTrackReconstructor::SetupTrackFitter()"));
      break;
    }
    default: {
      std::string message("Unsupported track fitter \"");
      message += fitter_name.asString();
      message += std::string("\" in recon configuration.");
      throw(Exception(Exception::nonRecoverable,
                   message.c_str(),
                   "MapCppGlobalTrackReconstructor::SetupTrackFitter()"));
    }
  }
}

void MapCppGlobalTrackReconstructor::LoadRawTracks(
    GlobalEvent const * const global_event,
    GlobalDS::TrackPArray & tracks) const {
  GlobalDS::TrackPArray * global_tracks = global_event->get_tracks();
  GlobalDS::TrackPArray::iterator global_track;
  const std::string recon_mapper_name("MapCppGlobalRawTracks");
  for (global_track = global_tracks->begin();
       global_track != global_tracks->end();
       ++global_track) {
    if ((*global_track)->get_mapper_name() == recon_mapper_name) {
      tracks.push_back(*global_track);
    }
  }
}

void MapCppGlobalTrackReconstructor::InsertIntermediateTrackPoints(
    GlobalDS::Track * track) const {
  PolynomialOpticsModel * const optics_model
    = static_cast<MAUS::PolynomialOpticsModel*>(optics_model_);
  if (optics_model == NULL) {
    throw(Exception(Exception::nonRecoverable,
                "Could not reconstruct intermediate track points: the optics "
                "model being used is not yet fully supported.",
                "MAUS::MapCppGlobalTrackReconstructor::"
                "InsertIntermediateTrackPoints()"));
  }

  // Get the fit track points
  GlobalDS::TrackPointCPArray fit_points = track->GetTrackPoints();

  // Convert the first fit track point into a PhaseSpaceVector for transporting
  GlobalDS::TrackPoint const * const fit_primary_track_point = fit_points[0];
  size_t particle_event = fit_primary_track_point->get_particle_event();
  DataStructureHelper helper = DataStructureHelper::GetInstance();
  PhaseSpaceVector fit_primary
    = helper.TrackPoint2PhaseSpaceVector(*fit_primary_track_point);

  // Construct a list of detector z-keys (z-position rounded to nearest integer)
  std::vector<int64_t> z_keys;
  GlobalDS::TrackPointCPArray::const_iterator fit_point
    = fit_points.begin();
  for (fit_point = fit_points.begin();
       fit_point != fit_points.end();
       ++fit_point) {
    // calculate the next guess
    const double z = (*fit_point)->get_position().Z();
    int64_t z_key = (z >= 0?static_cast<int64_t>(z+.5):static_cast<int64_t>(z-.5));
    z_keys.push_back(z_key);
  }

  // Reconstruct the intermediate track points by transporting the fit primary
  // to all desired intermediate z-positions
  // FIXME(Lane) restore once particle identification is working
  // const GlobalDS::PID particle_id = track->get_pid();
  // FIXME(Lane) remove once particle identification is working
  // ===========================================================================
  MAUSGeant4Manager * const simulator = MAUSGeant4Manager::GetInstance();
  MAUSPrimaryGeneratorAction::PGParticle reference_pgparticle
    = simulator->GetReferenceParticle();
  const GlobalDS::PID particle_id = GlobalDS::PID(reference_pgparticle.pid);
  // ===========================================================================

  std::vector<int64_t>::const_iterator z_key = z_keys.begin();
  const std::vector<int64_t> map_positions
    = optics_model->GetAvailableMapPositions();
  std::vector<int64_t>::const_iterator map_z;
  for (map_z = map_positions.begin(); map_z != map_positions.end(); ++map_z) {
    // locate the next detector z-position
    while (z_key != z_keys.end() && (*z_key) < (*map_z)) {
      ++z_key;
    }

    // transport the fit primary to the desired z-position
    const PhaseSpaceVector point = optics_model_->Transport(fit_primary,
                                                            *map_z);
    std::cout << "DEBUG MapCppGlobalTrackReconstructor"
              << "::InsertIntermediateTrackPoints: track point: "
              << point << std::endl;

    // skip if we've already added this point to the track during fitting
    if ((*z_key) == (*map_z)) {
      continue;
    }

    // add the reconstructed intermediate to the track
    GlobalDS::TrackPoint track_point;
    try {
      track_point
        = helper.PhaseSpaceVector2TrackPoint(point, *map_z, particle_id);
    } catch (Exception exc) {
        std::cerr << "DEBUG MapCppGlobalTrackReconstructor"
                  << "::InsertIntermediateTrackPoints: "
                  << "something bad happened during track fitting: "
                  << exc.what() << std::endl;
    }

    track_point.set_mapper_name(kClassname);
    track_point.set_particle_event(particle_event);
    track->AddTrackPoint(new GlobalDS::TrackPoint(track_point));
  }
  track->SortTrackPointsByZ();
}

const std::string MapCppGlobalTrackReconstructor::kClassname
  = "MapCppGlobalTrackReconstructor";

}  // namespace MAUS
