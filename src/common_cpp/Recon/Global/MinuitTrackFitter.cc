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

#include "CLHEP/Units/PhysicalConstants.h"
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
namespace GlobalDS = MAUS::DataStructure::Global;

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
    OpticsModel const * optics_model,
    const double start_plane)
    : TrackFitter(optics_model, start_plane), rounds_(0) {
  // Setup *global* scope Minuit object
  common_cpp_optics_recon_minuit_track_fitter_minuit
    = new TMinuit(kPhaseSpaceDimension);
  TMinuit * minimizer
    = common_cpp_optics_recon_minuit_track_fitter_minuit;

  /*
  Json::Value const * configuration = optics_model_->configuration();

  rounds_ = JsonWrapper::GetProperty(
      *configuration, "global_recon_minuit_rounds",
      JsonWrapper::intValue).asInt();
  if (rounds_ < 2) {
    rounds_ = 2;
  }
  */

  minimizer->SetObjectFit(this);

  minimizer->SetFCN(
    common_cpp_optics_recon_minuit_track_fitter_score_function);

  ResetParameters();
}

MinuitTrackFitter::~MinuitTrackFitter() {
  delete common_cpp_optics_recon_minuit_track_fitter_minuit;
}

void MinuitTrackFitter::ResetParameters() {
  TMinuit * minimizer
    = common_cpp_optics_recon_minuit_track_fitter_minuit;

  // setup the index, name, init value, step size, min, and max value for each
  // phase space variable (mins and maxes calculated from 800MeV/c ISIS beam)
  int error_flag = 0;
  Json::Value const * configuration = optics_model_->configuration();
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

    minimizer->mnparm(index, name, initial_value, value_step,
                       min_value, max_value, error_flag);
    if (fixed) {
      minimizer->FixParameter(index);
    }
  }
}

void MinuitTrackFitter::Fit(Track const * const raw_track, Track * const track,
                            const std::string mapper_name) {
  std::cout << "CHECKPOINT Fit(): BEGIN" << std::endl;
  std::cout.flush();
  detector_events_ = raw_track->GetTrackPoints();
  std::cout << "DEBUG MinuitTrackFitter::Fit(): CHECKPOINT 0" << std::endl;
  std::cout << "DEBUG MinuitTrackFitter::Fit(): Fitting track with "
            << detector_events_.size() << " track points." << std::endl;
  std::cout << "DEBUG MinuitTrackFitter::Fit(): CHECKPOINT 0.5" << std::endl;
  particle_id_ = raw_track->get_pid();
  std::cout << "DEBUG MinuitTrackFitter::Fit(): particle ID: "
            << particle_id_ << std::endl;

  std::cout << "DEBUG MinuitTrackFitter::Fit(): CHECKPOINT 1" << std::endl;
  if (detector_events_.size() < 2) {
    throw(Squeal(Squeal::recoverable,
                 "Not enough track points to fit track (need at least two).",
                 "MAUS::MinuitTrackFitter::Fit()"));
  }
  std::cout << "DEBUG MinuitTrackFitter::Fit(): CHECKPOINT 2" << std::endl;

  ResetParameters();

  TMinuit * minimizer
    = common_cpp_optics_recon_minuit_track_fitter_minuit;

  // Find the start plane coordinates that minimize the score for the calculated
  // track based off of this track point (i.e. best fits the measured track
  // points from the detectors).
  Json::Value const * const configuration = optics_model_->configuration();
  const std::string method = JsonWrapper::GetProperty(
      *configuration, "global_recon_minuit_minimizer",
      JsonWrapper::stringValue).asString();
  const double max_iterations = JsonWrapper::GetProperty(
      *configuration, "global_recon_minuit_max_iterations",
      JsonWrapper::intValue).asInt();
  const double max_EDM = JsonWrapper::GetProperty(
      *configuration, "global_recon_minuit_max_edm",
      JsonWrapper::realValue).asDouble();

  Int_t err = 0;
  Double_t args[2] = {max_iterations, max_EDM};
  minimizer->mnexcm(method.c_str(), args, 2, err);

  DataStructureHelper helper = DataStructureHelper::GetInstance();

  PhaseSpaceVector fit_primary;
  for (size_t index = 0; index < kPhaseSpaceDimension; ++index) {
    Double_t value, error;
    minimizer->GetParameter(index, value, error);
    fit_primary[index] = value;
  }
  std::cout << "DEBUG MinuitTrackFitter::Fit: Fit Primary: " << fit_primary
            << std::endl;
  try {
    TrackPoint track_point = helper.PhaseSpaceVector2TrackPoint(
        fit_primary,
        optics_model_->primary_plane(),
        particle_id_);
    track_point.set_mapper_name(mapper_name);
    track->AddTrackPoint(new TrackPoint(track_point));
  } catch (Squeal squeal) {
      std::cerr << "DEBUG MinuitTrackFitter::ScoreTrack: "
                << "something bad happened during track fitting: "
                << squeal.what() << std::endl;
      // FIXME(Lane) handle better by reporting horrible score or something
  }


  // Reconstruct the fit track from the fit primary
  GlobalDS::TrackPointCPArray raw_points = raw_track->GetTrackPoints();
  size_t particle_event = raw_points[0]->get_particle_event();
  GlobalDS::TrackPointCPArray::const_iterator raw_point = raw_points.begin();
  for (; raw_point != raw_points.end(); ++raw_point) {
    // transport the fit primary to the desired z-position
    const double z = (*raw_point)->get_position().Z();
    const PhaseSpaceVector point = optics_model_->Transport(fit_primary, z);
    std::cout << "DEBUG MinuitTrackFitter::Fit: track point: " << point << std::endl;

    TrackPoint track_point;
    try {
      track_point = helper.PhaseSpaceVector2TrackPoint(point, z, particle_id_);
    } catch (Squeal squeal) {
        std::cerr << "DEBUG MinuitTrackFitter::ScoreTrack: "
                  << "something bad happened during track fitting: "
                  << squeal.what() << std::endl;
        // FIXME(Lane) handle better by reporting horrible score or something
    }

    track_point.set_particle_event(particle_event);
    track_point.set_mapper_name(mapper_name);
    track->AddTrackPoint(new TrackPoint(track_point));
  }
  track->set_pid(raw_track->get_pid());

  /*
  for (std::vector<TrackPoint>::iterator reconstructed_point
          = reconstructed_points_.begin();
       reconstructed_point != reconstructed_points_.end();
       ++reconstructed_point) {
    reconstructed_point->set_particle_event(particle_event);
    reconstructed_point->set_mapper_name(mapper_name);
    track->AddTrackPoint(new TrackPoint(*reconstructed_point));
  }
  track->set_pid(raw_track->get_pid());
  */

std::cout << "CHECKPOINT Fit(): END" << std::endl;
std::cout.flush();
}

Double_t MinuitTrackFitter::ScoreTrack(
    Double_t * const start_plane_track_coordinates) {
  MAUSGeant4Manager * const simulator = MAUSGeant4Manager::GetInstance();
  MAUSPrimaryGeneratorAction::PGParticle reference_pgparticle
    = simulator->GetReferenceParticle();
  const double time_calibration = 0;
  /*
  const double t0 = reference_pgparticle.time;
  const double E0 = reference_pgparticle.energy;
  const double P0 = reference_pgparticle.pz;
  */
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
  if (!ValidVector(guess)) {
//    return std::numeric_limits<double>::max();
    return 1.0e+15;
  }
  std::cout << "DEBUG MinuitTrackFitter::ScoreTrack: particle ID = "
            << particle_id_ << std::endl;
  /*
  try {
    TrackPoint primary = helper.PhaseSpaceVector2TrackPoint(
        guess, optics_model_->primary_plane(), particle_id_);
    reconstructed_points_.push_back(primary);
  } catch (Squeal squeal) {
      std::cerr << "DEBUG MinuitTrackFitter::ScoreTrack: "
                << "something bad happened during track fitting: "
                << squeal.what() << std::endl;
    return 1.0e+15;
  }
  */

  std::vector<const TrackPoint*>::const_iterator event
    = detector_events_.begin();

  // calculate chi^2
  Double_t chi_squared = 0.0;
  size_t index = 0;
  for (std::vector<const TrackPoint*>::const_iterator event
        = detector_events_.begin();
       event != detector_events_.end();
       ++event) {
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
//std::cerr << uncertainties << std::endl;

    // save the calculated track point in case this is the
    // last (best fitting) track
    /*
    try {
      reconstructed_points_.push_back(
        helper.PhaseSpaceVector2TrackPoint(point, end_plane, particle_id_));
    } catch (Squeal squeal) {
        std::cerr << "DEBUG MinuitTrackFitter::ScoreTrack: "
                  << "something bad happened during track fitting: "
                  << squeal.what() << std::endl;
      return 1.0e+15;
    }
    */
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
    const PhaseSpaceVector time_comp_vector(time_calibration, 0, 0, 0, 0, 0);
    PhaseSpaceVector event_point = helper.TrackPoint2PhaseSpaceVector(**event);
    PhaseSpaceVector residual = PhaseSpaceVector(
      weight_matrix * (event_point - point));// - time_comp_vector));
    /*
    ParticleOpticalVector normalized_event(event_point,
                                           t0 + time_calibration, E0, P0);
    ParticleOpticalVector normalized_estimate(point, t0, E0, P0);
    ParticleOpticalVector normalized_residual(
      normalized_event - normalized_estimate);
    */
std::cout << "DEBUG MinuitTrackFitter::ScoreTrack(): Event: "
          << event_point << std::endl;
/*
std::cout << "DEBUG MinuitTrackFitter::ScoreTrack(): Normalized Event: "
          << normalized_event << std::endl;
*/
std::cout << "DEBUG MinuitTrackFitter::ScoreTrack(): Estimate: "
          << point << std::endl;
/*
std::cout << "DEBUG MinuitTrackFitter::ScoreTrack(): Normalized Estimate: "
          << normalized_estimate << std::endl;
*/
std::cout << "DEBUG MinuitTrackFitter::ScoreTrack(): Residual: "
          << residual << std::endl;
/*
std::cout << "DEBUG MinuitTrackFitter::ScoreTrack(): Normalized Residual: "
          << setprecision(20) << normalized_residual << std::endl;
    const double residual_squared = (transpose(normalized_residual)
                                     * uncertainties
                                     * normalized_residual)[0];
*/
    residual -= time_comp_vector;
    std::cout << "DEBUG MinuitTrackFitter::ScoreTrack(): Weighted Residual: "
          << (inverse(uncertainties) * residual) << std::endl;
    const double residual_squared = (transpose(residual)
                                     * inverse(uncertainties)
                                     * residual)[0];
    //std::cerr << transpose(residual) << "\n" << inverse(uncertainties) << std::endl;
    std::cout << "DEBUG MinuitTrackFitter::ScoreTrack(): Residual Squared: "
          << residual_squared << std::endl;
    chi_squared += residual_squared;
/*
std::cerr << "xm: " << event_point.x() << " pxm: " << event_point.Px() << " ym: " << event_point.y() << " pym: " << event_point.Py()
          << " xr: " << point.x() << " pxr: " << point.Px() << " yr: " << point.y() << " pyr: " << point.Py()
          << " dx: " << residual.x() << " dpx: " << residual.Px() << " dy: " << residual.y() << " dpy: " << residual.Py()
          << " chi2: " << chi_squared << std::endl;
*/
std::cerr << residual << std::endl
          << " = " << event_point << std::endl
          << " - " << point << std::endl
          << " -- chi2: " << chi_squared << std::endl;
    ++index;
  }
  std::cerr << std::endl;
std::cout << "DEBUG MinuitTrackFitter::ScoreTrack(): "
          << "Energy = " << guess[1] << "\tScore = " << chi_squared << std::endl;

  return chi_squared;
}

/*
bool MinuitTrackFitter::ValidVector(const PhaseSpaceVector & guess) const {
  bool valid = true;

  if (guess != guess) {
    // No NaN guesses
    valid = false;
  }

  return valid;
}
*/

bool MinuitTrackFitter::ValidVector(const PhaseSpaceVector & guess) const {
  const double mass = Particle::GetInstance().GetMass(particle_id_);
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
    std::cout << "DEBUG ValidVector(): Guess is off mass shell" << std::endl;
    valid = false;
  }

  return valid;
}

const size_t MinuitTrackFitter::kPhaseSpaceDimension = 6;

}  // namespace global
}  // namespace recon
}  // namespace MAUS
