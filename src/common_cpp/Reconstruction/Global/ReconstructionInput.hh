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

#include <map>
#include <vector>

#include "src/common_cpp/Reconstruction/Global/Detector.hh"
#include "src/common_cpp/Reconstruction/Global/TrackPoint.hh"

namespace MAUS {
namespace reconstruction {
namespace global {

class ReconstructionInput {
 public:
  /* @brief	Create with the given input values.
   */
  ReconstructionInput(const bool beam_polarity_negative,
                      const std::map<int, Detector> & detectors,
                      const std::vector<TrackPoint> & events);

  ~ReconstructionInput() { }

  bool beam_polarity_negative();
  const std::map<int, Detector> & detectors();
  const std::vector<TrackPoint> & events();
 protected:
  ReconstructionInput();

  bool beam_polarity_negative_;
  std::map<int, Detector> detectors_;
  std::vector<TrackPoint> events_;
};

}  // namespace global
}  // namespace reconstruction
}  // namespace MAUS

#endif
