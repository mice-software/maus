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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_TOFDIGITPROCESSOR_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_TOFDIGITPROCESSOR_

#include "src/common_cpp/DataStructure/TOFDigit.hh"
#include "src/common_cpp/JsonCppProcessors/ObjectProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/PrimitivesProcessors.hh"

namespace MAUS {

/** @class TOFDigitProcessor Conversions for TOFDigit between C++ and Json 
 *
 *  @var _pmt_proc_proc Processor for _pmt
 *  @var _trigger_leading_time_proc_proc Processor for _trigger_leading_time
 *  @var _trigger_request_leading_time_proc_proc Processor for _trigger_request_leading_time
 *  @var _trigger_trailing_time_proc_proc Processor for _trigger_trailing_time
 *  @var _trigger_request_trailing_time_proc_proc Processor for _trigger_request_trailing_time
 *  @var _charge_mm_proc_proc Processor for _charge_mm
 *  @var _trailing_time_proc_proc Processor for _trailing_time
 *  @var _plane_proc_proc Processor for _plane
 *  @var _charge_pm_proc_proc Processor for _charge_pm
 *  @var _phys_event_number_proc_proc Processor for _phys_event_number
 *  @var _station_proc_proc Processor for _station
 *  @var _leading_time_proc_proc Processor for _leading_time
 *  @var _time_stamp_proc_proc Processor for _time_stamp
 *  @var _tof_key_proc_proc Processor for _tof_key
 *  @var _trigger_time_tag_proc_proc Processor for _trigger_time_tag
 *  @var _part_event_number_proc_proc Processor for _part_event_number
 *  @var _slab_proc_proc Processor for _slab
 */

class TOFDigitProcessor : public ObjectProcessor<TOFDigit> {
  public:
    /** Set up processors and register branches
     *
     *  Everything else is handled by the base class
     */
    TOFDigitProcessor();

  private:
    IntProcessor _pmt_proc;
    IntProcessor _trigger_leading_time_proc;
    IntProcessor _trigger_request_leading_time_proc;
    IntProcessor _trigger_trailing_time_proc;
    IntProcessor _trigger_request_trailing_time_proc;
    IntProcessor _charge_mm_proc;
    IntProcessor _trailing_time_proc;
    IntProcessor _plane_proc;
    IntProcessor _charge_pm_proc;
    IntProcessor _phys_event_number_proc;
    IntProcessor _station_proc;
    IntProcessor _leading_time_proc;
    IntProcessor _time_stamp_proc;
    StringProcessor _tof_key_proc;
    IntProcessor _trigger_time_tag_proc;
    IntProcessor _part_event_number_proc;
    IntProcessor _slab_proc;
};
}  // namespace MAUS

#endif  // #define _SRC_COMMON_CPP_JSONCPPPROCESSORS_TOFDIGITPROCESSOR_

