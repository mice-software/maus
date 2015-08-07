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

/** @class ImportTOFRecon
 *  @author Celeste Pidcott, University of Warwick
 *  Imports the space points from the TOF detectors into
 *  the MAUS::recon::global format.  
 */

#ifndef _SRC_COMMON_CPP_RECON_GLOBAL_IMPORTTOFRECON_HH_
#define _SRC_COMMON_CPP_RECON_GLOBAL_IMPORTTOFRECON_HH_

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

  class ImportTOFRecon {
  public:

    /// Default constructor
    ImportTOFRecon() {}

    /// Destructor
    ~ImportTOFRecon() {}

    /**  Main process, accepting the MAUS::TOFEvent and importing
     *   space points into an existing MAUS::GlobalEvent
     *  @param tof_event  The reconstructed TOF Event
     *  @param global_event The Global Event, which will be changed
     */
    void process(const MAUS::TOFEvent &tof_event,
		 MAUS::GlobalEvent* global_event,
		 std::string mapper_name,
		 std::string geo_filename);

  private:
    /// Disallow copy constructor as unnecessary
    ImportTOFRecon(const ImportTOFRecon&);

    /// Disallow operator= as unnecessary
    void operator=(const ImportTOFRecon);

    /**  @brief Add TOF space points to global event
     *   
     *  @param tofEventSpacepoint  The reconstructed TOF Event
     *  @param global_event The Global Event, which will be changed
     *  @param mapper_name 
     */
    void ImportTOFSpacePoints(
			      const MAUS::TOFEventSpacePoint tofEventSpacepoint,
			      MAUS::GlobalEvent* global_event,
			      std::string mapper_name,
			      std::string geo_filename);
  }; // ~class ImportTOFRecon
} // ~namespace global
} // ~namespace recon
} // ~namespace MAUS

#endif
