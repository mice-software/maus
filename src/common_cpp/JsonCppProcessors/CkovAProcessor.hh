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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_CKOVAPROCESSOR_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_CKOVAPROCESSOR_

#include "src/common_cpp/DataStructure/CkovA.hh"
#include "src/common_cpp/JsonCppProcessors/ObjectProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/PrimitivesProcessors.hh"

namespace MAUS {

/** @class CkovAProcessor Conversions for CkovA between C++ and Json 
 *
 *  @var _position_min_0_proc_proc Processor for _position_min_0
 *  @var _arrival_time_2_proc_proc Processor for _arrival_time_2
 *  @var _arrival_time_3_proc_proc Processor for _arrival_time_3
 *  @var _arrival_time_0_proc_proc Processor for _arrival_time_0
 *  @var _arrival_time_1_proc_proc Processor for _arrival_time_1
 *  @var _pulse_1_proc_proc Processor for _pulse_1
 *  @var _pulse_0_proc_proc Processor for _pulse_0
 *  @var _pulse_3_proc_proc Processor for _pulse_3
 *  @var _pulse_2_proc_proc Processor for _pulse_2
 *  @var _coincidences_proc_proc Processor for _coincidences
 *  @var _position_min_2_proc_proc Processor for _position_min_2
 *  @var _position_min_3_proc_proc Processor for _position_min_3
 *  @var _total_charge_proc_proc Processor for _total_charge
 *  @var _position_min_1_proc_proc Processor for _position_min_1
 *  @var _part_event_number_proc_proc Processor for _part_event_number
 *  @var _number_of_pes_proc_proc Processor for _number_of_pes
 */

class CkovAProcessor : public ObjectProcessor<CkovA> {
  public:
    /** Set up processors and register branches
     *
     *  Everything else is handled by the base class
     */
    CkovAProcessor();

  private:
    IntProcessor _position_min_0_proc;
    IntProcessor _arrival_time_2_proc;
    IntProcessor _arrival_time_3_proc;
    IntProcessor _arrival_time_0_proc;
    IntProcessor _arrival_time_1_proc;
    IntProcessor _pulse_1_proc;
    IntProcessor _pulse_0_proc;
    IntProcessor _pulse_3_proc;
    IntProcessor _pulse_2_proc;
    IntProcessor _coincidences_proc;
    IntProcessor _position_min_2_proc;
    IntProcessor _position_min_3_proc;
    IntProcessor _total_charge_proc;
    IntProcessor _position_min_1_proc;
    IntProcessor _part_event_number_proc;
    DoubleProcessor _number_of_pes_proc;
};
}  // namespace MAUS

#endif  // #define _SRC_COMMON_CPP_JSONCPPPROCESSORS_CKOVAPROCESSOR_

