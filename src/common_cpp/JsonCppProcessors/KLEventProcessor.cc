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

#include "src/common_cpp/JsonCppProcessors/KLEventProcessor.hh"

namespace MAUS {

KLEventProcessor::KLEventProcessor()
    : _kl_digits_proc(), _kl_cell_hits_proc() {

    RegisterValueBranch
          ("kl_digits", &_kl_digits_proc, &KLEvent::GetKLEventDigit,
          &KLEvent::SetKLEventDigit, false);

    RegisterValueBranch
          ("kl_cell_hits", &_kl_cell_hits_proc, &KLEvent::GetKLEventCellHit,
          &KLEvent::SetKLEventCellHit, false);
}
}  // namespace MAUS


