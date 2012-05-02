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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_V1724PROCESSOR_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_V1724PROCESSOR_

#include "src/common_cpp/DataStructure/V1724.hh"
#include "src/common_cpp/JsonCppProcessors/ObjectProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/PrimitivesProcessors.hh"

namespace MAUS {

/** @class V1724Processor Conversions for V1724 between C++ and Json 
 *
 *  @var _ldc_id_proc_proc Processor for _ldc_id
 *  @var _charge_mm_proc_proc Processor for _charge_mm
 *  @var _equip_type_proc_proc Processor for _equip_type
 *  @var _channel_key_proc_proc Processor for _channel_key
 *  @var _charge_pm_proc_proc Processor for _charge_pm
 *  @var _phys_event_number_proc_proc Processor for _phys_event_number
 *  @var _position_max_proc_proc Processor for _position_max
 *  @var _time_stamp_proc_proc Processor for _time_stamp
 *  @var _trigger_time_tag_proc_proc Processor for _trigger_time_tag
 *  @var _detector_proc_proc Processor for _detector
 *  @var _part_event_number_proc_proc Processor for _part_event_number
 *  @var _geo_proc_proc Processor for _geo
 *  @var _pedestal_proc_proc Processor for _pedestal
 *  @var _channel_proc_proc Processor for _channel
 */

class V1724Processor : public ObjectProcessor<V1724> {
  public:
    /** Set up processors and register branches
     *
     *  Everything else is handled by the base class
     */
    V1724Processor();

  private:
    IntProcessor _ldc_id_proc;
    IntProcessor _charge_mm_proc;
    IntProcessor _equip_type_proc;
    StringProcessor _channel_key_proc;
    IntProcessor _charge_pm_proc;
    IntProcessor _phys_event_number_proc;
    IntProcessor _position_max_proc;
    IntProcessor _time_stamp_proc;
    IntProcessor _trigger_time_tag_proc;
    StringProcessor _detector_proc;
    IntProcessor _part_event_number_proc;
    IntProcessor _geo_proc;
    IntProcessor _pedestal_proc;
    IntProcessor _channel_proc;
};
}  // namespace MAUS

#endif  // #define _SRC_COMMON_CPP_JSONCPPPROCESSORS_V1724PROCESSOR_

