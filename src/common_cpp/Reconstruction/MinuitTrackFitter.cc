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
    std::vector<DetectorEvent> const * const detector_events
    std::vector<ParticleTrajectory> * trajectories) {

  // TODO(plane1@hawk.iit.edu) eventually we'll neeed to trick out which events
  // came from the same particle and compile a set of event sets. For now just
  // assume the events are all for the same particle.

  if (detector_events.size() < 2) {
    // TODO(plane1@hawk.iit.edu) Squeal: less than 2 tracks = no trajectory
  }

  // TODO(plane1@hawk.iit.edu) for each particle minimize the score function
  TMinuit * minimiser
    = common_cpp_optics_reconstruction_minuit_track_fitter_minuit;
  minimiser->SetMaxIteration(100);

  const_iterator events = detector_events->begin();
  last_detector_event = &(*events);
  while (events != detector_events.end())
  {
    Int_t status = minimiser->Migrad();
    last_detector_event = ++events;
  }

  // TODO(plane1@hawk.iit.edu) assemble trajectories for each particle
}

Double_t ScoreTrack(Double_t * phase_space_coordinates) {
  const PhaseSpaceVector first_detector_guess(phase_space_coordinates[0],
                                              phase_space_coordinates[1],
                                              phase_space_coordinates[2],
                                              phase_space_coordinates[3],
                                              phase_space_coordinates[4],
                                              phase_space_coordinates[5]);
  TransferMap transfer_map = optics_model_->transfer_map();

  // FIXME(plane1@hake.iit.edu) assuming muons with m = 105.7 MeV/c^2
  // should use PID to look up the mass of the particle
  double mass = 105.7;

  // 1) Get z coordinates of the detectors.
  std::vector<double> * detector_planes = GetDetectorPlanes();
  int number_of_detectors = detector_planes->size();
  // 2) Get measured PSV v^M_1 and error matrix E_1 from detector 1
  PhaseSpaceVector * measurement = GetDetectorMeasurement(0);
  CovarianceMatrix * detector_error = GetDetectorError(0);

  double chi_squared = 0;
  PhaseSpaceVector delta;
  TransferMap transfer_map;
  PhaseSpaceVector guess = first_detector_guess;
  for (int index = 0; index < number_of_detectors; ++index) {
    delta = guess - (*measurement);
    chi_squared += delta * (*detector_error) * transpose(delta)

    if (index < (number_of_detectors-1)) {
      transfer_map = optics_model_->transfer_map((*detector_planes)[index],
                                                 (*detector_planes)[index+1],
                                                 mass)
      guess = transfer_map.Transport(*measurement);
      measurement = GetDetectorMeasurement(index+1);
      detector_error = GetDetectorError(index+1);
    }
  }
}

std::vector<double> const * const GetDetectorPlanes() {
  // TODO(plane1@hawk.iit.edu) get z coordinates of available detectors
}

PhaseSpaceVector const * const GetDetectorMeasurement(int detector_index) {
  // TODO(plane1@hawk.iit.edu) get track of particle from detector i
}

CovarianceMatrix const * const GetDetectorError(int detector_index) {
  // TODO(plane1@hawk.iit.edu) get error matrix from detector i
}

}  // namespace MAUS
