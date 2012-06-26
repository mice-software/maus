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
#include <vector>
#include <string>

// External
#include "TMinuit.h"
#include "json/json.h"

// Legacy/G4MICE
#include "Interface/Squeal.hh"
#include "Interface/dataCards.hh"

// MAUS
#include "src/common_cpp/Optics/CovarianceMatrix.hh"
#include "src/common_cpp/Optics/LinearApproximationOpticsModel.hh"
#include "src/common_cpp/Optics/PolynomialOpticsModel.hh"
#include "src/common_cpp/Reconstruction/Global/Detector.hh"
#include "src/common_cpp/Reconstruction/Global/MinuitTrackFitter.hh"
#include "src/common_cpp/Reconstruction/Global/Particle.hh"
#include "src/common_cpp/Reconstruction/Global/ReconstructionInput.hh"
#include "src/common_cpp/Reconstruction/Global/Track.hh"
#include "src/common_cpp/Reconstruction/Global/TrackFitter.hh"
#include "src/common_cpp/Reconstruction/Global/TrackPoint.hh"
#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"

namespace MAUS {

using MAUS::reconstruction::global::Detector;
using MAUS::reconstruction::global::MinuitTrackFitter;
using MAUS::reconstruction::global::Particle;
using MAUS::reconstruction::global::ReconstructionInput;
using MAUS::reconstruction::global::Track;
using MAUS::reconstruction::global::TrackFitter;
using MAUS::reconstruction::global::TrackPoint;

MapCppGlobalTrackReconstructor::MapCppGlobalTrackReconstructor()
    : optics_model_(NULL), track_fitter_(NULL), reconstruction_input_(NULL) {
}

MapCppGlobalTrackReconstructor::~MapCppGlobalTrackReconstructor() {
  if (reconstruction_input_ != NULL) {
    delete reconstruction_input_;
  }

  if (optics_model_ != NULL) {
    delete optics_model_;
  }

  if (track_fitter_ != NULL) {
    delete track_fitter_;
  }
}

bool MapCppGlobalTrackReconstructor::birth(std::string configuration) {
  // parse the JSON document.
  try {
    configuration_ = JsonWrapper::StringToJson(configuration);
    SetupOpticsModel();
    SetupTrackFitter();
  } catch(Squeal& squee) {
    MAUS::CppErrorHandler::getInstance()->HandleSquealNoJson(
      squee, MapCppGlobalTrackReconstructor::kClassname);
  } catch(std::exception& exc) {
    MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(
      exc, MapCppGlobalTrackReconstructor::kClassname);
  }

  return true;  // Sucessful parsing
}

std::string MapCppGlobalTrackReconstructor::process(std::string run_data) {
  // parse the JSON document.
  try {
    run_data_ = Json::Value(JsonWrapper::StringToJson(run_data));
  } catch(Squeal& squee) {
    MAUS::CppErrorHandler::getInstance()->HandleSquealNoJson(squee, kClassname);
  } catch(std::exception& exc) {
    MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, kClassname);
  }

  // Populate ReconstructionInput instance from JSON data
  Json::Value data_acquisition_mode_names = JsonWrapper::GetProperty(
      configuration_,
      "data_acquisition_modes",
      JsonWrapper::arrayValue);
  Json::Value data_acquisition_mode = JsonWrapper::GetProperty(
      configuration_,
      "data_acquisition_mode",
      JsonWrapper::stringValue);
  if (data_acquisition_mode == "Random") {
    LoadRandomData();
  } else if (data_acquisition_mode == "Testing") {
    LoadTestingData();
  } else if (data_acquisition_mode == "Simulation") {
    LoadSimulationData();
  } else if (data_acquisition_mode == "Live") {
    LoadLiveData();
  } else {
    std::string message = "Invalid data acquisition mode: ";
    message += data_acquisition_mode.asString();
    throw(Squeal(Squeal::recoverable,
                 message,
                 "MapCppGlobalTrackReconstructor::process()"));
  }

  if (reconstruction_input_ == NULL) {
/*
    Json::FastWriter writer;
    std::string output = writer.write(run_data_);
    return output;
*/
    throw(Squeal(Squeal::recoverable,
                 "Null reconstruction input.",
                 "MapCppGlobalTrackReconstructor::process()"));
  }

  // TODO(plane1@hawk.iit.edu) Implement Kalman Filter and TOF track fitters
  //  in addition to Minuit, and select between them based on the configuration

  // associate tracks with individual particles
  std::vector<MAUS::reconstruction::global::Track> tracks;
  CorrelateTrackPoints(tracks);

  int particle_id;
  if (reconstruction_input_->beam_polarity_negative()) {
    particle_id = Particle::kMuMinus;
  } else {
    particle_id = Particle::kMuPlus;
  }

  Json::Value global_tracks;

  // Find the best fit track for each particle traversing the lattice
  MAUS::reconstruction::global::Track best_fit_track;

  for (std::vector<MAUS::reconstruction::global::Track>::const_iterator
          measured_track = tracks.begin();
       measured_track < tracks.end();
       ++measured_track) {
    best_fit_track.set_particle_id(particle_id);
    best_fit_track.clear();
fprintf(stdout, "CHECKPOINT: Before Fit()");
    track_fitter_->Fit(*measured_track, best_fit_track);
fprintf(stdout, "CHECKPOINT: After Fit()");
std::cout << "Measured Track: " << (*measured_track) << std::endl;
std::cout << "Best Fit Track: " << best_fit_track << std::endl;

    // TODO(plane1@hawk.iit.edu) Reconstruct track at the desired locations
    //  specified in the configuration.

    reconstructed_tracks_.push_back(best_fit_track);
    global_tracks.append(TrackToJson(best_fit_track));
  }

  // TODO(plane1@hawk.iit.edu) Update the run data with reconstruction results.
  run_data_["global_tracks"] = global_tracks;

  // pass on the updated run data to the next map in the workflow
  Json::FastWriter writer;
  std::string output = writer.write(run_data_);

  return output;
}

void MapCppGlobalTrackReconstructor::SetupOpticsModel() {
fprintf(stdout, "CHECKPOINT: SetupOpticsModel() 0\n"); fflush(stdout);
  Json::Value optics_model_names = JsonWrapper::GetProperty(
      configuration_,
      "reconstruction_optics_models",
      JsonWrapper::arrayValue);
  Json::Value optics_model_name = JsonWrapper::GetProperty(
      configuration_,
      "reconstruction_optics_model",
      JsonWrapper::stringValue);
  size_t model;
  for (model = 0; model < optics_model_names.size(); ++model) {
    if (optics_model_name == optics_model_names[model]) {
      break;  // leave the current index into optics_model_names in model
    }
  }

fprintf(stdout, "CHECKPOINT: SetupOpticsModel() 1\n"); fflush(stdout);
  switch (model) {
    case 0: {
      // "Differentiating"
      /* TODO(plane1@hawk.iit.edu)
      optics_model_ = new DifferentiatingOpticsModel();
      */
      throw(Squeal(Squeal::nonRecoverable,
                   "DifferentiatingOpticsModel is not yet implemented.",
                   "MapCppGlobalTrackReconstructor::process()"));
      break;
    }
    case 1: {
      // "Integrating"
      /* TODO(plane1@hawk.iit.edu)
      optics_model_ = new IntegratingOpticsModel();
      */
      throw(Squeal(Squeal::nonRecoverable,
                   "IntegratingOpticsModel is not yet implemented.",
                   "MapCppGlobalTrackReconstructor::SetupOpticsModel()()"));
      break;
    }
    case 2: {
fprintf(stdout, "CHECKPOINT SetupOpticsModel() 1.1c\n"); fflush(stdout);
     optics_model_ = new PolynomialOpticsModel(configuration_);
      break;
    }
    case 3: {
      // "Runge Kutta"
      /* TODO(plane1@hawk.iit.edu)
      optics_model_ = new RungeKuttaOpticsModel();
      */
      throw(Squeal(Squeal::nonRecoverable,
                   "RungeKuttaOpticsModel is not yet implemented.",
                   "MapCppGlobalTrackReconstructor::SetupOpticsModel()()"));
      break;
    }
    case 4: {
      // "Linear Approximation"
fprintf(stdout, "CHECKPOINT SetupOpticsModel() 1.1e\n"); fflush(stdout);
     optics_model_ = new LinearApproximationOpticsModel(configuration_);
      break;
    }
    default: {
      std::string message("Unsupported optics model \"");
      message += optics_model_name.asString();
      message += std::string("\" in reconstruction configuration.");
      throw(Squeal(Squeal::nonRecoverable,
                   message.c_str(),
                   "MapCppGlobalTrackReconstructor::SetupOpticsModel()()"));
    }
  }
fprintf(stdout, "CHECKPOINT: SetupOpticsModel() 2\n"); fflush(stdout);
  optics_model_->Build();
fprintf(stdout, "CHECKPOINT: SetupOpticsModel() 3\n"); fflush(stdout);
}

void MapCppGlobalTrackReconstructor::SetupTrackFitter() {
  Json::Value fitter_names = JsonWrapper::GetProperty(
      configuration_,
      "reconstruction_track_fitters",
      JsonWrapper::arrayValue);
  Json::Value fitter_name = JsonWrapper::GetProperty(
      configuration_,
      "reconstruction_track_fitter",
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
      double start_plane = optics_model_->start_plane();
      track_fitter_ = new MinuitTrackFitter(*optics_model_, start_plane);
      break;
    }
    case 1: {
      // Kalman Filter
      /* TODO(plane1@hawk.iit.edu)
      track_fitter_ = new KalmanFilterTrackFitter(optics_model_);
      */
      throw(Squeal(Squeal::nonRecoverable,
                   "KalmanFilterTrackFitter is not yet implemented.",
                   "MapCppGlobalTrackReconstructor::SetupTrackFitter()"));
      break;
    }
    case 2: {
      // TOF
      /* TODO(plane1@hawk.iit.edu)
      track_fitter_ = new TOFTrackFitter(optics_model_);
      */
      throw(Squeal(Squeal::nonRecoverable,
                   "TOFTrackFitter is not yet implemented.",
                   "MapCppGlobalTrackReconstructor::SetupTrackFitter()"));
      break;
    }
    default: {
      std::string message("Unsupported track fitter \"");
      message += fitter_name.asString();
      message += std::string("\" in reconstruction configuration.");
      throw(Squeal(Squeal::nonRecoverable,
                   message.c_str(),
                   "MapCppGlobalTrackReconstructor::SetupTrackFitter()"));
    }
  }
}

void MapCppGlobalTrackReconstructor::LoadRandomData() {
  // Create random track points for TOF0, TOF1, Tracker 1, Tracker 2, and TOF 2
  srand((unsigned)time(NULL));

  const bool beam_polarity_negative = false;  // mu+

  std::vector<Detector> detectors;
  const double mass = Particle::GetInstance()->GetMass(Particle::kMuMinus);
  double plane;
  double uncertainty_data[36];
  std::vector<TrackPoint> events;
  double position[3], momentum[3];

  // generate mock detector info and random muon detector event data
  for (size_t id = Detector::kTOF0; id <= Detector::kCalorimeter; ++id) {
    // plane = ((double)rand()/(double)RAND_MAX) * 20;  // 0.0 - 20.0 meters
    plane = id * 1.4;  // 0.0 - 20.0 meters
    for (int index = 0; index < 36; ++index) {
      uncertainty_data[index] = static_cast<double>(rand()) / RAND_MAX
                              * 100.0;
    }
    CovarianceMatrix uncertainties(uncertainty_data);
    Detector detector(id, plane, uncertainties);
    detectors.push_back(detector);

    // skip detectors we're not using
    if ((id == Detector::kCherenkov1) ||
        (id == Detector::kCherenkov2) ||
        (id == Detector::kCalorimeter)) {
      continue;
    }

    for (int coordinate = 0; coordinate < 3; ++coordinate) {
      position[coordinate] = static_cast<double>(rand()) / RAND_MAX
                           * 20.0;  // meters
    }

    for (int coordinate = 0; coordinate < 3; ++coordinate) {
      momentum[coordinate] = static_cast<double>(rand()) / RAND_MAX
                           * 500.0;  // MeV
    }

    // force the positions to be monotonically increasing
    if (!events.empty()) {
      TrackPoint const & last_point = events.back();
      position[0] += last_point.x();
      position[1] += last_point.y();
      position[2] += last_point.z();
    }

    TrackPoint track_point(position[0], momentum[0]/100.0,
                           position[1], momentum[1]/100.0,
                           position[2], momentum[2],
                           detector,
                           PhaseSpaceVector::PhaseSpaceType::kPositional);
    TrackPoint temporal_track_point(
        track_point, mass,
        PhaseSpaceVector::PhaseSpaceType::kPositional);
    events.push_back(temporal_track_point);
  }


  reconstruction_input_ = new ReconstructionInput(beam_polarity_negative,
                                                  detectors,
                                                  events);
}

void MapCppGlobalTrackReconstructor::LoadTestingData() {
  // Load some pre-generated MC data with TOF and Tracker hits

  bool beam_polarity_negative = false;
  std::vector<Detector> detectors;
  std::vector<TrackPoint> events;

  try {
    Json::Value testing_data = JsonWrapper::GetProperty(
        configuration_,"testing_data", JsonWrapper::objectValue);

    const Json::Value beam_polarity_negative_json = JsonWrapper::GetProperty(
        testing_data, "beam_polarity_negative", JsonWrapper::booleanValue);
    beam_polarity_negative = beam_polarity_negative_json.asBool();

    // *** Get detector info ***
    const Json::Value detectors_json = JsonWrapper::GetProperty(
        testing_data, "detectors", JsonWrapper::arrayValue);
    const Json::Value::UInt detector_count = detectors_json.size();
    for (Json::Value::UInt index = 0; index < detector_count; ++index) {
      const Json::Value detector_json = detectors_json[index];
      const Json::Value id_json = JsonWrapper::GetProperty(
          detector_json, "id", JsonWrapper::intValue);
      const int id = id_json.asInt();

      const Json::Value plane_json = JsonWrapper::GetProperty(
          detector_json, "plane", JsonWrapper::realValue);
      const double plane = plane_json.asDouble();

      const Json::Value uncertainties_json = JsonWrapper::GetProperty(
          detector_json, "uncertainties", JsonWrapper::arrayValue);
      const CovarianceMatrix uncertainties
          = GetJsonCovarianceMatrix(uncertainties_json);

      const Detector detector(id, plane, uncertainties);
      detectors.push_back(detector);
    }

    const Json::Value mc_events = JsonWrapper::GetProperty(
        testing_data, "mc_events", JsonWrapper::arrayValue);
    const Json::Value mc_event = mc_events[Json::Value::UInt(0)];

    const Json::Value sci_fi_hits = JsonWrapper::GetProperty(
        mc_event, "sci_fi_hits", JsonWrapper::arrayValue);
    const size_t sci_fi_hit_count = sci_fi_hits.size(); 
    for (size_t index = 0; index < sci_fi_hit_count; ++index) {
      const Json::Value sci_fi_hit = sci_fi_hits[index];

      double coordinates[6];

      const Json::Value time = JsonWrapper::GetProperty(
          sci_fi_hit, "time", JsonWrapper::realValue);

      const Json::Value position_json = JsonWrapper::GetProperty(
          sci_fi_hit, "position", JsonWrapper::objectValue);
      const Json::Value x = JsonWrapper::GetProperty(
          position_json, "x", JsonWrapper::realValue);
      coordinates[0] = x.asDouble();
      const Json::Value y = JsonWrapper::GetProperty(
          position_json, "y", JsonWrapper::realValue);
      coordinates[2] = y.asDouble();
      const Json::Value z = JsonWrapper::GetProperty(
          position_json, "z", JsonWrapper::realValue);
      coordinates[4] = z.asDouble();

      const Json::Value energy = JsonWrapper::GetProperty(
          sci_fi_hit, "energy", JsonWrapper::realValue);

      const Json::Value momentum_json = JsonWrapper::GetProperty(
          sci_fi_hit, "momentum", JsonWrapper::objectValue);
      const Json::Value px = JsonWrapper::GetProperty(
          momentum_json, "x", JsonWrapper::realValue);
      coordinates[1] = px.asDouble();
      const Json::Value py = JsonWrapper::GetProperty(
          momentum_json, "y", JsonWrapper::realValue);
      coordinates[3] = py.asDouble();
      const Json::Value pz = JsonWrapper::GetProperty(
          momentum_json, "z", JsonWrapper::realValue);
      coordinates[5] = pz.asDouble();

      TrackPoint track_point(coordinates,
                             PhaseSpaceVector::PhaseSpaceType::kPositional);

      const Json::Value channel_id = JsonWrapper::GetProperty(
          sci_fi_hit, "channel_id", JsonWrapper::objectValue);
      const Json::Value tracker_number_json = JsonWrapper::GetProperty(
          channel_id, "tracker_number", JsonWrapper::intValue);
      const size_t tracker_number = tracker_number_json.asUInt();
      const Json::Value station_number_json = JsonWrapper::GetProperty(
          channel_id, "station_number", JsonWrapper::intValue);
      const size_t station_number = tracker_number_json.asUInt();
      const size_t detector_id = tracker_number + 4 + station_number;

      track_point.set_detector_id(detector_id);

      // cheat a little since all of the uncertainties are the same at present
      track_point.set_uncertainties(detectors[0].uncertainties());

      events.push_back(track_point);
    }

    const Json::Value tof_hits = JsonWrapper::GetProperty(
        mc_event, "tof_hits", JsonWrapper::arrayValue);
    const size_t tof_hit_count = tof_hits.size(); 
    for (size_t index = 0; index < tof_hit_count; ++index) {
      const Json::Value tof_hit = tof_hits[index];

      double coordinates[6];

      const Json::Value time = JsonWrapper::GetProperty(
          tof_hit, "time", JsonWrapper::realValue);

      const Json::Value position_json = JsonWrapper::GetProperty(
          tof_hit, "position", JsonWrapper::objectValue);
      const Json::Value x = JsonWrapper::GetProperty(
          position_json, "x", JsonWrapper::realValue);
      coordinates[0] = x.asDouble();
      const Json::Value y = JsonWrapper::GetProperty(
          position_json, "y", JsonWrapper::realValue);
      coordinates[2] = y.asDouble();
      const Json::Value z = JsonWrapper::GetProperty(
          position_json, "z", JsonWrapper::realValue);
      coordinates[4] = z.asDouble();

      const Json::Value energy = JsonWrapper::GetProperty(
          tof_hit, "energy", JsonWrapper::realValue);

      const Json::Value momentum_json = JsonWrapper::GetProperty(
          tof_hit, "momentum", JsonWrapper::objectValue);
      const Json::Value px = JsonWrapper::GetProperty(
          momentum_json, "x", JsonWrapper::realValue);
      coordinates[2] = px.asDouble();
      const Json::Value py = JsonWrapper::GetProperty(
          momentum_json, "y", JsonWrapper::realValue);
      coordinates[3] = py.asDouble();
      const Json::Value pz = JsonWrapper::GetProperty(
          momentum_json, "z", JsonWrapper::realValue);
      coordinates[5] = pz.asDouble();

      TrackPoint track_point(coordinates,
                             PhaseSpaceVector::PhaseSpaceType::kPositional);

      const Json::Value channel_id = JsonWrapper::GetProperty(
          tof_hit, "channel_id", JsonWrapper::objectValue);
      const Json::Value station_number_json = JsonWrapper::GetProperty(
          channel_id, "station_number", JsonWrapper::intValue);
      const size_t station_number = station_number_json.asUInt();
      switch (station_number) {
       case 0: track_point.set_detector_id(Detector::kTOF0); break;
       case 1: track_point.set_detector_id(Detector::kTOF1); break;
       case 2: track_point.set_detector_id(Detector::kTOF2); break;
       default: track_point.set_detector_id(Detector::kNone); break;
      }
 
      // cheat a little since all of the uncertainties are the same at present
      track_point.set_uncertainties(detectors[0].uncertainties());

      events.push_back(track_point);
    }
  } catch(Squeal& squee) {
    run_data_ = MAUS::CppErrorHandler::getInstance()->HandleSqueal(
        run_data_, squee, "MAUS::MapCppGlobalTrackReconstructor");
  } catch(std::exception& exc) {
    run_data_ = MAUS::CppErrorHandler::getInstance()->HandleStdExc(
        run_data_, exc, "MAUS::MapCppGlobalTrackReconstructor");
  }

  // TODO(plan1@hawk.iit.edu) do an insertion sort instead of sorting afterwards
  std::sort(events.begin(), events.end());  // sort in chronological order

  reconstruction_input_ = new ReconstructionInput(beam_polarity_negative,
                                                  detectors,
                                                  events);
}

void MapCppGlobalTrackReconstructor::LoadSimulationData() {
  // TODO(plane1@hawk.iit.edu) use "digitized" data (simulated DAQ output)
  // instead of Monte Carlo truth data

  try {
    // MapCppSimulation puts the simulated spill in run_data_["mc"]
    Json::Value mc = JsonWrapper::GetProperty(run_data_,
                                              "mc",
                                              JsonWrapper::arrayValue);

    // TODO(plane1@hawk.iit.edu) Collect tracks from the detectors and
    // dynamically allocate the ReconstructionInput instance
    // reconstruction_input_ = new ReconstructionInput(...);
  } catch(Squeal& squee) {
    run_data_ = MAUS::CppErrorHandler::getInstance()->HandleSqueal(
        run_data_, squee, "MAUS::MapCppGlobalTrackReconstructor");
  } catch(std::exception& exc) {
    run_data_ = MAUS::CppErrorHandler::getInstance()->HandleStdExc(
        run_data_, exc, "MAUS::MapCppGlobalTrackReconstructor");
  }
}

void MapCppGlobalTrackReconstructor::LoadLiveData() {
  // reconstruction_input_ = new ReconstructionInput(...);
}


void MapCppGlobalTrackReconstructor::CorrelateTrackPoints(
    std::vector<Track> & tracks) {
  const std::vector<TrackPoint> & track_points
      = reconstruction_input_->events();

  // TODO(plane1@hawk.iit.edu) create sets of events where each set corresponds
  // to a different particle. For now assume that all events are from a single
  // particle.
  tracks.push_back(Track(track_points, Particle::kNone));
}

bool MapCppGlobalTrackReconstructor::death() {
  return true;  // successful
}

CovarianceMatrix const MapCppGlobalTrackReconstructor::GetJsonCovarianceMatrix(
    Json::Value const & value) {
  if (value.size() < static_cast<Json::Value::UInt>(6)) {
    throw(Squeal(Squeal::recoverable,
                 "Not enough row elements to convert JSON to CovarianceMatrix",
                 "MapCppGlobalTrackReconstructor::GetJsonCovarianceMatrix()"));
  }

  const size_t size = 6;
  double matrix_data[size*size];
  for (size_t row = 0; row < 6; ++row) {
    const Json::Value row_json = value[row];
    if (row_json.size() < static_cast<Json::Value::UInt>(6)) {
      throw(Squeal(
          Squeal::recoverable,
          "Not enough column elements to convert JSON to CovarianceMatrix",
          "MapCppGlobalTrackReconstructor::GetJsonCovarianceMatrix()"));
    }
    for (size_t column = 0; column < 6; ++column) {
      const Json::Value element_json = row_json[column];
      matrix_data[row*size+column] = element_json.asDouble();
    }
  }

  return CovarianceMatrix(matrix_data);
}

Json::Value MapCppGlobalTrackReconstructor::TrackToJson(const Track & track) {
  Json::Value track_node;

  // track points
  Json::Value track_points;
  for (size_t index = 0; index < track.size(); ++index) {
    track_points.append(TrackPointToJson(track[index]));
  }
  track_node["track_points"] = track_points;

  // particle ID
  track_node["PID"] = Json::Value(track.particle_id());

  return track_node;
}

Json::Value MapCppGlobalTrackReconstructor::TrackPointToJson(
    const TrackPoint & track_point) {
  Json::Value track_point_node;

  // coordinates
  Json::Value coordinates;
  coordinates["t"] = track_point.t();
  coordinates["E"] = track_point.E();
  coordinates["x"] = track_point.x();
  coordinates["Px"] = track_point.Px();
  coordinates["y"] = track_point.y();
  coordinates["Py"] = track_point.Py();
  coordinates["z"] = track_point.z();
  coordinates["Pz"] = track_point.Pz();
  coordinates["Pz"] = track_point.Pz();
  track_point_node["coordinates"] = coordinates;

  // detector ID
  track_point_node["detector_id"] = Json::Value(track_point.detector_id());

  // uncertainty matrix
  Json::Value uncertainties;
  size_t size = track_point.uncertainties().size();
  for (size_t row = 1; row <= size; ++row) {
    Json::Value row_node;
    for (size_t column = 1; column <= size; ++column) {
      row_node.append(Json::Value(track_point.uncertainties()(row, column)));
    }
    uncertainties.append(row_node);
  }
  track_point_node["uncertainties"] = uncertainties;

  return track_point_node;
}

const std::string MapCppGlobalTrackReconstructor::kClassname
  = "MapCppGlobalTrackReconstructor";

}  // namespace MAUS

