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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_TOFSLABHITPROCESSOR_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_TOFSLABHITPROCESSOR_

#include "src/common_cpp/DataStructure/TOFSlabHit.hh"
#include "src/common_cpp/JsonCppProcessors/ObjectProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/Pmt0Processor.hh"
#include "src/common_cpp/JsonCppProcessors/Pmt1Processor.hh"
#include "src/common_cpp/JsonCppProcessors/PrimitivesProcessors.hh"

namespace MAUS {

/** @class TOFSlabHitProcessor Conversions for TOFSlabHit between C++ and Json 
 *
 *  @var _slab_proc_proc Processor for _slab
 *  @var _phys_event_number_proc_proc Processor for _phys_event_number
 *  @var _raw_time_proc_proc Processor for _raw_time
 *  @var _charge_proc_proc Processor for _charge
 *  @var _plane_proc_proc Processor for _plane
 *  @var _charge_product_proc_proc Processor for _charge_product
 *  @var _time_proc_proc Processor for _time
 *  @var _station_proc_proc Processor for _station
 *  @var _detector_proc_proc Processor for _detector
 *  @var _part_event_number_proc_proc Processor for _part_event_number
 *  @var _pmt1_proc_proc Processor for _pmt1
 *  @var _pmt0_proc_proc Processor for _pmt0
 */

class TOFSlabHitProcessor : public ObjectProcessor<TOFSlabHit> {
  public:
    /** Set up processors and register branches
     *
     *  Everything else is handled by the base class
     */
    TOFSlabHitProcessor();

  private:
    IntProcessor _slab_proc;
    IntProcessor _phys_event_number_proc;
    DoubleProcessor _raw_time_proc;
    IntProcessor _charge_proc;
    IntProcessor _plane_proc;
    IntProcessor _charge_product_proc;
    DoubleProcessor _time_proc;
    IntProcessor _station_proc;
    StringProcessor _detector_proc;
    IntProcessor _part_event_number_proc;
    Pmt1Processor _pmt1_proc;
    Pmt0Processor _pmt0_proc;
    DoubleProcessor _global_x_proc;
    DoubleProcessor _global_y_proc;
    DoubleProcessor _global_z_proc;
    DoubleProcessor _global_x_err_proc;
    DoubleProcessor _global_y_err_proc;
    DoubleProcessor _global_z_err_proc;
    BoolProcessor _horizontal_slab_proc;
    BoolProcessor _vertical_slab_proc;
};
}  // namespace MAUS

#endif  // #define _SRC_COMMON_CPP_JSONCPPPROCESSORS_TOFSLABHITPROCESSOR_

