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

#include "src/common_cpp/Recon/Global/Detector.hh"

#include <iostream>
#include <vector>

namespace MAUS {
namespace recon {
namespace global {

Detector::Detector(const ID id,
                   const double plane,
                   const CovarianceMatrix & uncertainties)
    : id_(id), plane_(plane), uncertainties_(uncertainties) {
}

Detector::~Detector() {
}

const Detector::ID Detector::id() const {
  return id_;
}

const double Detector::plane() const {
  return plane_;
}

const CovarianceMatrix & Detector::uncertainties() const {
  return uncertainties_;
}

}  // namespace global
}  // namespace recon
}  // namespace MAUS

