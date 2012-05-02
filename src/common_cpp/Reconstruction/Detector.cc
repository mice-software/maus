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

#include "src/common_cpp/Reconstruction/ReconstructionInput.hh"

namespace MAUS {

Detector::Detector(const unsigned int id,
                   const double plane,
                   const CovarianceMatrix & uncertainties)
    : id_(id), plane_(plane), uncertainties_(uncertainties) {
}

Detector::~Detector() {
}

const unsigned int Detector::id() const {
  return id_;
}

const double Detector::plane() const {
  return plane_;
}

const CovarianceMatrix & Detector::uncertainties() const {
  return uncertainties_;
}

const unsigned int Detector::kNone = 0;
const unsigned int Detector::kTOF0 = 1;
const unsigned int Detector::kCherenkov1 = 2;
const unsigned int Detector::kTOF1 = 3;
const unsigned int Detector::kTracker1_1 = 4;
const unsigned int Detector::kTracker1_2 = 5;
const unsigned int Detector::kTracker1_3 = 6;
const unsigned int Detector::kTracker1_4 = 7;
const unsigned int Detector::kTracker1_5 = 8;
const unsigned int Detector::kTracker2_1 = 9;
const unsigned int Detector::kTracker2_2 = 10;
const unsigned int Detector::kTracker2_3 = 11;
const unsigned int Detector::kTracker2_4 = 12;
const unsigned int Detector::kTracker2_5 = 13;
const unsigned int Detector::kTOF2 = 14;
const unsigned int Detector::kCherenkov2 = 15;
const unsigned int Detector::kCalorimeter = 16;

}  // namespace MAUS

