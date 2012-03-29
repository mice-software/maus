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

#include <Python.h>

#include <vector>
#include <string>

#include "TMinuit.h"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"

#include "Interface/Squeal.hh"
#include "Interface/dataCards.hh"

#include "src/map/MapCppTrackReconstructor/MapCppTrackReconstructor.hh"

namespace MAUS {

using MAUS::LeastSquaresOpticsModel::Algorithm;

bool MapCppTrackReconstructor::birth(std::string argJsonConfigDocument) {
  // Attempt to parse the JSON document.
  try {
    SetConfiguration(argJsonConfigDocument);
    return true;  // Sucessful completion
  } catch(Squeal& squee) {
    MAUS::CppErrorHandler::getInstance()->HandleSquealNoJson(squee, _classname);
  } catch(std::exception& exc) {
    MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, _classname);
  }
  return false;
}

void MapCppSimulation::SetConfiguration(std::string json_configuration) {
  // load the configuration
  MICERun& run = *MICERun::getInstance();
  run.jsonConfiguration = new Json::Value
                                (JsonWrapper::StringToJson(json_configuration));
  Json::Value& config = *run.jsonConfiguration;
  run.DataCards = new dataCards("Reconstruction");

  // Next function disables std::cout, std::clog,
  // std::cerr depending on VerboseLevel
  Squeak::setStandardOutputs();

  // Populate the materials data structure from the configuration
  run.miceMaterials = new MiceMaterials();
  // MICE Model setup
  Json::Value modname = JsonWrapper::GetProperty
             (config, "simulation_geometry_filename", JsonWrapper::stringValue);
  run.miceModule = new MiceModule(modname.asString());
  // G4 Materials
  fillMaterials(run);

  // TODO(plane1@hawk.iit.edu) Put the materials info into some form that can be
  //                           use in creating the transfer map

  // set the RF cavity phases
  g4manager_ = MAUS::MAUSGeant4Manager::GetInstance();
  g4manager_->SetPhases();

  // save the electromagnetic field for calculating the transfer map
  electromagnetic_field_ = run.btFieldConstructor->GetElectroMagneticField();
}

std::string MapCppTrackReconstructor::process(std::string document) {
  // TODO(plane1@hawk.iit.edu) Create transfer map from the materials and fields
  MICERun& run = *MICERun::getInstance();
  Json::Value& config = *run.jsonConfiguration;
  Json::Value optics_model_names
    = JsonWrapper::GetProperty(config, "reconstruction_optics_models",
                               JsonWrapper::stringValue);
  Json::Value optics_model_name
    = JsonWrapper::GetProperty(config, "reconstruction_optics_model",
                               JsonWrapper::stringValue);

  if (false) {
  if (optics_model_name == optics_model_names[0]) {
    // "Differentiating"
    /* TODO(plane1@hawk.iit.edu)
    optics_model_ = new DifferentiatingOpticsModel();
    */
    throw(Squeal(Squeal::nonRecoverable,
                 "DifferentiatingOpticsModel is not yet implemented.",
                 "MapCppTrackReconstructor::process()"));
  } else if (optics_model_name == optics_model_names[1]) {
    // "Integrating"
    /* TODO(plane1@hawk.iit.edu)
    optics_model_ = new IntegratingOpticsModel();
    */
    throw(Squeal(Squeal::nonRecoverable,
                 "IntegratingOpticsModel is not yet implemented.",
                 "MapCppTrackReconstructor::process()"));
  } else if (optics_model_name == optics_model_names[2]) {
    // "Least Squares"
    const int algorithm_number
      = JsonWrapper::GetProperty(config, "LeastSquaresOpticsModel_algorithm",
                                 JsonWrapper::stringValue).asInt();
    Algorithm algorithm;
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
  } else if (optics_model_name == optics_model_names[3]) {
    // "Runge Kutta"
    /* TODO(plane1@hawk.iit.edu)
    optics_model_ = new RungeKuttaOpticsModel();
    */
    throw(Squeal(Squeal::nonRecoverable,
                 "RungeKuttaOpticsModel is not yet implemented.",
                 "MapCppTrackReconstructor::process()"));
  } else {
    std::string message(
      "Unsupported optics model in reconstruction configuration: ");
    message += optics_model_name.asString();
    throw(Squeal(Squeal::nonRecoverable,
                 message.c_str(),
                 "MapCppTrackReconstructor::process()"));
  }
  /* TODO(plane1@hawk.iit.edu)
  if (optics_model_name == optics_model_names[0]) {
   optics_model_ = new DifferentiatingOpticsModel();
  } else if (optics_model_name == optics_model_names[1]) {
    optics_model_ = new IntegratingOpticsModel();
  } else if (optics_model_name == optics_model_names[2]) {
    optics_model_ = new LeastSquaresOpticsModel();
  } else if (optics_model_name == optics_model_names[3]) {
    optics_model_ = new RungeKuttaOptics  /* TODO(plane1@hawk.iit.edu)
Model();
  } else {
    return std::string("{\"errors\":{\"bad_json_document\":") +
           std::string("\"Unsupported optics model in reconstruction ") +
           std::string("configuration.\"}}");
  }
  */
  // Use a stupid linear approximation model until everything else is working.
  optics_model_ = new LinearApproximationOpticsModel();
  optics_model_.Build(&config);

  Json::Value spill;
  try {spill = JsonWrapper::StringToJson(document);}
  catch(...) {
    return std::string("{\"errors\":{\"bad_json_document\":")+
           std::string("\"Failed to parse input document\"}}");
  }

  try {
    // MapCppSimulation puts the simulated spill in spill["mc"]
    Json::Value mc   = JsonWrapper::GetProperty
                                        (spill, "mc", JsonWrapper::arrayValue);

    // TODO(plane1@hawk.iit.edu) Collect tracks from the detectors if this is
    //                           not a simulation.
  } catch(Squeal& squee) {
    spill = MAUS::CppErrorHandler::getInstance()
                                       ->HandleSqueal(spill, squee, _classname);
  } catch(std::exception& exc) {
    spill = MAUS::CppErrorHandler::getInstance()
                                         ->HandleStdExc(spill, exc, _classname);
  }

  // TODO(plane1@hawk.iit.edu) Implement Kalman Filter and TOF track fitters
  //  in addition to Minuit, and select between them based on the configuration

  // Use Minuit to reconstruct the start plane track
  trajectory_fitter_ = new MinuitTrajectoryFitter(optics_model_);
 
  // determine which events came from which particles
  std::vector<std::vector<DetectorEvent const * const> > event_sets;
  CorrelateDetectorEvents(detector_events_, &event_sets);

  // Find the best fit trajectory for each particle traversing the lattice
  const_iterator<std::vector<DetectorEvent const * const> >
                                                          measured_trajectories;
  for (trajectories = event_sets.begin();
       trajectories < event_sets.end();
       ++trajectories) {
    int particle_id = (*trajectories).front()->paritcle_id();
    ParticleTrajectory best_fit_trajectory(particle_id);
    trajectory_fitter_->Fit(&(*measured_trajectories), &best_fit_trajectory);

    // TODO(plane1@hawk.iit.edu) Reconstruct tracks at the desired locations
    //  specified in the configuration.

    trajectories_.push_back(best_fit_trajectory);
  }

  Json::FastWriter writer;
  std::string output = writer.write(spill);

  // clean up
  delete optics_model_;
  delete trajectory_fitter_;

  return output;
}

void MapCppTrackReconstructor::CorrelateDetectorEvents(
    std::vector<DetectorEvent> const * const detector_events,
    std::vector<std::vector<DetectorEvent const * const> > * event_sets) {
  // TODO(plane1@hawk.iit.edu) create sets of events where each set corresponds
  // to a different particle. For now assume that all events are from the same
  // particle.
  event_sets->assign(detector_events->begin(), detector_events->end());
}

bool MapCppTrackReconstructor::death() {
  return true;  // successful
}
