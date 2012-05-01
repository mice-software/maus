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

#include "TrackPoint.hh"
#include "Track.hh"

namespace MAUS {

class OpticsModel;

class TrackFitter
{
 public:
  TrackFitter(const OpticsModel & optics_model, const double start_plane)
      : optics_model_(&optics_model), start_plane_(start_plane) { }

  virtual void Fit(const Track & detector_events, Track & track) = 0;
 protected:
  OpticsModel const * optics_model_;
  const double start_plane_;

  TrackFitter() : optics_model_(NULL), start_plane_(0.0) { };
};

}

#endif
