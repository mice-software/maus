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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_DAQDATAPROCESSOR_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_DAQDATAPROCESSOR_

#include "src/common_cpp/DataStructure/DAQData.hh"
#include "src/common_cpp/JsonCppProcessors/PrimitivesProcessors.hh"
#include "src/common_cpp/JsonCppProcessors/ArrayProcessors.hh"
#include "src/common_cpp/JsonCppProcessors/CkovDaqProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/KLDaqProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/ObjectProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/TOFDaqProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/EMRDaqProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/TagProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/TriggerProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/TriggerRequestProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/UnknownProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/V830Processor.hh"
#include "src/common_cpp/JsonCppProcessors/TrackerDaqProcessor.hh"

namespace MAUS {

/** @class DAQDataProcessor Conversions for DAQData between C++ and Json 
 *
 *  @var _V830_proc_proc Processor for _V830
 *  @var _trigger_request_proc_proc Processor for _trigger_request
 *  @var _tof1_proc_proc Processor for _tof1
 *  @var _ckov_proc_proc Processor for _ckov
 *  @var _tof2_proc_proc Processor for _tof2
 *  @var _unknown_proc_proc Processor for _unknown
 *  @var _kl_proc_proc Processor for _kl
 *  @var _tag_proc_proc Processor for _tag
 *  @var _tof0_proc_proc Processor for _tof0
 *  @var _trigger_proc_proc Processor for _trigger
 *  @var _tracker0_proc_proc Processor for _tracker0
 *  @var _tracker1_proc_proc Processor for _tracker1
 */

class DAQDataProcessor : public ObjectProcessor<DAQData> {
  public:
    /** Set up processors and register branches
     *
     *  Everything else is handled by the base class
     */
    DAQDataProcessor();

  private:
    UIntProcessor _uint_proc;
    V830Processor _V830_proc;
    PointerArrayProcessor<TriggerRequest> _trigger_request_proc;
    PointerArrayProcessor<TOFDaq> _tof1_proc;
    PointerArrayProcessor<CkovDaq> _ckov_proc;
    PointerArrayProcessor<TOFDaq> _tof2_proc;
    PointerArrayProcessor<Unknown> _unknown_proc;
    PointerArrayProcessor<KLDaq> _kl_proc;
    PointerArrayProcessor<Tag> _tag_proc;
    PointerArrayProcessor<TOFDaq> _tof0_proc;
    EMRDaqProcessor _emr_proc;
    PointerArrayProcessor<Trigger> _trigger_proc;
    PointerArrayProcessor<TrackerDaq> _tracker0_proc;
    PointerArrayProcessor<TrackerDaq> _tracker1_proc;
};
}  // namespace MAUS

#endif  // #define _SRC_COMMON_CPP_JSONCPPPROCESSORS_DAQDATAPROCESSOR_

