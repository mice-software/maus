/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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
 */

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_TOFEVENTSPACEPOINTPROCESSOR_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_TOFEVENTSPACEPOINTPROCESSOR_

#include "src/common_cpp/DataStructure/TOFEventSpacePoint.hh"
#include "src/common_cpp/JsonCppProcessors/ArrayProcessors.hh"
#include "src/common_cpp/JsonCppProcessors/ObjectProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/TOFSpacePointProcessor.hh"

namespace MAUS {

/** @class TOFEventSpacePointProcessor Conversions for TOFEventSpacePoint between C++ and Json 
 *
 *  @var _tof1_proc_proc Processor for _tof1
 *  @var _tof0_proc_proc Processor for _tof0
 *  @var _tof2_proc_proc Processor for _tof2
 */

class TOFEventSpacePointProcessor : public ObjectProcessor<TOFEventSpacePoint> {
  public:
    /** Set up processors and register branches
     *
     *  Everything else is handled by the base class
     */
    TOFEventSpacePointProcessor();

  private:
    ValueArrayProcessor<TOFSpacePoint> _tof0_proc;
    ValueArrayProcessor<TOFSpacePoint> _tof1_proc;
    ValueArrayProcessor<TOFSpacePoint> _tof2_proc;
};
}  // namespace MAUS

#endif  // #define _SRC_COMMON_CPP_JSONCPPPROCESSORS_TOFEVENTSPACEPOINTPROCESSOR_

