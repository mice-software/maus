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

#include "src/common_cpp/DataStructure/Global/ReconEnums.hh"
#include "src/common_cpp/Optics/CovarianceMatrix.hh"
#include "Recon/Global/TrackFitter.hh"
#include "Recon/Global/Particle.hh"

namespace MAUS {

class OpticsModel;

namespace DataStructure {
namespace Global {
  class Track;
  class TrackPoint;
}
}

namespace recon {
namespace global {

// Minuit requires a gobal, static function to minimize. This requires a
// global instance of TMinuit to use TMinuit::GetObjectFit().
TMinuit * common_cpp_optics_recon_minuit_track_fitter_minuit = NULL;

void common_cpp_optics_recon_minuit_track_fitter_score_function(
    Int_t &    number_of_parameters,
    Double_t * gradiants,
    Double_t & function_value,
    Double_t * phase_space_coordinate_values,
    Int_t      execution_stage_flag);

class MinuitTrackFitter : public TrackFitter, public TObject {
 public:
  MinuitTrackFitter(MAUS::OpticsModel const * optics_model,
                    const double start_plane);

  // pure virtual function from TrackFitter base class
  void Fit(MAUS::DataStructure::Global::Track const * const raw_track,
           MAUS::DataStructure::Global::Track * const track,
           const std::string mapper_name);

  ~MinuitTrackFitter();

  Double_t ScoreTrack(Double_t * const start_plane_track_coordinates);
 protected:
  static const size_t kPhaseSpaceDimension;

  size_t rounds_;
  const std::vector<const MAUS::DataStructure::Global::TrackPoint *>
  detector_events_;
  std::vector<MAUS::DataStructure::Global::TrackPoint> reconstructed_points_;
  MAUS::DataStructure::Global::PID particle_id_;

  void ResetParameters();
  bool ValidVector(const MAUS::PhaseSpaceVector & guess) const;
  MinuitTrackFitter();
};

}  // namespace global
}  // namespace recon
}  // namespace MAUS

#endif
