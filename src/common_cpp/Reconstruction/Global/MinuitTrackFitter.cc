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
fprintf(stdout, "CHECKPOINT: ..._score_function() 0"); fflush(stdout);
for (int index = 0; index < 6; ++index) {
  std::cout << "Parameter " << index+1 << " value: "
            << phase_space_coordinate_values[index] << std::endl;
}
  // common_cpp_optics_reconstruction_minuit_track_fitter_minuit is defined
  // globally in the header file
  TMinuit * minuit
    = common_cpp_optics_reconstruction_minuit_track_fitter_minuit;

  MinuitTrackFitter * track_fitter
    = static_cast<MinuitTrackFitter *>(minuit->GetObjectFit());
fprintf(stdout, "CHECKPOINT: ..._score_function() 1"); fflush(stdout);
  score = track_fitter->ScoreTrack(phase_space_coordinate_values);
fprintf(stdout, "CHECKPOINT: ..._score_function() 2"); fflush(stdout);
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
  minimiser->mnparm(0, "Time", 0., 0.1, -1000., 1., error_flag);   // ns
  minimiser->mnparm(1, "Energy", 200., 1, 105.7, 1860., error_flag);  // MeV
  minimiser->mnparm(2, "X", 0, 0.001, -150., 150., error_flag);      // mm
  minimiser->mnparm(3, "Px", 0., 0.1, -100., 100, error_flag);    // MeV/c
  minimiser->mnparm(4, "Y", 0, 0.001, -150., 150., error_flag);      // mm
  minimiser->mnparm(5, "Py", 0., 0.1, -100., 100, error_flag);    // MeV/c
}

MinuitTrackFitter::~MinuitTrackFitter() {
  delete common_cpp_optics_reconstruction_minuit_track_fitter_minuit;
}

void MinuitTrackFitter::Fit(const Track & detector_events, Track & track) {
fprintf(stdout, "CHECKPOINT: Fit() 0"); fflush(stdout);
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
fprintf(stdout, "CHECKPOINT: Fit() 1"); fflush(stdout);
  minimiser->Migrad();
fprintf(stdout, "CHECKPOINT: Fit() 2"); fflush(stdout);

  // TODO(plane1@hawk.iit.edu) Handle status from minimiser
}

Double_t MinuitTrackFitter::ScoreTrack(
    Double_t const * const start_plane_track_coordinates) {
  // clear the last saved track
  track_->clear();

  // Setup the start plane track point based on the Minuit initial conditions
  CovarianceMatrix null_uncertainties;
  const TrackPoint guess(start_plane_track_coordinates[0],
                         start_plane_track_coordinates[1],
                         start_plane_track_coordinates[2],
                         start_plane_track_coordinates[3],
                         start_plane_track_coordinates[4],
                         start_plane_track_coordinates[5],
                         particle_id_);
std::cout << "Guess: " << guess << std::endl;
  double start_plane = start_plane_;
fprintf(stdout, "CHECKPOINT: ScoreTrack() 0\n"); fflush(stdout);
  track_->push_back(guess);

  PhaseSpaceVector delta;  // difference between the guess and the measurement
  TransferMap const * transfer_map = NULL;
  CovarianceMatrix const * uncertainties = NULL;
  std::vector<TrackPoint>::const_iterator events
    = detector_events_->begin();

  // calculate chi^2
  Double_t chi_squared = 0.0;
  for (size_t index = 0; events < detector_events_->end(); ++index) {
    // calculate the next guess
    transfer_map
      = optics_model_->GenerateTransferMap(events->z());
fprintf(stdout, "CHECKPOINT: ScoreTrack() 1\n"); fflush(stdout);
if (transfer_map == NULL) {
  fprintf(stdout, "ERROR: transfer_map is NULL! Everybody panic!\n");
}
fprintf(stdout, "CHECKPOINT: ScoreTrack() 2\n"); fflush(stdout);
    TrackPoint point = TrackPoint(transfer_map->Transport(guess));
fprintf(stdout, "CHECKPOINT: ScoreTrack() 3\n"); fflush(stdout);
std::cout << "DEBUG ScoreTrack(): Station Point: " << point << std::endl;

    uncertainties = &events->uncertainties();
    point.set_uncertainties(*uncertainties);

    // save the calculated track point in case this is the
    // last (best fitting) track
    track_->push_back(point);

    // Sum the squares of the differences between the calculated phase space
    // coordinates and the measured coordinates.
    delta = TrackPoint(point - (*events));
std::cout << "DEBUG ScoreTrack(): event: " << (*events) << std::endl;
    chi_squared += (transpose(delta) * (*uncertainties) * delta)[0];
std::cout << "DEBUG ScoreTrack(): delta: " << delta << std::endl;
std::cout << "DEBUG ScoreTrack(): uncertainties: " << *uncertainties << std::endl;
std::cout << "DEBUG ScoreTrack(): chi_squared: " << chi_squared << std::endl;

    ++events;
  }

fprintf(stdout, "CHECKPOINT: ScoreTrack() 4"); fflush(stdout);
  return chi_squared;
}

const size_t MinuitTrackFitter::kPhaseSpaceDimension = 6;

}  // namespace global
}  // namespace reconstruction
}  // namespace MAUS
