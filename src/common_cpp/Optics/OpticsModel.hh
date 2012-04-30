#ifndef SRC_COMMON_CPP_OPTICS_OPTICS_MODEL_HH
#define SRC_COMMON_CPP_OPTICS_OPTICS_MODEL_HH

#include "src/common_cpp/Optics/TransferMap.hh"

namespace Json {
  class Value;
}

namespace MAUS {

class OpticsModel
{
 public:
  virtual void Build(const Json::Value & configuration) = 0;

  /* @brief Dynamically allocate a new TransferMap between two z-axis.
   *
   * The user of this function takes ownership of the dynamically allocated
   * memory and is responsible for deallocating it.
   */
  virtual TransferMap * GenerateTransferMap(
      double start_plane, double end_plane, double mass) const = 0;
 protected:
};

}

#endif
