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

#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/ScalarsProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/EMRSpillDataProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/DAQDataProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/MCEventProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/ReconEventProcessor.hh"

#include <iostream>

namespace MAUS {

SpillProcessor::SpillProcessor() {
    Spill* a_spill;
    DAQDataProcessor ddp;
    JsonCppObjectProcessor<Spill>::add_branch<DAQData>(std::string("daq_data"), &ddp, a_spill, &Spill::GetDAQData, &Spill::SetDAQData, true);
/*
    ScalarsProcessor scal;
    EMRSpillDataProcessor emr;
    ReconEventArrayProcessor rec;
    MCEventArrayProcessor mc;

    JsonCppObjectProcessor<Spill>::add_branch<DAQData>(std::string("daq_data"), &scal, a_spill, &Spill::GetDAQData, &Spill::SetDAQData, true);
    JsonCppObjectProcessor<Spill>::add_branch<DAQData>(std::string("daq_data"), &emr, a_spill, &Spill::GetDAQData, &Spill::SetDAQData, true);
    JsonCppObjectProcessor<Spill>::add_branch<DAQData>(std::string("daq_data"), &rec, a_spill, &Spill::GetDAQData, &Spill::SetDAQData, true);
    JsonCppObjectProcessor<Spill>::add_branch<DAQData>(std::string("daq_data"), &mc, a_spill, &Spill::GetDAQData, &Spill::SetDAQData, true);
*/
}

}

