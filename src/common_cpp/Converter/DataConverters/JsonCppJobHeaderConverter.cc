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
 *
 */

#include "src/common_cpp/Converter/DataConverters/JsonCppJobHeaderConverter.hh"

#include "src/common_cpp/DataStructure/JobHeader.hh"
#include "src/common_cpp/JsonCppProcessors/JobHeaderProcessor.hh"

namespace MAUS {

JobHeaderData* JsonCppJobHeaderConverter::_convert(
    const Json::Value* data) const {
  JobHeader* job_header = JobHeaderProcessor().JsonToCpp(*data);
  JobHeaderData* data_cpp = new JobHeaderData();
  data_cpp->SetEvent(job_header);
  return data_cpp;
}
}

