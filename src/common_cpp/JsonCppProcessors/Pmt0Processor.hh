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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_PMT0PROCESSOR_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_PMT0PROCESSOR_

#include "src/common_cpp/DataStructure/Pmt0.hh"
#include "src/common_cpp/JsonCppProcessors/ObjectProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/PrimitivesProcessors.hh"

namespace MAUS {

/** @class Pmt0Processor Conversions for Pmt0 between C++ and Json 
 *
 *  @var _raw_time_proc_proc Processor for _raw_time
 *  @var _charge_proc_proc Processor for _charge
 *  @var _leading_time_proc_proc Processor for _leading_time
 *  @var _time_proc_proc Processor for _time
 *  @var _tof_key_proc_proc Processor for _tof_key
 *  @var _trigger_request_leading_time_proc_proc Processor for _trigger_request_leading_time
 */

class Pmt0Processor : public ObjectProcessor<Pmt0> {
  public:
    /** Set up processors and register branches
     *
     *  Everything else is handled by the base class
     */
    Pmt0Processor();

  private:
    DoubleProcessor _raw_time_proc;
    IntProcessor _charge_proc;
    IntProcessor _leading_time_proc;
    DoubleProcessor _time_proc;
    StringProcessor _tof_key_proc;
    IntProcessor _trigger_request_leading_time_proc;
};
}  // namespace MAUS

#endif  // #define _SRC_COMMON_CPP_JSONCPPPROCESSORS_PMT0PROCESSOR_

