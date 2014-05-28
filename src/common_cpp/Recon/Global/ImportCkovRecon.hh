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

/** @class ImportCkovRecon
 *  Imports the space points from the Ckov detectors into
 *  the MAUS::recon::global format.  
 */

#ifndef _SRC_COMMON_CPP_RECON_GLOBAL_IMPORTCKOVRECON_HH_
#define _SRC_COMMON_CPP_RECON_GLOBAL_IMPORTCKOVRECON_HH_

// C++ headers
#include <string>
#include <vector>

// ROOT headers
#include "Rtypes.h"

// MAUS headers
#include "DataStructure/ReconEvent.hh"

namespace MAUS {
namespace recon {
namespace global {

  class ImportCkovRecon {
  public:

    /// Default constructor
    ImportCkovRecon() {}

    /// Destructor
    ~ImportCkovRecon() {}

    /**  Main process, accepting the MAUS::CkovEvent and importing
     *   space points into an existing MAUS::GlobalEvent
     *  @param ckov_event  The reconstructed Ckov Event
     *  @param global_event The Global Event, which will be changed
     */
    void process(const MAUS::CkovEvent &ckov_event,
		 MAUS::GlobalEvent* global_event,
		 std::string mapper_name);

  private:
    /// Disallow copy constructor as unnecessary
    ImportCkovRecon(const ImportCkovRecon&);

    /// Disallow operator= as unnecessary
    void operator=(const ImportCkovRecon);
  };// ~class ImportCkovRecon
} // ~namespace global
} // ~namespace recon
} // ~namespace MAUS

#endif
