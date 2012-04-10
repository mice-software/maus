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

#ifndef SRC_COMMON_CPP_RECONSTRUCTION_TRACK_FITTER_HH
#define SRC_COMMON_CPP_RECONSTRUCTION_TRACK_FITTER_HH

#include <vector>

#include "DetectorEvent.hh"
#include "ParticleTrajectory.hh"

namespace MAUS {

class OpticsModel;

class TrackFitter
{
 public:
  virtual void Fit(std::vector<DetectorEvent const *> const * const detector_events
                   ParticleTrajectory * const trajectory) = 0;
 protected:
  OpticsModel const * optics_model_;
  std::vector<double> const * detector_planes_;
  std::vector<CovarianceMatrix> const * detector_uncertainties_;
};

}

#endif
