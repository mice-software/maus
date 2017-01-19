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

/** @class ImportKLRecon
 *  @author Celeste Pidcott, University of Warwick 
 *  Imports the detector information from the KL detector into
 *  the MAUS::recon::global format.  
 */

#ifndef _SRC_COMMON_CPP_RECON_GLOBAL_IMPORTKLRECON_HH_
#define _SRC_COMMON_CPP_RECON_GLOBAL_IMPORTKLRECON_HH_

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

  class ImportKLRecon {
  public:

    /// Default constructor
    ImportKLRecon() {}

    /// Destructor
    ~ImportKLRecon() {}

    /** @brief Main process, accepting the MAUS::KLEvent and importing
     *   space points into an existing MAUS::GlobalEvent
     *  @param kl_event  The reconstructed KL Event
     *  @param global_event The Global Event, which will be changed
     */
    void process(const MAUS::KLEvent &kl_event, MAUS::GlobalEvent* global_event,
                 std::string mapper_name, bool merge_cell_hits);

    /** @brief Determine whether groupings of adjacent cells exist in the KLCellHits.
     *    Returns a vector of vectors of cell numbers appropriately grouped, e.g. [[4],[7,8]]
     *    If merge_cell_hits is false, all are separated, e.g. [[4],[7],[8]]
     */
    std::vector<std::vector<size_t> > AdjacentCellGroupings(std::vector<KLCellHit> kl_cell_hits,
        bool merge_cell_hits);

    /**
     * @brief Returns whether the first KLCellHit has a lower cell number than the
     * second one. Used to perform an std::sort on a vector of KLCellHits.
     */
    static bool KLCellHitSort(KLCellHit hit1, KLCellHit hit2);

  private:
    /// Disallow copy constructor as unnecessary
    ImportKLRecon(const ImportKLRecon&);

    /// Disallow operator= as unnecessary
    void operator=(const ImportKLRecon);
  }; // ~class ImportKLRecon
} // ~namespace global
} // ~namespace recon
} // ~namespace MAUS

#endif
