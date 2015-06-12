/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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

#include <Python.h>

#include <vector>

#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/ImageData.hh"

#include "src/common_cpp/Recon/SciFi/LeastSquaresFitter.hh"
#include "src/common_cpp/Recon/SciFi/SimpleCircle.hh"

#include "src/reduce/ReduceCppTiltedHelix/ReduceCppTiltedHelix.hh"

namespace MAUS {

ReduceCppTiltedHelix::ReduceCppTiltedHelix() 
  : ReduceBase<Data, ImageData>("ReduceCppTiltedHelix") {}

ReduceCppTiltedHelix::~ReduceCppTiltedHelix() {}

void ReduceCppTiltedHelix::_birth(const std::string& config) {}

void ReduceCppTiltedHelix::_death() {}

MAUS::ImageData* ReduceCppTiltedHelix::_process(MAUS::Data* data) {
    std::vector<ReconEvent*>* ev = data->GetSpill()->GetReconEvents();
    for (size_t i = 0; i < ev->size(); ++i) {
        std::vector<SciFiSpacePoint*> sps =
                                      ev->at(i)->GetSciFiEvent()->spacepoints();
        MAUS::SimpleCircle circle;
        LeastSquaresFitter::circle_fit(0.5, 0.5, 100., sps, circle);
        std::cerr << circle.get_x0() << " " << circle.get_y0() << std::endl; 
    }

    ImageData* image = new ImageData();
    return image;
}

}

