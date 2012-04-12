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

#include "MapCppTrackReconstructor/MapCppTrackReconstructor.hh"

// C++
#include <vector>
#include <string>

// External
#include "TMinuit.h"
#include "json/json.h"

// MAUS
#include "src/common_cpp/Optics/CovarianceMatrix.hh"
#include "src/common_cpp/Optics/LeastSquaresOpticsModel.hh"
#include "src/common_cpp/Optics/LinearApproximationOpticsModel.hh"
#include "src/common_cpp/Reconstruction/Detector.hh"
#include "src/common_cpp/Reconstruction/Particle.hh"
#include "src/common_cpp/Reconstruction/ReconstructionInput.hh"
#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"

// Legacy/G4MICE
#include "Interface/Squeal.hh"
#include "Interface/dataCards.hh"

namespace MAUS {

MapCppTrackReconstructor::MapCppTrackReconstructor()
    : reconstruction_input_(NULL) {
}

MapCppTrackReconstructor::~MapCppTrackReconstructor() {
  if (reconstruction_input_ ~= NULL) {
    delete reconstruction_input_;
  }
}

bool MapCppTrackReconstructor::birth(std::string configuration) {
  // parse the JSON document.
  try {
    configuration_ = new Json::Value(JsonWrapper::StringToJson(configuration));
    SetupOpticsModel();
    SetupTrajectoryFitter();
    return true;  // Sucessful parsing
  } catch(Squeal& squee) {
    MAUS::CppErrorHandler::getInstance()->HandleSquealNoJson(squee, _classname);
  } catch(std::exception& exc) {
    MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, _classname);
  }
  return false;
}

std::string MapCppTrackReconstructor::process(std::string run_data) {
  // parse the JSON document.
  try {
    run_data_ = new Json::Value(JsonWrapper::StringToJson(run_data));
  } catch(Squeal& squee) {
    MAUS::CppErrorHandler::getInstance()->HandleSquealNoJson(squee, _classname);
  } catch(std::exception& exc) {
    MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, _classname);
  }

  // Populate ReconstructionInput instance from JSON data
  if (run_data_.isMember("ReconstructionTestingData")) {
    LoadTestingData();
  } else if (run_data_.isMember("mc")) {
    LoadSimulationData();
  } else {
    LoadLiveData();
  }

  // TODO(plane1@hawk.iit.edu) Implement Kalman Filter and TOF track fitters
  //  in addition to Minuit, and select between them based on the configuration
 
  // associate tracks with individual particles
  std::vector<std::vector<ParticleTrack const *> > track_sets;
  CorrelateParticleTracks(&track_sets);

  // Find the best fit trajectory for each particle traversing the lattice
  std::vector<std::vector<ParticleTrack const *> >::const_iterator
                                                          measured_trajectories;
  int particle_ids[] = {
  for (trajectories = track_sets.begin();
       trajectories < track_sets.end();
       ++trajectories) {
    ParticleTrajectory best_fit_trajectory(particle_id);
    trajectory_fitter_->Fit(&(*measured_trajectories), &best_fit_trajectory);

    // TODO(plane1@hawk.iit.edu) Reconstruct tracks at the desired locations
    //  specified in the configuration.

    trajectories_.push_back(best_fit_trajectory);
  }

  // clean up
  delete optics_model_;
  delete trajectory_fitter_;

  // pass on the updated run data to the next map in the workflow
  Json::FastWriter writer;
  std::string output = writer.write(run_data_);

  return output;
}

MapCppTrackReconstructor::SetupOpticsModel() {
  Json::Value optics_model_names = JsonWrapper::GetProperty(
      configuration_,
      "reconstruction_optics_models",
      JsonWrapper::stringValue);
  Json::Value optics_model_name = JsonWrapper::GetProperty(
      configuration_,
      "reconstruction_optics_model",
      JsonWrapper::stringValue);
  int model;
  for (model = 0; model < optics_model_names.size(); ++model) {
    if (optics_model_name == optics_model_names[model]) {
      break;  // leave the current index into optics_model_names in model
    }
  }
  
  switch (model) {
    case 0: {
      // "Differentiating"
      /* TODO(plane1@hawk.iit.edu)
      optics_model_ = new DifferentiatingOpticsModel();
      */
      throw(Squeal(Squeal::nonRecoverable,
                   "DifferentiatingOpticsModel is not yet implemented.",
                   "MapCppTrackReconstructor::process()"));
      break;
    }
    case 1: {
      // "Integrating"
      /* TODO(plane1@hawk.iit.edu)
      optics_model_ = new IntegratingOpticsModel();
      */
      throw(Squeal(Squeal::nonRecoverable,
                   "IntegratingOpticsModel is not yet implemented.",
                   "MapCppTrackReconstructor::process()"));
      break;
    }
    case 2: {
      // "Least Squares"
      Json::Value algorithm_names = JsonWrapper::GetProperty(
          configuration_,
          "LeastSquaresOpticsModel_algorithms",
          JsonWrapper::stringValue);
      Json::Value algorithm_name = JsonWrapper::GetProperty(
          configuration_,
          "LeastSquaresOpticsModel_algorithm",
          JsonWrapper::stringValue);
      int algorithm_number;
      for (algorithm_number = 0;
           algorithm_number < algorithm_names.size();
           ++algorithm_number) {
        if (algorithm_name == algorithm_names[algorithm_number]) {
          // leave the current index into algorithm_names in algorithm_number
          break;
        }
      }

      Algorithm algorithm;  track_sets->push_back(

      switch (algorithm_number) {
        case 4: algorithm = Algorithm::kSweepingChiSquaredWithVariableWalls;
                break;
        case 3: algorithm = Algorithm::kSweepingChiSquared;       break;
        case 2: algorithm = Algorithm::kConstrainedChiSquared;    break;
        case 1: algorithm = Algorithm::kConstrainedPolynomial;    break;
        case 0:
        default: algorithm = Algorithm::kUnconstrainedPolynomial;      
      }
      optics_model_ = new LeastSquaresOpticsModel(algorithm);

      break;
    }
    case 3: {
      // "Runge Kutta"
      /* TODO(plane1@hawk.iit.edu)
      optics_model_ = new RungeKuttaOpticsModel();
      */
      throw(Squeal(Squeal::nonRecoverable,
                   "RungeKuttaOpticsModel is not yet implemented.",
                   "MapCppTrackReconstructor::process()"));
      break;
    }
    case 4: {
      // "Linear Approximation"
      optics_model_ = new LinearApproximationOpticsModel();
      break;
    }
    default: {
      std::string message("Unsupported optics model \"");
      message += optics_model_name.asString();
      message += std::string("\" in reconstruction configuration.");
      throw(Squeal(Squeal::nonRecoverable,
                   message.c_str(),
                   "MapCppTrackReconstructor::process()"));
    }
  }
  optics_model_.Build(&configuration_);
}

MapCppTrackReconstructor::SetupTrajectoryFitter() {
  Json::Value fitter_names = JsonWrapper::GetProperty(
      configuration_,
      "reconstruction_trajectory_fitters",
      JsonWrapper::stringValue);
  Json::Value fitter_name = JsonWrapper::GetProperty(
      configuration_,
      "reconstruction_trajectory_fitter",
      JsonWrapper::stringValue);
  int fitter;
  for (fitter = 0; fitter < fitter_names.size(); ++fitter) {
    if (fitter_name == fitter_names[fitter]) {
      break;  // leave the current index into fitter_names in fitter
    }
  }
  
  switch (fitter) {
    case 0: {
      // Minuit
      trajectory_fitter_ = new MinuitTrajectoryFitter(optics_model_);
      break;
    }
    case 1: {
      // Kalman Filter
      /* TODO(plane1@hawk.iit.edu)
      trajectory_fitter_ = new KalmanFilterTrajectoryFitter(optics_model_);
      */
      throw(Squeal(Squeal::nonRecoverable,
                   "KalmanFilterTrajectoryFitter is not yet implemented.",
                   "MapCppTrackReconstructor::process()"));
      break;
    }
    case 2: {
      // TOF
      /* TODO(plane1@hawk.iit.edu)
      trajectory_fitter_ = new TOFTrajectoryFitter(optics_model_);
      */
      throw(Squeal(Squeal::nonRecoverable,
                   "TOFTrajectoryFitter is not yet implemented.",
                   "MapCppTrackReconstructor::process()"));
      break;
    }
    default: {
      std::string message("Unsupported trajectory fitter \"");
      message += optics_model_name.asString();
      message += std::string("\" in reconstruction configuration.");
      throw(Squeal(Squeal::nonRecoverable,
                   message.c_str(),
                   "MapCppTrackReconstructor::process()"));
    }
  }
}

void MapCppTrackReconstructor::LoadTestingData() {
  // TODO(plane1@hawk.iit.edu) reconstruction_input_ = new ReconstructionInput(...);
  // Create random track points for TOF0, TOF1, Tracker 1, Tracker 2, and TOF 2
  srand((unsigned)time(NULL));

  const bool beam_polarity_negative = true;  // e-, mu-, pi-

  CovarianceMatrix * uncertainties = NULL;

  // generate moch detector info
  std::vector<Detector> detectors;
  double plane;
  double uncertainty_data[36];
  for (int id = 0; id <= Detector.kCalorimeter_id; ++id) {
    plane = ((double)rand()/(double)RAND_MAX) * 20;  // 0.0 - 20.0 meters
    for (int index = 0; index < 36; ++index) {
      uncertainty_data[index] = ((double)rand()/(double)RAND_MAX) * 100;
    }
    uncertainties = new CovarianceMatrix(uncertainty_data);
    detectors.push_back(Detector(id, plane, uncertainties));
    delete uncertainties;  // gets copied in Detector constructor
  }

  // generate random muon detector event data
  Particle const * const particle = Particle::GetInstance();
  const double mass = particle->GetMass(Particle::kMuMinus);
  std::vector<TrackPoint> events;
  // 2 trackers with 5 planes each + 3 TOFs = 13 events
  unsigned int detector_id;
  double position[4], momentum[4];
  for (int index = 0; index < 13; ++index) {
    switch (index) {
     case 0: detector_id = Detector::kTOF0; break;
     case 1: detector_id = Detector::kTOF1; break;
     case 2: case 3: case 4: case 5: case 6:
      detector_id = Detector::kTracker1; break;
     case 7: case 8: case 9: case 10: case 11:
      detector_id = Detector::kTracker2; break;
     case 12: detector_id = Detector::kTOF2; break;
    }

    for (int coordinate = 0; coordinate < 4; ++coordinate) {
      position[coordinate] = ((double)rand()/(double)RAND_MAX) * 20;  // meters
    }

    for (int coordinate = 0; coordinate < 4; ++coordinate) {
      momentum[coordinate] = ((double)rand()/(double)RAND_MAX) * 500;  // MeV
    }

    events.push_back(TrackPoint(position[0], momentum[0],
                                position[1], momentum[1],
                                position[2], momentum[2],
                                position[3], momentum[3],
                                detector_id));
  }

  reconstruction_input_ = new ReconstructionInput(beam_polarity_negative,
                                                  detectors,
                                                  events);
}

void MapCppTrackReconstructor::LoadSimulationData() {
  try {
    // MapCppSimulation puts the simulated spill in run_data_["mc"]
    Json::Value mc = JsonWrapper::GetProperty(run_data_,
                                              "mc",
                                              JsonWrapper::arrayValue);
    //mc --> Json::Value[mc.size()] --> "primary"

    // TODO(plane1@hawk.iit.edu) Collect tracks from the detectors and
    // dynamically allocate the ReconstructionInput instance
    // reconstruction_input_ = new ReconstructionInput(...);
  } catch(Squeal& squee) {
    run_data_ = MAUS::CppErrorHandler::getInstance()->HandleSqueal(
        run_data_, squee, "MAUS::MapCppTrackReconstructor");
  } catch(std::exception& exc) {
    run_data_ = MAUS::CppErrorHandler::getInstance()->HandleStdExc(
        run_data_, exc, "MAUS::MapCppTrackReconstructor");
  }
}

void MapCppTrackReconstructor::LoadLiveData() {
  // reconstruction_input_ = new ReconstructionInput(...);
}


void MapCppTrackReconstructor::CorrelateParticleTracks(
    std::vector<std::vector<ParticleTrack const *> > * track_sets) {
  // TODO(plane1@hawk.iit.edu) create sets of events where each set corresponds
  // to a different particle. For now assume that all events are from a single
  // particle.
  std::vector<ParticleTrack> const * const particle_tracks
      = reconstruction_input_->tracks();
  std::vector<ParticleTrack const *> superset;
  std::vector<ParticleTrack>::const_iterator iter = particle_tracks->begin();
  while (iter < particle_tracks->end()) {
    superset->push_back(&(*iter));
    ++iter;
  }
  track_sets->push_back(superset);
}

bool MapCppTrackReconstructor::death() {
  return true;  // successful
}
