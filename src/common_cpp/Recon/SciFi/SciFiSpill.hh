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

// namespace MAUS {

class SciFiSpill {
 public:
  SciFiSpill();

  ~SciFiSpill();

  void add_event(SciFiEvent* event) { _events_in_spill.push_back(event); }

  std::vector<SciFiEvent*> events() { return _events_in_spill; }

 private:
  /// This is a vector containing the events of a spill.
  std::vector<SciFiEvent*>   _events_in_spill;
};  // Don't forget this trailing colon!!!!

// } // ~namespace MAUS

#endif
