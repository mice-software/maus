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
#include "src/common_cpp/Recon/SciFiEvent.hh"


SciFiEvent::SciFiEvent() {
  // scifidigits.clear();
}

SciFiEvent::~SciFiEvent() {
/*
  std::vector<SciFiHit*>::iterator hit;
  for (hit=scifihits.begin(); hit!=scifihits.end(); ++hit) {
    delete (*hit);
  }
  //scifihits.clear();

  std::vector<SciFiDigit*>::iterator digit;
  for (digit=scifidigits.begin(); digit!=scifidigits.end(); ++digit) {
    delete (*digit);
  }
  //scifidigits.clear();

  std::vector<SciFiSpacePoint*>::iterator spacepoint;
  for (spacepoint=scifispacepoints.begin(); spacepoint!=scifispacepoints.end(); ++spacepoint) {
    delete (*spacepoint);
  }
  //scifispacepoints.clear();
*/
}
