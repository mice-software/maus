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
#include "src/common_cpp/Recon/SingleStation/SEEvent.hh"

SEEvent::SEEvent() {
  _SEdigits.resize(0);
  _SEclusters.resize(0);
  _SEspacepoints.resize(0);
  _TOF0spacepoints.resize(0);
  _TOF1spacepoints.resize(0);
}

SEEvent::~SEEvent() {
  std::cerr << "SEEVENT DESTRUCTOR CALLED" << std::endl;
  std::vector<SEDigit*>::iterator digit;
  for (digit = _SEdigits.begin(); digit!= _SEdigits.end(); ++digit) {
    delete (*digit);
  }

  std::vector<SESpacePoint*>::iterator spacepoint;
  for (spacepoint = _SEspacepoints.begin(); spacepoint!= _SEspacepoints.end(); ++spacepoint) {
    delete (*spacepoint);
  }
}

SEEvent::SEEvent(const SEEvent &event) {
  _phys_event_no = event.get_phys_event_no();
  _part_event_no = event.get_part_event_no();

  _SEdigits        = event.digits();
  _SEclusters      = event.clusters();
  _SEspacepoints   = event.spacepoints();
  _TOF0spacepoints = event.TOF0spacepoints();
  _TOF1spacepoints = event.TOF1spacepoints();
}

SEEvent& SEEvent::operator=(const SEEvent &event) {
  _phys_event_no = event.get_phys_event_no();
  _part_event_no = event.get_part_event_no();

  _SEdigits        = event.digits();
  _SEclusters      = event.clusters();
  _SEspacepoints   = event.spacepoints();
  _TOF0spacepoints = event.TOF0spacepoints();
  _TOF1spacepoints = event.TOF1spacepoints();
}
