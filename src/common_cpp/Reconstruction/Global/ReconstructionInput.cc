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

#include <iostream>
#include <vector>

#include "src/common_cpp/Reconstruction/Global/ReconstructionInput.hh"

namespace MAUS {
namespace reconstruction {
namespace global {

ReconstructionInput::ReconstructionInput(
    const bool beam_polarity_negative,
    const std::vector<Detector> & detectors,
    const std::vector<TrackPoint> & events)
    : beam_polarity_negative_(beam_polarity_negative),
      detectors_(detectors), events_(events) {
}

bool ReconstructionInput::beam_polarity_negative() {
  return beam_polarity_negative_;
}

const std::vector<Detector> & ReconstructionInput::detectors() {
  return detectors_;
}

const std::vector<TrackPoint> & ReconstructionInput::events() {
  return events_;
}

}  // namespace global
}  // namespace reconstruction
}  // namespace MAUS

