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

#include "src/common_cpp/JsonCppProcessors/TOFEventSlabHitProcessor.hh"

namespace MAUS {

TOFEventSlabHitProcessor::TOFEventSlabHitProcessor()
    : _tof1_proc(new TOFSlabHitProcessor),
      _tof0_proc(new TOFSlabHitProcessor), _tof2_proc(new TOFSlabHitProcessor) {
    RegisterValueBranch
          ("tof1", &_tof1_proc, &TOFEventSlabHit::GetTOF1SlabHitArray,
          &TOFEventSlabHit::SetTOF1SlabHitArray, true);
    RegisterValueBranch
          ("tof0", &_tof0_proc, &TOFEventSlabHit::GetTOF0SlabHitArray,
          &TOFEventSlabHit::SetTOF0SlabHitArray, true);
    RegisterValueBranch
          ("tof2", &_tof2_proc, &TOFEventSlabHit::GetTOF2SlabHitArray,
          &TOFEventSlabHit::SetTOF2SlabHitArray, true);
}
}  // namespace MAUS


