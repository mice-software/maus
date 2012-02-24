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

void transfer_map_score_function(Int_t &    number_of_parameters,
                                 Double_t * gradiants,
                                 Double_t & score,
                                 Double_t * phase_space_coordinate_values,
                                 Int_t      execution_stage_flag) {
  // map_cpp_track_reconstructor_minuit is defined globally in the header file
  TMinuit * minuit = map_cpp_track_reconstructor_minuit;

  const PhaseSpaceVector start_plane_track(phase_space_coordinate_values[0],
                                           phase_space_coordinate_values[1],
                                           phase_space_coordinate_values[2],
                                           phase_space_coordinate_values[3],
                                           phase_space_coordinate_values[4],
                                           phase_space_coordinate_values[5]) {

  MapCppTrackReconstructor * reconstructor
    = (MapCppTrackReconstructor *) minuit->GetObjectFit();

  const double score = reconstructor->ScoreTrack(&start_plane_track);
}

MapCppTrackReconstructor::MapCppTrackReconstructor()
    : optics_model_(NULL), events_(NULL), track_fitter_(NULL),
      trajectories_(NULL), electromagnetic_field_(NULL) {
  classname_ = "MapCppTrackReconstructor";
    
  // Setup *global* scope Minuit object
  map_cpp_track_reconstructor_minuit = new TMinuit(kPhaseSpaceDimension);
}

MapCppTrackReconstructor::MapCppTrackReconstructor() {
  delete map_cpp_track_reconstructor_minuit;
}

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
  /* TODO(plane1@hawk.iit.edu)
  if (optics_model_name == optics_model_names[0]) {
    optics_model_ = new DifferentiatingOpticsModel();
  } else if (optics_model_name == optics_model_names[1]) {
    optics_model_ = new IntegratingOpticsModel();
  } else if (optics_model_name == optics_model_names[2]) {
    optics_model_ = new LeastSquaresOpticsModel();
  } else if (optics_model_name == optics_model_names[3]) {
    optics_model_ = new RungeKuttaOpticsModel();
  */
  if (optics_model_name == optics_model_names[2]) {
    int algorithm_number
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
  } else {
    throw(Squeal(Squeal::nonRecoverable,
                 "Unsupported optics model in reconstruction configuration.",
                 "MapCppTrackReconstructor::process()"));
  }
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

  // TODO(plane1@hawk.iit.edu) Use Minuit to reconstruct the start plane track

  // TODO(plane1@hawk.iit.edu) Reconstruct tracks at the desired locations
  //                           specified in the configuration.

  Json::FastWriter writer;
  std::string output = writer.write(spill);
  return output;
}

bool MapCppTrackReconstructor::death() {
  return true;  // successful
}

