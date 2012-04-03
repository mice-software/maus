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

#ifndef COMMON_CPP_RECONSTRUCTION_RECONSTRUCTION_INPUT_HH
#define COMMON_CPP_RECONSTRUCTION_RECONSTRUCTION_INPUT_HH

#include <iostream>
#include <vector>

#include "src/common_cpp/Optics/ParticleTrack.hh"

namespace MAUS {

class ReconstructionInput {
 public:
  ReconstructionInput();

  /* @brief	Create with the given input values.
   */
  ReconstructionInput(std::vector<Detector> detectors,
                      std::vector<ParticleTrack> tracks);

  ~ReconstructionInput();

  void AddParameters(double detector_plane,
                     CovarianceMatrix const * const detector_uncertainties,
                     DetectorEvent
 protected:
  std::vector<double> detector_planes_;
  std::vector<CovarianceMatrix> detector_uncertainties_;
  std::vector<DetectorEvent> events_;
};

}  // namespace MAUS

#endif
