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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_KLDIGITPROCESSOR_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_KLDIGITPROCESSOR_

#include "src/common_cpp/DataStructure/KLDigit.hh"
#include "src/common_cpp/JsonCppProcessors/ObjectProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/PrimitivesProcessors.hh"

namespace MAUS {

/** @class KLDigitProcessor Conversions for KLDigit between C++ and Json 
 *
 *  @var _pmt_proc_proc Processor for _pmt
 *  @var _charge_mm_proc_proc Processor for _charge_mm
 *  @var _charge_pm_proc_proc Processor for _charge_pm
 *  @var _phys_event_number_proc_proc Processor for _phys_event_number
 *  @var _kl_key_proc_proc Processor for _kl_key
 *  @var _part_event_number_proc_proc Processor for _part_event_number
 *  @var _cell_proc_proc Processor for _cell
 *  @var _position_max_proc_proc Processor for _position_max
 */

class KLDigitProcessor : public ObjectProcessor<KLDigit> {
  public:
    /** Set up processors and register branches
     *
     *  Everything else is handled by the base class
     */
    KLDigitProcessor();


  private:
    IntProcessor _pmt_proc;
    IntProcessor _charge_mm_proc;
    IntProcessor _charge_pm_proc;
    IntProcessor _phys_event_number_proc;
    StringProcessor _kl_key_proc;
    IntProcessor _part_event_number_proc;
    IntProcessor _cell_proc;
    IntProcessor _position_max_proc;
};
}  // namespace MAUS

#endif  // #define _SRC_COMMON_CPP_JSONCPPPROCESSORS_KLDIGITPROCESSOR_

