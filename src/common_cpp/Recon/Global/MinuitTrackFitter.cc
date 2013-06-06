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

#include "Recon/Global/MinuitTrackFitter.hh"

#include <cmath>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "TMinuit.h"
#include "json/json.h"

#include "DataStructure/Global/Track.hh"
#include "DataStructure/Global/TrackPoint.hh"
#include "Interface/Squeal.hh"
#include "src/common_cpp/Optics/CovarianceMatrix.hh"
#include "src/common_cpp/Optics/OpticsModel.hh"
#include "src/common_cpp/Optics/PhaseSpaceVector.hh"
#include "src/common_cpp/Optics/TransferMap.hh"
#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"
#include "src/common_cpp/Simulation/MAUSPrimaryGeneratorAction.hh"
#include "Recon/Global/DataStructureHelper.hh"
#include "Recon/Global/Particle.hh"
#include "Recon/Global/ParticleOpticalVector.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"

namespace MAUS {
namespace recon {
namespace global {

using MAUS::DataStructure::Global::Track;
using MAUS::DataStructure::Global::TrackPoint;

void common_cpp_optics_recon_minuit_track_fitter_score_function(
    Int_t &    number_of_parameters,
    Double_t * gradiants,
    Double_t & score,
    Double_t * phase_space_coordinate_values,
    Int_t      execution_stage_flag) {
for (size_t index = 0; index < 6; ++index) {
  std::cout << "DEBUG common_..._score_function: coordinate[" << index << "] = "
            << phase_space_coordinate_values[index] << std::endl;
}
  // common_cpp_optics_recon_minuit_track_fitter_minuit is defined
  // globally in the header file
  TMinuit * minuit
    = common_cpp_optics_recon_minuit_track_fitter_minuit;

  MinuitTrackFitter * track_fitter
    = static_cast<MinuitTrackFitter *>(minuit->GetObjectFit());

  score = track_fitter->ScoreTrack(phase_space_coordinate_values);
}

MinuitTrackFitter::MinuitTrackFitter(
    const OpticsModel & optics_model,
    const double start_plane)
    : TrackFitter(optics_model, start_plane) {
  // Setup *global* scope Minuit object
  common_cpp_optics_recon_minuit_track_fitter_minuit
    = new TMinuit(kPhaseSpaceDimension);
  TMinuit * minimiser
    = common_cpp_optics_recon_minuit_track_fitter_minuit;

  Json::Value const * configuration = optics_model.configuration();

  const double max_iterations = JsonWrapper::GetProperty(
      *configuration, "global_recon_minuit_max_iterations",
      JsonWrapper::intValue).asInt();
  minimiser->SetMaxIterations(max_iterations);

  minimiser->SetObjectFit(this);

  minimiser->SetFCN(
    common_cpp_optics_recon_minuit_track_fitter_score_function);

  // setup the index, name, init value, step size, min, and max value for each
  // phase space variable (mins and maxes calculated from 800MeV/c ISIS beam)
  int error_flag = 0;
  const Json::Value parameters = JsonWrapper::GetProperty(
      *configuration, "global_recon_minuit_parameters",
      JsonWrapper::arrayValue);
  const Json::Value::UInt parameter_count = parameters.size();
  for (Json::Value::UInt index = 0; index < parameter_count; ++index) {
    const Json::Value parameter = parameters[index];
    const std::string name = JsonWrapper::GetProperty(
        parameter, "name", JsonWrapper::stringValue).asString();
    const bool fixed = JsonWrapper::GetProperty(
        parameter, "fixed", JsonWrapper::booleanValue).asBool();
    const double initial_value = JsonWrapper::GetProperty(
        parameter, "initial_value", JsonWrapper::realValue).asDouble();
    const double value_step = JsonWrapper::GetProperty(
        parameter, "value_step", JsonWrapper::realValue).asDouble();
    const double min_value = JsonWrapper::GetProperty(
        parameter, "min_value", JsonWrapper::realValue).asDouble();
    const double max_value = JsonWrapper::GetProperty(
        parameter, "max_value", JsonWrapper::realValue).asDouble();

    minimiser->mnparm(index, name, initial_value, value_step,
                       min_value, max_value, error_flag);
    if (fixed) {
      minimiser->FixParameter(index);
    }
  }
  /*
  minimiser->mnparm(0, "Time", 0., 0.1, -2., 2., error_flag);   // ns
  minimiser->mnparm(1, "Energy", 1860., 1, 105.7, 1860., error_flag);  // MeV
  minimiser->mnparm(2, "X", 0, 0.001, -150., 150., error_flag);      // mm
  minimiser->mnparm(3, "Px", 0., 0.1, -100., 100, error_flag);    // MeV/c
  minimiser->mnparm(4, "Y", 0, 0.001, -150., 150., error_flag);      // mm
  minimiser->mnparm(5, "Py", 0., 0.1, -100., 100, error_flag);    // MeV/c
  minimiser->FixParameter(1);
  */
}

MinuitTrackFitter::~MinuitTrackFitter() {
  delete common_cpp_optics_recon_minuit_track_fitter_minuit;
}

void MinuitTrackFitter::Fit(Track const * const raw_track, Track * const track) {
  std::cout << "CHECKPOINT Fit(): BEGIN" << std::endl;
  std::cout.flush();
  *const_cast<std::vector<const TrackPoint*>*>(&detector_events_)
    = raw_track->GetTrackPoints();
  std::cout << "DEBUG MinuitTrackFitter::Fit(): CHECKPOINT 0" << std::endl;
  std::cout << "DEBUG MinuitTrackFitter::Fit(): Fitting track with "
            << detector_events_.size() << "track points." << std::endl;
  std::cout << "DEBUG MinuitTrackFitter::Fit(): CHECKPOINT 0.5" << std::endl;
  particle_id_ = raw_track->get_pid();

  std::cout << "DEBUG MinuitTrackFitter::Fit(): CHECKPOINT 1" << std::endl;
  if (detector_events_.size() < 2) {
    throw(Squeal(Squeal::recoverable,
                 "Not enough track points to fit track (need at least two).",
                 "MAUS::MinuitTrackFitter::Fit()"));
  }
  std::cout << "DEBUG MinuitTrackFitter::Fit(): CHECKPOINT 2" << std::endl;

  // Find the start plane coordinates that minimize the score for the calculated
  // track based off of this track point (i.e. best fits the measured track
  // points from the detectors).
  TMinuit * minimiser
    = common_cpp_optics_recon_minuit_track_fitter_minuit;
  // Int_t status = minimiser->Migrad();
  minimiser->Migrad();
  // TODO(plane1@hawk.iit.edu) Handle status from minimiser

  size_t particle_event
    = raw_track->GetTrackPoints()[0]->get_particle_event();

  for (std::vector<TrackPoint>::iterator reconstructed_point
          = reconstructed_points_.begin();
       reconstructed_point < reconstructed_points_.end();
       ++reconstructed_point) {
    reconstructed_point->set_particle_event(particle_event);
    track->AddTrackPoint(new TrackPoint(*reconstructed_point));
  }
  track->set_pid(raw_track->get_pid());

std::cout << "CHECKPOINT Fit(): END" << std::endl;
std::cout.flush();
}

Double_t MinuitTrackFitter::ScoreTrack(
    Double_t * const start_plane_track_coordinates) {
  MAUSGeant4Manager * const simulator = MAUSGeant4Manager::GetInstance();
  MAUSPrimaryGeneratorAction::PGParticle reference_pgparticle
    = simulator->GetReferenceParticle();
  const double t0 = reference_pgparticle.time;
  const double E0 = reference_pgparticle.energy;
  const double P0 = reference_pgparticle.pz;
std::cout << "CHECKPOINT ScoreTrack(): 0" << std::endl;
std::cout.flush();
  // clear the last saved track
  reconstructed_points_.clear();

  DataStructureHelper helper = DataStructureHelper::GetInstance();

  // Setup the start plane track point based on the Minuit initial conditions
  CovarianceMatrix null_uncertainties;
  const PhaseSpaceVector guess(start_plane_track_coordinates[0],
                               start_plane_track_coordinates[1],
                               start_plane_track_coordinates[2],
                               start_plane_track_coordinates[3],
                               start_plane_track_coordinates[4],
                               start_plane_track_coordinates[5]);
  // If the guess is not physical then return a horrible score
  if (!ValidGuess(guess)) {
//    return std::numeric_limits<double>::max();
    return 1.0e+15;
  }
  TrackPoint primary = helper.PhaseSpaceVector2TrackPoint(
      guess, optics_model_->primary_plane(), particle_id_);
  reconstructed_points_.push_back(primary);

  std::vector<const TrackPoint*>::const_iterator event
    = detector_events_.begin();

  // calculate chi^2
  Double_t chi_squared = 0.0;
  for (size_t index = 0; event < detector_events_.end(); ++index) {
std::cout << "DEBUG MinuitTrackFitter::ScoreTrack(): Guess: "
          << guess << std::endl;
std::cout << "DEBUG MinuitTrackFitter::ScoreTrack(): Measured: "
          << *event << std::endl;
    // calculate the next guess
    const double end_plane = (*event)->get_position().Z();
    PhaseSpaceVector point =
    #if 0
      ParticleOpticalVector(
        optics_model_->Transport(ParticleOpticalVector(guess, t0, E0, P0),
                                 end_plane)),
        t0, E0, P0);
    #else
      optics_model_->Transport(guess, end_plane);
    #endif
std::cout << "DEBUG MinuitTrackFitter::ScoreTrack(): Calculated: "
          << point << std::endl;

    TLorentzVector position_error = (*event)->get_position_error();
    TLorentzVector momentum_error = (*event)->get_momentum_error();
    const double errors[36] = {
      position_error.T(), 0., 0., 0., 0., 0.,
      0., momentum_error.E(), 0., 0., 0., 0.,
      0., 0., position_error.X(), 0., 0., 0.,
      0., 0., 0., momentum_error.Px(), 0., 0.,
      0., 0., 0., 0., position_error.Y(), 0.,
      0., 0., 0., 0., 0., momentum_error.Py(),
    };
    const Matrix<double> error_matrix(6, 6, errors);
    const CovarianceMatrix uncertainties(error_matrix*error_matrix);
std::cout << "DEBUG MinuitTrackFitter::ScoreTrack(): Uncertainties: "
          << uncertainties << std::endl;

    // save the calculated track point in case this is the
    // last (best fitting) track
    reconstructed_points_.push_back(
      helper.PhaseSpaceVector2TrackPoint(point, end_plane, particle_id_));
std::cout << "DEBUG MinuitTrackFitter::ScoreTrack(): Pushed track point #"
          << reconstructed_points_.size() << std::endl;

    const double weights[36] = {
      1., 0., 0., 0., 0., 0.,
      0., 1., 0., 0., 0., 0.,
      0., 0., 1., 0., 0., 0.,
      0., 0., 0., 1., 0., 0.,
      0., 0., 0., 0., 1., 0.,
      0., 0., 0., 0., 0., 1.,
    };
    Matrix<double> weight_matrix(6, 6, weights);

    // Sum the squares of the differences between the calculated phase space
    // coordinates (point) and the measured coordinates (event).
    // TrackPoint residual = TrackPoint(weight_matrix * (point - (*events)));
    PhaseSpaceVector event_point = helper.TrackPoint2PhaseSpaceVector(**event);
    PhaseSpaceVector residual = PhaseSpaceVector(
      weight_matrix * (event_point - point));
    ParticleOpticalVector normalized_residual(event_point, t0, E0, P0);
    normalized_residual -= ParticleOpticalVector(point, t0, E0, P0);
std::cout << "DEBUG MinuitTrackFitter::ScoreTrack(): Point: "
          << point << std::endl;
std::cout << "DEBUG MinuitTrackFitter::ScoreTrack(): Event: "
          << event_point << std::endl;
std::cout << "DEBUG MinuitTrackFitter::ScoreTrack(): Residual: "
          << residual << std::endl;
std::cout << "DEBUG MinuitTrackFitter::ScoreTrack(): Normalized Residual: "
          << normalized_residual << std::endl;
    chi_squared += (transpose(normalized_residual)
                    * uncertainties
                    * normalized_residual)[0];
std::cout << "DEBUG MinuitTrackFitter::ScoreTrack(): Chi Squared: "
          << chi_squared << std::endl;

    ++event;
  }
std::cout << "DEBUG MinuitTrackFitter::ScoreTrack(): "
          << "Energy = " << guess[1] << "\tScore = " << chi_squared << std::endl;

  return chi_squared;
}

bool MinuitTrackFitter::ValidGuess(const PhaseSpaceVector & guess) const {
  bool valid = true;

  if (guess != guess) {
    // No NaN guesses
    valid = false;
  }

  return valid;
}
/*
bool MinuitTrackFitter::ValidGuess(const TrackPoint & guess) const {
  const double mass = Particle::GetInstance()->GetMass(guess.particle_id());
  const double E = guess.E();
  const double px = guess.Px();
  const double py = guess.Py();

  bool valid = true;
  
  if (guess != guess) {
    // No NaN guesses
    valid = false;
  } else if (::sqrt(px*px + py*py + mass*mass) > E) {
    // Energy cannot be greater than the sum of the squares of the transverse
    // momenta and mass
std::cout << "DEBUG ValidGuess(): Energy is less than the sum of the squares of the transverse momenta and mass." << std::endl;
    valid = false;
  } else if (::abs(start_plane_ - guess.z()) > 1) {
    // The z coordinate cannot be significantly different from the start plane
std::cout << "DEBUG ValidGuess(): The z coordinate differs significantly from the start plane." << std::endl;
    valid = false;
  }

  return valid;
}
*/

const size_t MinuitTrackFitter::kPhaseSpaceDimension = 6;

}  // namespace global
}  // namespace recon
}  // namespace MAUS
