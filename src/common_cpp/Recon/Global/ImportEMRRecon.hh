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

/** @class ImportEMRRecon
 *  @author Celeste Pidcott, University of Warwick
 *  Imports the hits from the EMR detector into
 *  the MAUS::recon::global format.  
 */

#ifndef _SRC_COMMON_CPP_RECON_GLOBAL_IMPORTEMRRECON_HH_
#define _SRC_COMMON_CPP_RECON_GLOBAL_IMPORTEMRRECON_HH_

// C++ headers
#include <string>
#include <vector>

// ROOT headers
#include "Rtypes.h"

// MAUS headers
#include "DataStructure/ReconEvent.hh"
#include "Config/MiceModule.hh"

namespace MAUS {
namespace recon {
namespace global {

  class ImportEMRRecon {
  public:

    /// Default constructor
    ImportEMRRecon() {}

    /// Destructor
    ~ImportEMRRecon() {}

    /**  Main process, accepting the MAUS::EMREvent and importing
     *   space points into an existing MAUS::GlobalEvent
     *  @param emr_event  The reconstructed EMR Event
     *  @param global_event The Global Event, which will be changed
     */
    void process(const MAUS::EMREvent &emr_event,
		 MAUS::GlobalEvent* global_event,
		 std::string mapper_name,
		 std::string geo_filename);

  private:
    /// Disallow copy constructor as unnecessary
    ImportEMRRecon(const ImportEMRRecon&);

    /// Disallow operator= as unnecessary
    void operator=(const ImportEMRRecon);
  }; // ~class ImportEMRRecon
} // ~namespace global
} // ~namespace recon
} // ~namespace MAUS

#endif
