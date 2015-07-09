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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_KLCELLHITPROCESSOR_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_KLCELLHITPROCESSOR_

#include "src/common_cpp/DataStructure/KLCellHit.hh"
#include "src/common_cpp/JsonCppProcessors/ObjectProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/PrimitivesProcessors.hh"

namespace MAUS {

/** @class KLCellHitProcessor Conversions for KLCellHit between C++ and Json 
 *
 *  @var _cell_proc Processor for _cell
 *  @var _phys_event_number_proc Processor for _phys_event_number
 *  @var _charge_proc Processor for _charge
 *  @var _charge_product_proc Processor for _charge_product
 *  @var _detector_proc Processor for _detector
 *  @var _part_event_number_proc Processor for _part_event_number
 *  @var _global_pos_proc Processor for _global_pos
 *  @var _local_pos_proc Processor for _local_pos
 *  @var _error_proc Processor for _error
 *  @var _flag_proc Processor for _flag
 */

class KLCellHitProcessor : public ObjectProcessor<KLCellHit> {
  public:
    /** Set up processors and register branches
     *
     *  Everything else is handled by the base class
     */
    KLCellHitProcessor();

  private:
    IntProcessor _cell_proc;
    IntProcessor _phys_event_number_proc;
    IntProcessor _charge_proc;
    IntProcessor _charge_product_proc;
    StringProcessor _detector_proc;
    IntProcessor _part_event_number_proc;
    DoubleProcessor _global_pos_x_proc;
    DoubleProcessor _global_pos_y_proc;
    DoubleProcessor _global_pos_z_proc;
    DoubleProcessor _local_pos_x_proc;
    DoubleProcessor _local_pos_y_proc;
    DoubleProcessor _local_pos_z_proc;
    DoubleProcessor _error_x_proc;
    DoubleProcessor _error_y_proc;
    DoubleProcessor _error_z_proc;
    BoolProcessor _flag_proc;
};
}  // namespace MAUS

#endif  // #define _SRC_COMMON_CPP_JSONCPPPROCESSORS_KLSLABHITPROCESSOR_

