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

/** @class SciFiSpill
 *  D...
 *
 */

#ifndef SCIFISPILL_HH
#define SCIFISPILL_HH
// C headers

// C++ headers
#include <vector>
// #include <string>

// others
#include "src/common_cpp/Recon/SciFi/SciFiEvent.hh"


class SciFiSpill {
 public:
  SciFiSpill();

  ~SciFiSpill();

  // std::vector events() const { return events_in_spill; }

  // private:
  std::vector<SciFiEvent>   events_in_spill;
};  // Don't forget this trailing colon!!!!

#endif
