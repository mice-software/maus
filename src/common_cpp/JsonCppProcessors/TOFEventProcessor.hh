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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_TOFEVENTPROCESSOR_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_TOFEVENTPROCESSOR_

#include "src/common_cpp/DataStructure/TOFEvent.hh"
#include "src/common_cpp/JsonCppProcessors/ObjectProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/TOFEventDigitProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/TOFEventSlabHitProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/TOFEventSpacePointProcessor.hh"

namespace MAUS {

/** @class TOFEventProcessor Conversions for TOFEvent between C++ and Json 
 *
 *  @var _tof_slab_hits_proc_proc Processor for _tof_slab_hits
 *  @var _tof_space_points_proc_proc Processor for _tof_space_points
 *  @var _tof_digits_proc_proc Processor for _tof_digits
 */

class TOFEventProcessor : public ObjectProcessor<TOFEvent> {
  public:
    /** Set up processors and register branches
     *
     *  Everything else is handled by the base class
     */
    TOFEventProcessor();

  private:
    TOFEventSlabHitProcessor _tof_slab_hits_proc;
    TOFEventSpacePointProcessor _tof_space_points_proc;
    TOFEventDigitProcessor _tof_digits_proc;
};
}  // namespace MAUS

#endif  // #define _SRC_COMMON_CPP_JSONCPPPROCESSORS_TOFEVENTPROCESSOR_

