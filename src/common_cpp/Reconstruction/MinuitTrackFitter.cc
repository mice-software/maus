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
 
#include <vector>

#include "TMinuit.h"

#include "Interface/Squeal.hh"
#include "src/common_cpp/Optics/CovarianceMatrix.hh"
#include "Optics/OpticsModel.hh"
#include "src/common_cpp/Optics/PhaseSpaceVector.hh"
#include "src/common_cpp/Optics/TransferMap.hh"
#include "Reconstruction/DetectorEvent.hh"
#include "Reconstruction/ParticleTrack.hh"
#include "Reconstruction/ParticleTrajectory.hh"

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
    OpticsModel const * const optics_model,
    std::vector<double> const * const detector_planes,
    std::vector<CovarianceMatrix> const * detector_uncertainties)
    : optics_model_(optics_model), detector_planes_(detector_planes),
      detector_uncertainties_(uncertainties) {
  // Setup *global* scope Minuit object
  common_cpp_optics_reconstruction_minuit_track_fitter_minuit
    = new TMinuit(kPhaseSpaceDimension);
  TMinuit * minimiser
    = common_cpp_optics_reconstruction_minuit_track_fitter_minuit;
  minimiser->SetMaxIteration(100);
  int error_flag = 0;
  // setup the index, name, init value, step size, min, and max value for each
  // phase space variable (mins and maxes calculated from 800MeV/c ISIS beam)
  minimiser->mnparm(0, "Time", 20, 0.1, 0.001, 100, error_flag);   // ns
  minimiser->mnparm(1, "Energy", 900, 1, 105.7, 1860, error_flag);  // MeV
  minimiser->mnparm(2, "X", 0, 0.001, -1.5, 1.5, error_flag);      // m
  minimiser->mnparm(3, "Px", 900, 0.1, 30., 1860, error_flag);    // MeV/c
  minimiser->mnparm(4, "Y", 0, 0.001, -1.5, 1.5, error_flag);      // m
  minimiser->mnparm(5, "Py", 900, 0.1, 30., 1860, error_flag);    // MeV/c
  minimiser->SetFitObject(this);
  minimiser->SetFCN(
    common_cpp_optics_reconstruction_minuit_track_fitter_score_function);
}

MinuitTrackFitter::~MinuitTrackFitter() {
  delete common_cpp_optics_reconstruction_minuit_track_fitter_minuit;
}

void MinuitTrackFitter::Fit(
    std::vector<DetectorEvent const *> const * const detector_events,
    ParticleTrajectory * const trajectory) {
  detector_events_ = detector_events;
  trajectory_ = trajectory;

  int particle_id = trajectory->particle_id();
  // FIXME(plane1@hake.iit.edu) assuming muons with m = 105.7 MeV/c^2.
  // Should use particle_id to look up the mass of the particle.
  mass_ = 105.7;

  if (detector_events->size() < 2) {
    throw(Squeal(Squeal::recoverable,
                 "Not enough tracks to fit trajectory (need at least two).",
                 "MAUS::MinuitTrackFitter::Fit()"));
  }

  // Find the start plane track that minimizes the score for the calculated
  // trajectory based off of this track (i.e. best fits the measured tracks
  // from the detectors).
  TMinuit * minimiser
    = common_cpp_optics_reconstruction_minuit_track_fitter_minuit;
  Int_t status = minimiser->Migrad();
  
  // TODO(plane1@hawk.iit.edu) Handle status from minimiser
}

Double_t MinuitTrackFitter::ScoreTrack(
    Double_t * start_plane_track_coordinates) {
  TransferMap transfer_map = optics_model_->transfer_map();

  // clear the last saved trajectory
  trajectory_->clear();

  // the first guess (calculated track) is the start plane track give by Minuit
  ParticleTrack guess(start_plane_track_coordinates[0],
                      start_plane_track_coordinates[1],
                      start_plane_track_coordinates[2],
                      start_plane_track_coordinates[3],
                      start_plane_track_coordinates[4],
                      start_plane_track_coordinates[5]);
  
  ParticleTrack guess; 
  PhaseSpaceVector delta;  // difference between the guess and the measurement

  std::vector<DetectorEvent const *>::const_iterator events = detector_events_->begin();
  std::vector<CovarianceMatrix>::const_iterator<CovarianceMatrix> errors = detector_uncertainties_->begin();
  double chi_squared = 0;
  while (events < detector_events_->end()) {
  // save the calculated trajectory in case this is the last one
  // (i.e. the best fit trajectory).
    trajectory_->push_back(guess);

    // sum the squares of the differences between the calculated phase space
    // coordinates and the measured coordinates
    delta = guess - (*events);
    chi_squared += delta * (*errors) * transpose(delta)

    // calculate the next event's phase space coordinates using the transfer map
    if (events > (*detector_events_).rbegin()) {
      transfer_map = optics_model_->transfer_map(detector_planes_[index],
                                                 detector_planes_[index+1],
                                                 mass_)
      guess = transfer_map.Transport(&guess);
    }

    ++events;
    ++errors;
  }
}

}  // namespace MAUS
