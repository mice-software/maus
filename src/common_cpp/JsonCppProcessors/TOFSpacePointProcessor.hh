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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_TOFSPACEPOINTPROCESSOR_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_TOFSPACEPOINTPROCESSOR_

#include "src/common_cpp/DataStructure/TOFSpacePoint.hh"
#include "src/common_cpp/JsonCppProcessors/ObjectProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/PrimitivesProcessors.hh"

namespace MAUS {

/** @class TOFSpacePointProcessor Conversions for TOFSpacePoint between C++ and Json 
 *
 *  @var _phys_event_number_proc_proc Processor for _phys_event_number
 *  @var _pixel_key_proc_proc Processor for _pixel_key
 *  @var _charge_proc_proc Processor for _charge
 *  @var _station_proc_proc Processor for _station
 *  @var _slabY_proc_proc Processor for _slabY
 *  @var _slabX_proc_proc Processor for _slabX
 *  @var _charge_product_proc_proc Processor for _charge_product
 *  @var _time_proc_proc Processor for _time
 *  @var _detector_proc_proc Processor for _detector
 *  @var _part_event_number_proc_proc Processor for _part_event_number
 *  @var _dt_proc_proc Processor for _dt
 */

class TOFSpacePointProcessor : public ObjectProcessor<TOFSpacePoint> {
  public:
    /** Set up processors and register branches
     *
     *  Everything else is handled by the base class
     */
    TOFSpacePointProcessor();

  private:
    IntProcessor _phys_event_number_proc;
    StringProcessor _pixel_key_proc;
    IntProcessor _charge_proc;
    IntProcessor _station_proc;
    IntProcessor _slabY_proc;
    IntProcessor _slabX_proc;
    IntProcessor _charge_product_proc;
    DoubleProcessor _time_proc;
    StringProcessor _detector_proc;
    IntProcessor _part_event_number_proc;
    DoubleProcessor _dt_proc;
    DoubleProcessor _global_x_proc;
    DoubleProcessor _global_y_proc;
    DoubleProcessor _global_z_proc;
    DoubleProcessor _global_x_err_proc;
    DoubleProcessor _global_y_err_proc;
    DoubleProcessor _global_z_err_proc;
};
}  // namespace MAUS

#endif  // #define _SRC_COMMON_CPP_JSONCPPPROCESSORS_TOFSPACEPOINTPROCESSOR_

