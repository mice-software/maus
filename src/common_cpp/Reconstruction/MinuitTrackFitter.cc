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

#include "Reconstruction/MinuitTrackFitter.hh"
 
#include <cmath>
#include <vector>

#include "TMinuit.h"

#include "Interface/Squeal.hh"
#include "src/common_cpp/Optics/CovarianceMatrix.hh"
#include "src/common_cpp/Optics/OpticsModel.hh"
#include "src/common_cpp/Optics/PhaseSpaceVector.hh"
#include "src/common_cpp/Optics/TransferMap.hh"
#include "Reconstruction/Particle.hh"
#include "Reconstruction/Track.hh"
#include "Reconstruction/TrackPoint.hh"

namespace MAUS {

void common_cpp_optics_reconstruction_minuit_track_fitter_score_function(
    Int_t &    number_of_parameters,
    Double_t * gradiants,
    Double_t & score,
    Double_t * phase_space_coordinate_values,
    Int_t      execution_stage_flag) {
  // common_cpp_optics_reconstruction_minuit_track_fitter_minuit is defined
  // globally in the header file
  TMinuit * minuit
    = common_cpp_optics_reconstruction_minuit_track_fitter_minuit;

  MinuitTrackFitter * track_fitter
    = (MinuitTrackFitter *) minuit->GetObjectFit();
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
  int error_flag = 0;
  // setup the index, name, init value, step size, min, and max value for each
  // phase space variable (mins and maxes calculated from 800MeV/c ISIS beam)
  minimiser->mnparm(0, "Time", 20, 0.1, 0.001, 100, error_flag);   // ns
  minimiser->mnparm(1, "Energy", 900, 1, 105.7, 1860, error_flag);  // MeV
  minimiser->mnparm(2, "X", 0, 0.001, -1.5, 1.5, error_flag);      // m
  minimiser->mnparm(3, "Px", 900, 0.1, 30., 1860, error_flag);    // MeV/c
  minimiser->mnparm(4, "Y", 0, 0.001, -1.5, 1.5, error_flag);      // m
  minimiser->mnparm(5, "Py", 900, 0.1, 30., 1860, error_flag);    // MeV/c
  minimiser->SetObjectFit(this);
  minimiser->SetFCN(
    common_cpp_optics_reconstruction_minuit_track_fitter_score_function);
}

MinuitTrackFitter::~MinuitTrackFitter() {
  delete common_cpp_optics_reconstruction_minuit_track_fitter_minuit;
}

void MinuitTrackFitter::Fit(const Track & detector_events, Track & track) {
  detector_events_ = &detector_events;
  track_ = &track;

  int particle_id = detector_events_->particle_id();
  mass_ = Particle::GetInstance()->GetMass(particle_id);

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
  //Int_t status = minimiser->Migrad();
  minimiser->Migrad();
  
  // TODO(plane1@hawk.iit.edu) Handle status from minimiser
}

Double_t MinuitTrackFitter::ScoreTrack(
    Double_t const * const start_plane_track_coordinates) {
  // clear the last saved track
  track_->clear();
  
  // Setup the start plane track point based on the Minuit initial conditions
  TrackPoint guess(start_plane_track_coordinates[0],
                   start_plane_track_coordinates[1],
                   start_plane_track_coordinates[2],
                   start_plane_track_coordinates[3],
                   start_plane_track_coordinates[4],
                   start_plane_track_coordinates[5],
                   start_plane_, 0.0, CovarianceMatrix());
  guess.FillInAxialCoordinates(mass_);
  double start_plane = start_plane_;
  track_->push_back(guess);

  PhaseSpaceVector delta;  // difference between the guess and the measurement
  TransferMap const * transfer_map = NULL;
  CovarianceMatrix const * uncertainties = &guess.uncertainties();
  std::vector<TrackPoint>::const_iterator events
    = detector_events_->begin();

  // calculate chi^2
  Double_t chi_squared = 0.0;
  for (size_t index = 0; events < detector_events_->end(); ++index) {
    // calculate the next guess
    transfer_map
      = optics_model_->GenerateTransferMap(start_plane, events->z(), mass_);
    guess = TrackPoint(transfer_map->Transport(guess));
    delete transfer_map;

    guess.FillInAxialCoordinates(mass_);
    uncertainties = &(*events).uncertainties();
    guess.set_uncertainties(*uncertainties);

    // save the calculated track in case this is the last one
    // (i.e. the best fit track).
    track_->push_back(guess);

    // Sum the squares of the differences between the calculated phase space
    // coordinates and the measured coordinates.
    delta = TrackPoint(guess - (*events));
    chi_squared += (transpose(delta) * (*uncertainties) * delta)[0];

    start_plane = events->z();
    ++events;
  }

  return chi_squared;
}

const size_t MinuitTrackFitter::kPhaseSpaceDimension = 6;

}  // namespace MAUS
