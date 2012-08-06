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
 *  Spill object for SciFi data, holding only SciFiEvents
 *
 */

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_SCIFISPILL_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_SCIFISPILL_HH_

// C++ headers
#include <vector>

// ROOT headers
#include "Rtypes.h"

// MAUS headers
#include "src/common_cpp/DataStructure/SciFiEvent.hh"

namespace MAUS {

class SciFiSpill {
 public:
  /** Default constructor */
  SciFiSpill();

  /** Copy constructor - any pointers are deep copied */
  SciFiSpill(const SciFiSpill& _scifispill);

  /** Assignment operator - any pointers are deep copied */
  SciFiSpill& operator=(const SciFiSpill& _scifispill);

  /** Destructor */
  virtual ~SciFiSpill();

  void add_event(SciFiEvent* event) {_scifievents.push_back(event); }

  SciFiEventPArray events() { return _scifievents; }

 private:
  /// This is a vector of pointers containing the events of a spill.
  SciFiEventPArray _scifievents;

  ClassDef(SciFiSpill, 1)
};  // Don't forget this trailing colon!!!!

} // ~namespace MAUS

#endif
