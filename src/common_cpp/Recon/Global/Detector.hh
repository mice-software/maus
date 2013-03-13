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

#include "src/common_cpp/DataStructure/Global/ReconEnums.hh"
#include "src/common_cpp/Optics/CovarianceMatrix.hh"
namespace MAUS {
namespace recon {
namespace global {

class Detector {
 public:
  Detector(const MAUS::DataStructure::Global::DetectorPoint id,
           const double plane,
           const CovarianceMatrix & uncertainties);

  Detector(const Detector & original_instance)
      : id_(original_instance.id_), plane_(original_instance.plane_),
        uncertainties_(original_instance.uncertainties_) { }

  ~Detector();

  const MAUS::DataStructure::Global::DetectorPoint id() const;
  const double plane() const;
  const CovarianceMatrix & uncertainties() const;
 protected:
  Detector();

  MAUS::DataStructure::Global::DetectorPoint id_;
  double plane_;
  CovarianceMatrix uncertainties_;
};

}  // namespace global
}  // namespace recon
}  // namespace MAUS

#endif
