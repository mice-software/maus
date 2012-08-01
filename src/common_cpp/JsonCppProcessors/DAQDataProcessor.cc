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

#include "src/common_cpp/JsonCppProcessors/DAQDataProcessor.hh"

namespace MAUS {

DAQDataProcessor::DAQDataProcessor()
    : _V830_proc(), _trigger_request_proc(new TriggerRequestProcessor),
      _tof1_proc(new TOFDaqProcessor), _ckov_proc(new CkovDaqProcessor),
      _tof2_proc(new TOFDaqProcessor), _unknown_proc(new UnknownProcessor),
      _kl_proc(new KLDaqProcessor), _tag_proc(new TagProcessor),
      _tof0_proc(new TOFDaqProcessor), _trigger_proc(new TriggerProcessor) {
    RegisterValueBranch
          ("V830", &_V830_proc, &DAQData::GetV830,
          &DAQData::SetV830, false);
    RegisterValueBranch
          ("trigger_request", &_trigger_request_proc, &DAQData::GetTriggerRequestArray,
          &DAQData::SetTriggerRequestArray, false);
    RegisterValueBranch
          ("tof1", &_tof1_proc, &DAQData::GetTOF1DaqArray,
          &DAQData::SetTOF1DaqArray, false);
    RegisterValueBranch
          ("ckov", &_ckov_proc, &DAQData::GetCkovArray,
          &DAQData::SetCkovArray, false);
    RegisterValueBranch
          ("tof2", &_tof2_proc, &DAQData::GetTOF2DaqArray,
          &DAQData::SetTOF2DaqArray, false);
    RegisterValueBranch
          ("unknown", &_unknown_proc, &DAQData::GetUnknownArray,
          &DAQData::SetUnknownArray, false);
    RegisterValueBranch
          ("kl", &_kl_proc, &DAQData::GetKLArray,
          &DAQData::SetKLArray, false);
    RegisterValueBranch
          ("tag", &_tag_proc, &DAQData::GetTagArray,
          &DAQData::SetTagArray, false);
    RegisterValueBranch
          ("tof0", &_tof0_proc, &DAQData::GetTOF0DaqArray,
          &DAQData::SetTOF0DaqArray, false);
    RegisterValueBranch
          ("trigger", &_trigger_proc, &DAQData::GetTriggerArray,
          &DAQData::SetTriggerArray, false);
}
}  // namespace MAUS


