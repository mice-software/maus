/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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
/* Author: Peter Lane
 */

#ifndef SRC_COMMON_CPP_OPTICS_RECONSTRUCTION_MINUIT_TRACK_FITTER_HH
#define SRC_COMMON_CPP_OPTICS_RECONSTRUCTION_MINUIT_TRACK_FITTER_HH

#include <vector>

#include "TMinuit.h"
#include "TObject.h"

#include "src/common_cpp/Optics/CovarianceMatrix.hh"
#include "Reconstruction/Global/TrackFitter.hh"

namespace MAUS {

class OpticsModel;

namespace reconstruction {
namespace global {

class Track;

// Minuit requires a gobal, static function to minimize. This requires a
// global instance of TMinuit to use TMinuit::GetObjectFit().
TMinuit * common_cpp_optics_reconstruction_minuit_track_fitter_minuit = NULL;

void common_cpp_optics_reconstruction_minuit_track_fitter_score_function(
    Int_t &    number_of_parameters,
    Double_t * gradiants,
    Double_t & function_value,
    Double_t * phase_space_coordinate_values,
    Int_t      execution_stage_flag);

class MinuitTrackFitter : public TrackFitter, public TObject {
 public:
  MinuitTrackFitter(const MAUS::OpticsModel & optics_model, const double start_plane);

  // pure virtual function from TrackFitter base class
  void Fit(const Track & detector_events, Track & track);

  ~MinuitTrackFitter();

  Double_t ScoreTrack(Double_t * const start_plane_track_coordinates);
 protected:
  static const size_t kPhaseSpaceDimension;

  Track const * detector_events_;
  Track * track_;
  int particle_id_;

  bool ValidGuess(const TrackPoint & guess) const;
  MinuitTrackFitter();
};

}  // namespace global
}  // namespace reconstruction
}  // namespace MAUS

#endif
