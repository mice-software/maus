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

#ifndef SRC_REDUCE_REDUCECPPTILTEDHELIX_HH
#define SRC_REDUCE_REDUCECPPTILTEDHELIX_HH

#include <string>

#include "src/common_cpp/API/ReduceBase.hh"
#include "src/common_cpp/API/PyWrapReduceBase.hh"

namespace MAUS {
class ImageData;
class Data;

class ReduceCppTiltedHelix : public ReduceBase<Data, ImageData> {
  public:
    ReduceCppTiltedHelix();
    ~ReduceCppTiltedHelix();

  private:

    void _birth(const std::string& config);

    void _death();

    ImageData* _process(Data* data);

    bool will_cut(std::vector<SciFiSpacePoint*> space_points);

    void do_fit(std::vector<SciFiSpacePoint*> space_points);

    ImageData* get_image_data();

    std::vector<TH1D> hist_vector_;
};

PyMODINIT_FUNC init_ReduceCppTiltedHelix(void) {
    PyWrapReduceBase<ReduceCppTiltedHelix>::PyWrapReduceBaseModInit(
                                  "ReduceCppTiltedHelix", "", "", "", "");
}
}

#endif // SRC_REDUCE_REDUCECPPTILTEDHELIX_HH

