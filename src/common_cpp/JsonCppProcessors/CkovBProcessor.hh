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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_CKOVBPROCESSOR_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_CKOVBPROCESSOR_

#include "src/common_cpp/DataStructure/CkovB.hh"
#include "src/common_cpp/JsonCppProcessors/ObjectProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/PrimitivesProcessors.hh"

namespace MAUS {

/** @class CkovBProcessor Conversions for CkovB between C++ and Json 
 *
 *  @var _arrival_time_6_proc_proc Processor for _arrival_time_6
 *  @var _arrival_time_7_proc_proc Processor for _arrival_time_7
 *  @var _arrival_time_4_proc_proc Processor for _arrival_time_4
 *  @var _arrival_time_5_proc_proc Processor for _arrival_time_5
 *  @var _pulse_5_proc_proc Processor for _pulse_5
 *  @var _pulse_4_proc_proc Processor for _pulse_4
 *  @var _pulse_7_proc_proc Processor for _pulse_7
 *  @var _pulse_6_proc_proc Processor for _pulse_6
 *  @var _position_min_6_proc_proc Processor for _position_min_6
 *  @var _coincidences_proc_proc Processor for _coincidences
 *  @var _total_charge_proc_proc Processor for _total_charge
 *  @var _part_event_number_proc_proc Processor for _part_event_number
 *  @var _position_min_7_proc_proc Processor for _position_min_7
 *  @var _number_of_pes_proc_proc Processor for _number_of_pes
 *  @var _position_min_5_proc_proc Processor for _position_min_5
 *  @var _position_min_4_proc_proc Processor for _position_min_4
 */

class CkovBProcessor : public ObjectProcessor<CkovB> {
  public:
    /** Set up processors and register branches
     *
     *  Everything else is handled by the base class
     */
    CkovBProcessor();

  private:
    IntProcessor _arrival_time_6_proc;
    IntProcessor _arrival_time_7_proc;
    IntProcessor _arrival_time_4_proc;
    IntProcessor _arrival_time_5_proc;
    IntProcessor _pulse_5_proc;
    IntProcessor _pulse_4_proc;
    IntProcessor _pulse_7_proc;
    IntProcessor _pulse_6_proc;
    IntProcessor _position_min_6_proc;
    IntProcessor _coincidences_proc;
    IntProcessor _total_charge_proc;
    IntProcessor _part_event_number_proc;
    IntProcessor _position_min_7_proc;
    DoubleProcessor _number_of_pes_proc;
    IntProcessor _position_min_5_proc;
    IntProcessor _position_min_4_proc;
};
}  // namespace MAUS

#endif  // #define _SRC_COMMON_CPP_JSONCPPPROCESSORS_CKOVBPROCESSOR_

