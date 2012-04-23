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

#ifndef COMMON_CPP_RECONSTRUCTION_DETECTOR_HH
#define COMMON_CPP_RECONSTRUCTION_DETECTOR_HH

#include <iostream>
#include <vector>

namespace MAUS {

class ReconstructionInput {
 public:
  /* @brief	Create with the given input values.
   */
  Detector(const unsigned int id,
           const double plane,
           const CovarianceMatrix const * const uncertainties);

  ~Detector();

  const unsigned int id();
  const double plane();
  const CovarianceMatrix const * uncertainties();

  static const unsigned int kTOF0;
  static const unsigned int kCherenkov1;
  static const unsigned int kTOF1;
  static const unsigned int kTracker1;
  static const unsigned int kTracker2;
  static const unsigned int kCherenkov2;
  static const unsigned int kCalorimeter;
 protected:
  Detector();

  unsigned int id_;
  double plane_;
  CovarianceMatrix uncertainties_;
};

const unsigned int Detector::None = 0;
const unsigned int Detector::TOF0 = 1;
const unsigned int Detector::Cherenkov1 = 2;
const unsigned int Detector::TOF1 = 3;
const unsigned int Detector::Tracker1 = 4;
const unsigned int Detector::Tracker2 = 5;
const unsigned int Detector::Cherenkov2 = 6;
const unsigned int Detector::Calorimeter = 7;

}  // namespace MAUS

#endif
