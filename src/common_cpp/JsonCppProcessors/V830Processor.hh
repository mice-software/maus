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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_V830PROCESSOR_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_V830PROCESSOR_

#include "src/common_cpp/DataStructure/V830.hh"
#include "src/common_cpp/JsonCppProcessors/ChannelsProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/ObjectProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/PrimitivesProcessors.hh"

namespace MAUS {

/** @class V830Processor Conversions for V830 between C++ and Json 
 *
 *  @var _ldc_id_proc_proc Processor for _ldc_id
 *  @var _equip_type_proc_proc Processor for _equip_type
 *  @var _channels_proc_proc Processor for _channels
 *  @var _phys_event_number_proc_proc Processor for _phys_event_number
 *  @var _time_stamp_proc_proc Processor for _time_stamp
 *  @var _geo_proc_proc Processor for _geo
 */

class V830Processor : public ObjectProcessor<V830> {
  public:
    /** Set up processors and register branches
     *
     *  Everything else is handled by the base class
     */
    V830Processor();

  private:
    IntProcessor _ldc_id_proc;
    IntProcessor _equip_type_proc;
    ChannelsProcessor _channels_proc;
    IntProcessor _phys_event_number_proc;
    IntProcessor _time_stamp_proc;
    IntProcessor _geo_proc;
};
}  // namespace MAUS

#endif  // #define _SRC_COMMON_CPP_JSONCPPPROCESSORS_V830PROCESSOR_

