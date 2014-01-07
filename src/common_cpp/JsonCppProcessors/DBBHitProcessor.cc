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

#include "src/common_cpp/JsonCppProcessors/DBBHitProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/DBBSpillDataProcessor.hh"

namespace MAUS {
DBBHitProcessor::DBBHitProcessor() : _int_proc(), _uint_proc() {
    RegisterValueBranch
          ("channel", &_int_proc, &DBBHit::GetChannel,
          &DBBHit::SetChannel, true);
    RegisterValueBranch
          ("geo", &_int_proc, &DBBHit::GetGEO,
          &DBBHit::SetGEO, true);
    RegisterValueBranch
          ("ldc", &_int_proc, &DBBHit::GetLDC,
          &DBBHit::SetLDC, true);
    RegisterValueBranch
          ("leading_edge_time", &_uint_proc, &DBBHit::GetLTime,
          &DBBHit::SetLTime, true);
    RegisterValueBranch
          ("trailing_edge_time", &_uint_proc,
          &DBBHit::GetTTime,
          &DBBHit::SetTTime, true);
}

}  // namespace MAUS

