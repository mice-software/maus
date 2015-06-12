
#ifndef SRC_REDUCE_REDUCECPPTILTEDHELIX_HH
#define SRC_REDUCE_REDUCECPPTILTEDHELIX_HH

#include <Python.h>

#include <string>

#include "src/common_cpp/API/ReduceBase.hh"
#include "src/common_cpp/API/PyWrapReduceBase.hh"

namespace MAUS {
class Image;
class Data;

class ReduceCppTiltedHelix : public ReduceBase<Data, Image> {
  public:
    ReduceCppTiltedHelix();
    ~ReduceCppTiltedHelix();

  private:

    void _birth(const std::string& config);

    void _death();

    Image* _process(Data* data);    
};

PyMODINIT_FUNC init_ReduceCppTiltedHelix(void) {
   PyWrapReduceBase<ReduceCppTiltedHelix>::PyWrapReduceBaseModInit(
                                  "ReduceCppTiltedHelix", "", "", "", "");
}

}

#endif // SRC_REDUCE_REDUCECPPTILTEDHELIX_HH

