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

#include "Reconstruction/ParticleTrajectory.hh"

#include "TMinuit.h"

#include "Interface/Squeak.hh"

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

MinuitTrackFitter::MinuitTrackFitter(OpticsModel const * const optics_model)
    : optics_model_(optics_model) {
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

  GetDetectorPlanes(&detector_planes_);

  GetDetectorErrors(&detector_errors_);
}

MinuitTrackFitter::~MinuitTrackFitter() {
  delete common_cpp_optics_reconstruction_minuit_track_fitter_minuit;
}
void MinuitTrackFitter::Fit(
    std::vector<DetectorEvent> const * const detector_events
    std::vector<ParticleTrajectory> * const trajectories) {
 
  // determine which events came from which particles
  std::vector<std::vector<DetectorEvent> > event_sets;
  CorrelateDetectorEvents(detector_events, &event_sets);

  if (detector_events.size() < 2) {
    // TODO(plane1@hawk.iit.edu) Squeal: less than 2 tracks = no trajectory
  }

  // Find the best fit trajectory for each particle traversing the lattice
  const_iterator<std::vector<DetectorEvent> > trajectories;
  for (trajectories = event_sets.begin();
       trajectories < event_sets.end();
       ++trajectories) {
    // Find the start plane track that minimizes the score for the calculated
    // trajectory based off of this track (i.e. best fits the measured tracks
    // from the detectors).
    TMinuit * minimiser
      = common_cpp_optics_reconstruction_minuit_track_fitter_minuit;
    Int_t status = minimiser->Migrad();
  }
}

Double_t MinuitTrackFitter::ScoreTrack(Double_t * phase_space_coordinates) {
  TransferMap transfer_map = optics_model_->transfer_map();

  // FIXME(plane1@hake.iit.edu) assuming muons with m = 105.7 MeV/c^2.
  // Should use PID to look up the mass of the particle.
  double mass = 105.7;

  const PhaseSpaceVector start_plane_track(phase_space_coordinates[0],
                                              phase_space_coordinates[1],
                                              phase_space_coordinates[2],
                                              phase_space_coordinates[3],
                                              phase_space_coordinates[4],
                                              phase_space_coordinates[5]);
  PhaseSpaceVector guess = start_plane_track;

  PhaseSpaceVector delta;
  PhaseSpaceVector measurement;

  const_iterator<DetectorEvent> events = (*trajectories_).begin();
  const_iterator<DetectorEvent> errors = detector_errors_.begin();
  double chi_squared = 0;
  while (events < (*trajectories_).end()) {
    // get the measured phase space coordinates of the event
    measurement = (*events).coordinates();

    // sum the squares of the differences between the calculated phase space
    // coordinates and the measured coordinates
    delta = guess - measurement;
    chi_squared += delta * (*errors) * transpose(delta)

    // calculate the next event's phase space coordinates using the transfer map
    if (events > (*trajectories_).rbegin()) {
      transfer_map = optics_model_->transfer_map(detector_planes_[index],
                                                 detector_planes_[index+1],
                                                 mass)
      guess = transfer_map.Transport(&guess);
    }

    ++events;
    ++errors;
  }

  // TODO(plane1@hawk.iit.edu) save the calculate trajectory in case this is the
  // last one (i.e. the best fit trajectory).
}

void MinuitTrackFitter::CorrelateDetectorEvents(
  std::vector<DetectorEvent> const * const detector_events,
  std::vector<std::vector<DetectorEvent> > * &event_sets) {
  // TODO(plane1@hawk.iit.edu) create sets of events where each set corresponds
  // to a different particle. For now assume that all events are from the same
  // particle.
  event_sets->assign(detector_events.begin(), detector_events.end());
}

void MinuitTrackFitter::GetDetectorPlanes(std::vector<double> * detector_planes) {
  // TODO(plane1@hawk.iit.edu) get z coordinates of available detectors
}

void MinuitTrackFitter::GetDetectorErrors(std::vector<CovarianceMatrix> * error_matrices) {
  // TODO(plane1@hawk.iit.edu) get error matrices of available detectors
}

}  // namespace MAUS
