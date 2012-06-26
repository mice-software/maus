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

#include "Reconstruction/Global/MinuitTrackFitter.hh"

#include <cmath>
#include <iostream>
#include <limits>
#include <vector>

#include "TMinuit.h"

#include "Interface/Squeal.hh"
#include "src/common_cpp/Optics/CovarianceMatrix.hh"
#include "src/common_cpp/Optics/OpticsModel.hh"
#include "src/common_cpp/Optics/PhaseSpaceVector.hh"
#include "src/common_cpp/Optics/TransferMap.hh"
#include "Reconstruction/Global/Particle.hh"
#include "Reconstruction/Global/Track.hh"
#include "Reconstruction/Global/TrackPoint.hh"

namespace MAUS {
namespace reconstruction {
namespace global {

void common_cpp_optics_reconstruction_minuit_track_fitter_score_function(
    Int_t &    number_of_parameters,
    Double_t * gradiants,
    Double_t & score,
    Double_t * phase_space_coordinate_values,
    Int_t      execution_stage_flag) {
for (size_t index = 0; index < 6; ++index) {
  std::cout << "DEBUG ..._score_function: coordinate[" << index << "] = "
            << phase_space_coordinate_values[index] << std::endl;
}
  // common_cpp_optics_reconstruction_minuit_track_fitter_minuit is defined
  // globally in the header file
  TMinuit * minuit
    = common_cpp_optics_reconstruction_minuit_track_fitter_minuit;

  MinuitTrackFitter * track_fitter
    = static_cast<MinuitTrackFitter *>(minuit->GetObjectFit());
  score = track_fitter->ScoreTrack(phase_space_coordinate_values);
}

MinuitTrackFitter::MinuitTrackFitter(
    const OpticsModel & optics_model,
    const double start_plane)
    : TrackFitter(optics_model, start_plane) {
  // Setup *global* scope Minuit object
  common_cpp_optics_reconstruction_minuit_track_fitter_minuit
    = new TMinuit(kPhaseSpaceDimension);

  TMinuit * minimiser
    = common_cpp_optics_reconstruction_minuit_track_fitter_minuit;
  minimiser->SetMaxIterations(100);
  minimiser->SetObjectFit(this);
  minimiser->SetFCN(
    common_cpp_optics_reconstruction_minuit_track_fitter_score_function);

  // setup the index, name, init value, step size, min, and max value for each
  // phase space variable (mins and maxes calculated from 800MeV/c ISIS beam)
  int error_flag = 0;
  // TODO(plane1@hawk.iit.edu) put this in the configuration file
//  minimiser->mnparm(0, "Time", 0., 0.1, -1000., 1., error_flag);   // ns
//  minimiser->mnparm(1, "Energy", 200., 1, 105.7, 1860., error_flag);  // MeV
  minimiser->mnparm(0, "X", 0, 0.001, -150., 150., error_flag);      // mm
  minimiser->mnparm(1, "Px", 0., 0.1, -100., 100, error_flag);    // MeV/c
  minimiser->mnparm(2, "Y", 0, 0.001, -150., 150., error_flag);      // mm
  minimiser->mnparm(3, "Py", 0., 0.1, -100., 100, error_flag);    // MeV/c
  minimiser->mnparm(4, "Z", start_plane, 0.001, -15000., 200000., error_flag);      // mm
  minimiser->mnparm(5, "Pz", 1., 0.1, -1., 500, error_flag);    // MeV/c
  minimiser->FixParameter(4);
//Int_t DefineParameter( Int_t parNo, const char *name, Double_t initVal, Double_t initErr, Double_t lowerLimit, Double_t upperLimit )

}

MinuitTrackFitter::~MinuitTrackFitter() {
  delete common_cpp_optics_reconstruction_minuit_track_fitter_minuit;
}

void MinuitTrackFitter::Fit(const Track & detector_events, Track & track) {
  detector_events_ = &detector_events;
  track_ = &track;

  // TODO(plane1@hawk.iit.edu) Fit to multiple masses or use mass as a fit
  // parameter to find the likely particle identity
  particle_id_ = Particle::kMuMinus;

  if (detector_events_->size() < 2) {
    throw(Squeal(Squeal::recoverable,
                 "Not enough track points to fit track (need at least two).",
                 "MAUS::MinuitTrackFitter::Fit()"));
  }

  // Find the start plane coordinates that minimize the score for the calculated
  // track based off of this track point (i.e. best fits the measured track
  // points from the detectors).
  TMinuit * minimiser
    = common_cpp_optics_reconstruction_minuit_track_fitter_minuit;
  // Int_t status = minimiser->Migrad();
std::cout << "CHECKPOINT Fit(): 1" << std::endl; std::cout.flush();
  minimiser->Migrad();
std::cout << "CHECKPOINT Fit(): 2" << std::endl; std::cout.flush();

  // TODO(plane1@hawk.iit.edu) Handle status from minimiser
}

Double_t MinuitTrackFitter::ScoreTrack(
    Double_t const * const start_plane_track_coordinates) {
std::cout << "CHECKPOINT ScoreTrack(): 0" << std::endl; std::cout.flush();
  // clear the last saved track
  track_->clear();
Track residuals;

  // Setup the start plane track point based on the Minuit initial conditions
  CovarianceMatrix null_uncertainties;
  const TrackPoint guess(start_plane_track_coordinates[0],
                         start_plane_track_coordinates[1],
                         start_plane_track_coordinates[2],
                         start_plane_track_coordinates[3],
                         start_plane_track_coordinates[4],
                         start_plane_track_coordinates[5],
                         particle_id_,
                         PhaseSpaceVector::PhaseSpaceType::kPositional);
std::cout << "DEBUG ScoreTrack(): particle ID = " << particle_id_ << std::endl;
  // If the guess is not physical then return a horrible score
  if (!ValidGuess(guess)) {
std::cout << "DEBUG ScoreTrack(): Returning maximum score for invalid guess." << std::endl;
//    return std::numeric_limits<double>::max();
    return 1.0e+15;
  }
  track_->push_back(guess);

  TransferMap const * transfer_map = NULL;
  CovarianceMatrix const * uncertainties = NULL;
  std::vector<TrackPoint>::const_iterator events
    = detector_events_->begin();

  // calculate chi^2
  Double_t chi_squared = 0.0;
  for (size_t index = 0; events < detector_events_->end(); ++index) {
std::cout << "DEBUG ScoreTrack(): Guess: " << guess << std::endl;
std::cout << "DEBUG ScoreTrack(): Measured: " << *events << std::endl;
    // calculate the next guess
std::cout << "CHECKPOINT ScoreTrack(): 1" << std::endl; std::cout.flush();
    transfer_map
      = optics_model_->GenerateTransferMap(events->z());
std::cout << "CHECKPOINT ScoreTrack(): 2" << std::endl; std::cout.flush();
    if (transfer_map == NULL) {
    throw(Squeal(Squeal::nonRecoverable,
                 "Got NULL transfer map.",
                 "MAUS::MinuitTrackFitter::ScoreTrack()"));
    }
    TrackPoint point = TrackPoint(
      transfer_map->Transport(guess),
      PhaseSpaceVector::PhaseSpaceType::kPositional);
std::cout << "DEBUG ScoreTrack(): Calculated: " << point << std::endl;

    uncertainties = &events->uncertainties();
    point.set_uncertainties(*uncertainties);

    // save the calculated track point in case this is the
    // last (best fitting) track
    track_->push_back(point);

    // Sum the squares of the differences between the calculated phase space
    // coordinates and the measured coordinates.
    TrackPoint residual = TrackPoint(
      point - (*events),
      PhaseSpaceVector::PhaseSpaceType::kPositional);
    chi_squared += (transpose(residual) * (*uncertainties) * residual)[0];
//std::cout << "DEBUG ScoreTrack(): Residual: " << residual << std::endl;
residuals.push_back(residual);
std::cout << "DEBUG ScoreTrack(): Uncertainties: " << *uncertainties << std::endl;
std::cout << "DEBUG ScoreTrack(): Chi Squared: " << chi_squared << std::endl;

    ++events;
  }

std::cout << "Residuals: " << std::endl << residuals;

  return chi_squared;
}

bool MinuitTrackFitter::ValidGuess(const TrackPoint & guess) const {
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
}  // namespace reconstruction
}  // namespace MAUS
