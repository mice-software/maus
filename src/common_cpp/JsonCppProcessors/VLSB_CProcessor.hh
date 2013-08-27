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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_VLSB_CPROCESSOR_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_VLSB_CPROCESSOR_

#include "src/common_cpp/DataStructure/VLSB_C.hh"
#include "src/common_cpp/JsonCppProcessors/ObjectProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/PrimitivesProcessors.hh"

namespace MAUS {

/** @class VLSB_CProcessor Conversions for VLSB_C between C++ and Json 
 *
 *  @var _detector_proc
 *  @var _equip_type_proc
 *  @var _time_stamp_proc
 *  @var _phys_event_number_proc
 *  @var _bank_id_proc
 *  @var _adc_proc
 *  @var _part_event_number_proc
 *  @var _channel_proc
 *  @var _tdc_proc
 *  @var _discriminator_proc
 *  @var _ldc_id_proc
 *  @var _geo_proc
 */

class VLSB_CProcessor : public ObjectProcessor<VLSB_C> {
  public:
    /** Set up processors and register branches
     *
     *  Everything else is handled by the base class
     */
    VLSB_CProcessor();

  private:
    StringProcessor _string_proc;
    IntProcessor _int_proc;
};
}  // namespace MAUS

#endif  // #define _SRC_COMMON_CPP_JSONCPPPROCESSORS_VLSB_CPROCESSOR_

