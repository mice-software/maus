#include <vector>

#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/Image.hh"

#include "src/common_cpp/Recon/SciFi/LeastSquaresFitter.hh"
#include "src/common_cpp/Recon/SciFi/SimpleCircle.hh"

#include "src/reduce/ReduceCppTiltedHelix/ReduceCppTiltedHelix.hh"

namespace MAUS {

ReduceCppTiltedHelix::ReduceCppTiltedHelix(std::string& name) 
  : ReduceBase<Data, Image>(name) {}

ReduceCppTiltedHelix::~ReduceCppTiltedHelix() {}

void ReduceCppTiltedHelix::_birth(const std::string& config) {}

void ReduceCppTiltedHelix::_death() {}

MAUS::Image* ReduceCppTiltedHelix::process(MAUS::Data* data) {
    std::vector<ReconEvent*>* ev = data->GetSpill()->GetReconEvents();
    for (size_t i = 0; i < ev->size(); ++i) {
        std::vector<SciFiSpacePoint*> sps =
                                      ev->at(i)->GetSciFiEvent()->spacepoints();
        MAUS::SimpleCircle circle;
        LeastSquaresFitter::circle_fit(0.5, 0.5, 100., sps, circle);
        std::cerr << circle.get_x0() << " " << circle.get_y0() << std::endl; 
    }

    Image* image = new Image();
    return image;
}

}

