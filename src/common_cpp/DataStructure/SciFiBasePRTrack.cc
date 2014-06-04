/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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

// MAUS headers
#include "src/common_cpp/DataStructure/SciFiBasePRTrack.hh"

namespace MAUS {

SciFiBasePRTrack::SciFiBasePRTrack() {
  _spoints = new TRefArray();
}

SciFiBasePRTrack::~SciFiBasePRTrack() {
  delete _spoints;
}

SciFiSpacePointPArray SciFiBasePRTrack::get_spacepoints_pointers() {
  SciFiSpacePointPArray sp_pointers;

  // Check the _spoints container is initialised
  if (!_spoints) return sp_pointers;

  for (int i = 0; i < (_spoints->GetLast()+1); ++i) {
    sp_pointers.push_back(static_cast<SciFiSpacePoint*>(_spoints->At(i)));
  }
  return sp_pointers;
}

void SciFiBasePRTrack::set_spacepoints_pointers(SciFiSpacePointPArray spoints) {
  if (_spoints) delete _spoints;
  _spoints = new TRefArray();
  for (std::vector<SciFiSpacePoint*>::iterator sp = spoints.begin(); sp != spoints.end(); ++sp) {
    _spoints->Add(*sp);
  }
}

} // ~namespace MAUS
