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

void MapCppGlobalTrackReconstructor::SetupOpticsModel() {
fprintf(stdout, "CHECKPOINT: SetupOpticsModel() 0\n");
fflush(stdout);
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

fprintf(stdout, "CHECKPOINT: SetupOpticsModel() 1\n");
fflush(stdout);
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
fprintf(stdout, "CHECKPOINT SetupOpticsModel() 1.1c\n");
fflush(stdout);
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
fprintf(stdout, "CHECKPOINT SetupOpticsModel() 1.1e\n");
fflush(stdout);
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
fprintf(stdout, "CHECKPOINT: SetupOpticsModel() 2\n");
fflush(stdout);
  optics_model_->Build();
fprintf(stdout, "CHECKPOINT: SetupOpticsModel() 3\n");
fflush(stdout);
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
      double start_plane = optics_model_->first_plane();
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

std::string MapCppGlobalTrackReconstructor::process(std::string run_data) {
  // parse the JSON document.
  try {
    run_data_ = Json::Value(JsonWrapper::StringToJson(run_data));
  } catch(Squeal& squee) {
    MAUS::CppErrorHandler::getInstance()->HandleSquealNoJson(squee, kClassname);
  } catch(std::exception& exc) {
    MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, kClassname);
  }
std::cout << "DEBUG process(): Run Data = " << std::endl << run_data_ << std::endl;

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
  } else if (data_acquisition_mode == "Smeared") {
    LoadSmearedData();
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

std::cout << "CHECKPOINT process(): 279" << std::endl;
std::cout.flush();
  int particle_id;
  if (reconstruction_input_->beam_polarity_negative()) {
    particle_id = Particle::kMuMinus;
  } else {
    particle_id = Particle::kMuPlus;
  }

  Json::Value global_tracks;

  // Find the best fit track for each particle traversing the lattice
std::cout << "CHECKPOINT process(): 290" << std::endl;
std::cout.flush();
  size_t track_count = 0;
  for (std::vector<MAUS::reconstruction::global::Track>::const_iterator
          measured_track = tracks.begin();
       measured_track < tracks.end();
       ++measured_track) {
    MAUS::reconstruction::global::Track best_fit_track(particle_id);

    track_fitter_->Fit(*measured_track, best_fit_track);
std::cout << "Best Fit Track: " << best_fit_track << std::endl;
    // TODO(plane1@hawk.iit.edu) Reconstruct track at the desired locations
    //  specified in the configuration.

    global_tracks.append(TrackToJson(best_fit_track));
    track_count += best_fit_track.size() - 1;
  }

std::cout << "DEBUG process(): # particles = " << global_tracks.size()
          << "\t# track points = " << track_count << std::endl;
std::cout.flush();

  // TODO(plane1@hawk.iit.edu) Update the run data with reconstruction results.
  run_data_["global_tracks"] = global_tracks;

  // pass on the updated run data to the next map in the workflow
  Json::FastWriter writer;
  std::string output = writer.write(run_data_);
std::cout << "CHECKPOINT process(): 315" << std::endl;
std::cout.flush();

  return output;
}

void MapCppGlobalTrackReconstructor::LoadRandomData() {
  // Create random track points for TOF0, TOF1, Tracker 1, Tracker 2, and TOF 2
  srand((unsigned)time(NULL));

  const bool beam_polarity_negative = false;  // mu+

  std::map<int, Detector> detectors;
  double plane;
  double uncertainty_data[36];
  std::vector<TrackPoint> events;
  double position[3], momentum[3];

  // generate mock detector info and random muon detector event data
  for (size_t id = Detector::kTOF0_1; id <= Detector::kCalorimeter; ++id) {
    // plane = ((double)rand()/(double)RAND_MAX) * 20;  // 0.0 - 20.0 meters
    plane = id * 1.4;  // 0.0 - 20.0 meters
    for (int index = 0; index < 36; ++index) {
      uncertainty_data[index] = static_cast<double>(rand()) / RAND_MAX
                              * 100.0;
    }
    CovarianceMatrix uncertainties(uncertainty_data);
    Detector detector(id, plane, uncertainties);
    detectors.insert(std::pair<int, Detector>(id, detector));

    // skip detectors we're not using
    if ((id == Detector::kCherenkov1) ||
        (id == Detector::kCherenkov2) ||
        (id == Detector::kCalorimeter)) {
      continue;
    }

    for (int coordinate = 0; coordinate < 3; ++coordinate) {
      position[coordinate] = static_cast<double>(rand()) / RAND_MAX
                           * 20.0;  // ns or mm
    }

    for (int coordinate = 0; coordinate < 3; ++coordinate) {
      momentum[coordinate] = static_cast<double>(rand()) / RAND_MAX
                           * 500.0;  // MeV(/c)
    }

    // force the positions to be monotonically increasing
    if (!events.empty()) {
      TrackPoint const & last_point = events.back();
      position[0] += last_point.t();
      position[1] += last_point.x();
      position[2] += last_point.y();
    }

    TrackPoint track_point(position[0], momentum[0]/100.0,
                           position[1], momentum[1]/100.0,
                           position[2], momentum[2],
                           detector);
    events.push_back(track_point);
  }


  reconstruction_input_ = new ReconstructionInput(beam_polarity_negative,
                                                  detectors,
                                                  events);
}

void MapCppGlobalTrackReconstructor::LoadTestingData() {
  // Load some pre-generated MC data with TOF and Tracker hits

  bool beam_polarity_negative = false;
  std::map<int, Detector> detectors;
  std::vector<TrackPoint> events;

  try {
    Json::Value testing_data = JsonWrapper::GetProperty(
        configuration_, "testing_data", JsonWrapper::objectValue);

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
      detectors.insert(std::pair<int, Detector>(id, detector));
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

      TrackPoint track_point(coordinates);

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
      std::map<int, Detector>::const_iterator detector
        = detectors.find(detector_id);
      track_point.set_uncertainties(detector->second.uncertainties());

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
      coordinates[0] = time.asDouble();
      const Json::Value position_json = JsonWrapper::GetProperty(
          tof_hit, "position", JsonWrapper::objectValue);
      const Json::Value x = JsonWrapper::GetProperty(
          position_json, "x", JsonWrapper::realValue);
      coordinates[2] = x.asDouble();
      const Json::Value y = JsonWrapper::GetProperty(
          position_json, "y", JsonWrapper::realValue);
      coordinates[4] = y.asDouble();
      const Json::Value z = JsonWrapper::GetProperty(
          position_json, "z", JsonWrapper::realValue);

      const Json::Value energy = JsonWrapper::GetProperty(
          tof_hit, "energy", JsonWrapper::realValue);
      coordinates[1] = energy.asDouble();
      const Json::Value momentum_json = JsonWrapper::GetProperty(
          tof_hit, "momentum", JsonWrapper::objectValue);
      const Json::Value px = JsonWrapper::GetProperty(
          momentum_json, "x", JsonWrapper::realValue);
      coordinates[3] = px.asDouble();
      const Json::Value py = JsonWrapper::GetProperty(
          momentum_json, "y", JsonWrapper::realValue);
      coordinates[5] = py.asDouble();

      TrackPoint track_point(coordinates, z.asDouble());

      const Json::Value channel_id = JsonWrapper::GetProperty(
          tof_hit, "channel_id", JsonWrapper::objectValue);
      const Json::Value station_number_json = JsonWrapper::GetProperty(
          channel_id, "station_number", JsonWrapper::intValue);
      const size_t station_number = station_number_json.asUInt();
      // FIXME set plane
      const size_t plane = 0;
      int detector_id = Detector::kNone;
      switch (station_number) {
       case 0: detector_id = Detector::kTOF0_1 + plane; break;
       case 1: detector_id = Detector::kTOF1_1 + plane; break;
       case 2: detector_id = Detector::kTOF2_1 + plane; break;
      }

      track_point.set_detector_id(detector_id);
      std::map<int, Detector>::const_iterator detector
        = detectors.find(detector_id);
      track_point.set_uncertainties(detector->second.uncertainties());

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
  LoadSimulationData("mc_events");
}

void MapCppGlobalTrackReconstructor::LoadSmearedData() {
  LoadSimulationData("mc_smeared");
}

void MapCppGlobalTrackReconstructor::LoadSimulationData(
    const std::string mc_branch_name) {
  bool beam_polarity_negative = false;
  std::map<int, Detector> detectors;
  std::vector<TrackPoint> events;

  MAUSGeant4Manager * const simulator = MAUSGeant4Manager::GetInstance();
  MAUSPrimaryGeneratorAction::PGParticle reference_pgparticle
    = simulator->GetReferenceParticle();
  int charge = Particle::GetInstance()->GetCharge(reference_pgparticle.pid);
  if (charge < 0) {
    beam_polarity_negative = true;
  } else if (charge == 0) {
    throw(Squeal(Squeal::recoverable,
                  "Reference particle has no charge. Assuming a positive beam.",
                  "MapCppGlobalTrackReconstructor::LoadSimulationData()"));
  }

  LoadDetectorConfiguration(detectors);
  LoadMonteCarloData(mc_branch_name, events, detectors);
std::cout << "DEBUG LoadSimulationData(): # events = " << events.size() << std::endl;
  reconstruction_input_ = new ReconstructionInput(beam_polarity_negative,
                                                  detectors,
                                                  events);
}

void MapCppGlobalTrackReconstructor::LoadDetectorConfiguration(
    std::map<int, Detector> & detectors) {
  try {
    // FIXME(plane1@hawk.iit.edu) Once the detector groups provide this
    // information this will need to be changed
    Json::Value testing_data = JsonWrapper::GetProperty(
        configuration_, "testing_data", JsonWrapper::objectValue);

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
std::cout << "DEBUG LoadDetectorConfiguration(): Detector id = " << detector.id() << std::endl;
      detectors.insert(std::pair<int, Detector>(id, detector));
    }
  } catch(Squeal& squee) {
    run_data_ = MAUS::CppErrorHandler::getInstance()->HandleSqueal(
        run_data_, squee,
        "MAUS::MapCppGlobalTrackReconstructor::LoadMonteCarloData()");
  } catch(std::exception& exc) {
    run_data_ = MAUS::CppErrorHandler::getInstance()->HandleStdExc(
        run_data_, exc,
        "MAUS::MapCppGlobalTrackReconstructor::LoadMonteCarloData()");
  }
}

void MapCppGlobalTrackReconstructor::LoadMonteCarloData(
    const std::string               branch_name,
    std::vector<TrackPoint> &       events,
    const std::map<int, Detector> & detectors) {
  try {
    // MC event branch
    Json::Value testing_data = JsonWrapper::GetProperty(
        configuration_, "testing_data", JsonWrapper::objectValue);
    const Json::Value mc_events = JsonWrapper::GetProperty(
        testing_data, branch_name, JsonWrapper::arrayValue);
        // run_data_, branch_name, JsonWrapper::arrayValue);
std::cout << "DEBUG LoadMonteCarloData(): # particles = " << mc_events.size()
          << std::endl;
    for (Json::Value::UInt particle_index = Json::Value::UInt(0);
         particle_index < mc_events.size();
         ++particle_index) {
      const Json::Value mc_event = mc_events[particle_index];
      std::vector<std::string> hit_group_names = mc_event.getMemberNames();
      std::vector<std::string>::const_iterator hit_group_name;
      for (hit_group_name = hit_group_names.begin();
           hit_group_name != hit_group_names.end();
           ++hit_group_name) {
std::cout << "DEBUG LoadMonteCarloData(): Hit Group Name = " << *hit_group_name
          << std::endl;
        const Json::Value hit_group = mc_event[*hit_group_name];
        // if (!hit_group.isArray()) {
        if (*hit_group_name != "tof_hits") {
std::cout << "DEBUG LoadMonteCarloData(): Is Array = " << hit_group.isArray()
          << std::endl;
          // primary hit, not hit group
          continue;
        }

std::cout << "DEBUG LoadMonteCarloData(): # Hits = " << hit_group.size()
          << std::endl;
        for (Json::Value::const_iterator hit = hit_group.begin();
              hit != hit_group.end();
              ++hit) {
          const Json::Value particle_id = JsonWrapper::GetProperty(
              *hit, "particle_id", JsonWrapper::intValue);

          double coordinates[6];

          const Json::Value time = JsonWrapper::GetProperty(
              *hit, "time", JsonWrapper::realValue);
          coordinates[0] = time.asDouble();
          const Json::Value position_json = JsonWrapper::GetProperty(
              *hit, "position", JsonWrapper::objectValue);
          const Json::Value x = JsonWrapper::GetProperty(
              position_json, "x", JsonWrapper::realValue);
          coordinates[2] = x.asDouble();
          const Json::Value y = JsonWrapper::GetProperty(
              position_json, "y", JsonWrapper::realValue);
          coordinates[4] = y.asDouble();
          const Json::Value z = JsonWrapper::GetProperty(
              position_json, "z", JsonWrapper::realValue);

          const Json::Value energy = JsonWrapper::GetProperty(
              *hit, "energy", JsonWrapper::realValue);
          coordinates[1] = energy.asDouble();
          const Json::Value momentum_json = JsonWrapper::GetProperty(
              *hit, "momentum", JsonWrapper::objectValue);
          const Json::Value px = JsonWrapper::GetProperty(
              momentum_json, "x", JsonWrapper::realValue);
          coordinates[3] = px.asDouble();
          const Json::Value py = JsonWrapper::GetProperty(
              momentum_json, "y", JsonWrapper::realValue);
          coordinates[5] = py.asDouble();

          TrackPoint track_point(
            time.asDouble(), energy.asDouble(),
            x.asDouble(), px.asDouble(),
            y.asDouble(), py.asDouble(),
            particle_id.asInt(), z.asDouble());

          const Json::Value channel_id = JsonWrapper::GetProperty(
              *hit, "channel_id", JsonWrapper::objectValue);
          size_t detector_id = Detector::kNone;
          if ((*hit_group_name) == "sci_fi_hits") {
            const Json::Value tracker_number_json = JsonWrapper::GetProperty(
                channel_id, "tracker_number", JsonWrapper::intValue);
            const size_t tracker_number = tracker_number_json.asUInt();
            const Json::Value station_number_json = JsonWrapper::GetProperty(
                channel_id, "station_number", JsonWrapper::intValue);
            const size_t station_number = tracker_number_json.asUInt();
            // FIXME detector IDs have changed
            detector_id = tracker_number + 4 + station_number;
          } else if ((*hit_group_name) == "tof_hits") {
            const Json::Value station_number_json = JsonWrapper::GetProperty(
                channel_id, "station_number", JsonWrapper::intValue);
            const size_t station_number = station_number_json.asUInt();
            const Json::Value plane_json = JsonWrapper::GetProperty(
                channel_id, "plane", JsonWrapper::intValue);
            const size_t plane = plane_json.asUInt();
            switch (station_number) {
              case 0: detector_id = Detector::kTOF0_1 + plane; break;
              case 1: detector_id = Detector::kTOF1_1 + plane; break;
              case 2: detector_id = Detector::kTOF2_2 + plane; break;
            }
          }
          track_point.set_detector_id(detector_id);
          std::map<int, Detector>::const_iterator detector
            = detectors.find(detector_id);
          track_point.set_uncertainties(detector->second.uncertainties());

          events.push_back(track_point);
std::cout << "DEBUG LoadMonteCarloData(): # Events = " << events.size()
          << std::endl;
        }
      }
    }
  } catch(Squeal& squee) {
    run_data_ = MAUS::CppErrorHandler::getInstance()->HandleSqueal(
        run_data_, squee,
        "MAUS::MapCppGlobalTrackReconstructor::LoadMonteCarloData()");
  } catch(std::exception& exc) {
    run_data_ = MAUS::CppErrorHandler::getInstance()->HandleStdExc(
        run_data_, exc,
        "MAUS::MapCppGlobalTrackReconstructor::LoadMonteCarloData()");
  }

  // TODO(plan1@hawk.iit.edu) do an insertion sort instead of sorting afterwards
  std::sort(events.begin(), events.end());  // sort in chronological order
}

void MapCppGlobalTrackReconstructor::LoadLiveData() {
  // reconstruction_input_ = new ReconstructionInput(...);
}


void MapCppGlobalTrackReconstructor::CorrelateTrackPoints(
    std::vector<Track> & tracks) {
std::cout << "CHECKPOINT CorrelateTrackPoints(): 0" << std::endl;
  const std::vector<TrackPoint> & track_points
      = reconstruction_input_->events();
std::cout << "DEBUG CorrelateTrackPoints(): # events = " << track_points.size() << std::endl;

  Track track;
  size_t last_detector = Detector::kNone;
  for (std::vector<TrackPoint>::const_iterator event = track_points.begin();
       event != track_points.end();
       ++event) {
    const double muon_mass
      = Particle::GetInstance()->GetMass(Particle::kMuMinus);
    if (event->energy() < muon_mass) {
      // reject particles with energy < muon mass (electrons)
      continue;
    } else if (event->detector_id() < last_detector) {
      tracks.push_back(track);
      track.clear();
      track.push_back(*event);
    } else {
      track.push_back(*event);
    }
    last_detector = event->detector_id();
  }
std::cout << "CHECKPOINT CorrelateTrackPoints(): 1" << std::endl;
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
  Json::Value position;
  position["x"] = track_point.x();
  position["y"] = track_point.y();
  position["z"] = track_point.z();
std::cout << "Final Z: " << track_point.z() << std::endl;
  track_point_node["position"] = position;

  Json::Value momentum;
  momentum["x"] = track_point.Px();
  momentum["y"] = track_point.Py();
  momentum["z"] = track_point.Pz();
  track_point_node["momentum"] = momentum;

  track_point_node["time"] = track_point.t();
  track_point_node["energy"] = track_point.E();

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

