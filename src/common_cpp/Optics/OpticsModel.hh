#ifndef SRC_COMMON_CPP_OPTICS_OPTICS_MODEL_HH
#define SRC_COMMON_CPP_OPTICS_OPTICS_MODEL_HH

#include "src/common_cpp/Optics/TransferMap.hh"

namespace MAUS {

class OpticsModel
{
 public:
  virtual void Build(Json::Value * configuration) = 0;
  virtual TransferMap GenerateTransferMap(double z1, double z2) = 0;
 protected:
};

}

#endif
