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

/** @class TrackMatching
 *  @author Celeste Pidcott, University of Warwick
 *  Takes a global event containing space points and creates global tracks.
 */

#ifndef _SRC_COMMON_CPP_RECON_TRACKMATCHING_HH_
#define _SRC_COMMON_CPP_RECON_TRACKMATCHING_HH_

// C++ headers
#include <string>
#include <vector>

// ROOT headers
#include "Rtypes.h"

// MAUS headers
#include "DataStructure/ReconEvent.hh"
#include "Recon/Global/ImportTOFRecon.hh"
#include "Recon/Global/ImportSciFiRecon.hh"
#include "Recon/Global/ImportKLRecon.hh"

namespace MAUS {
namespace recon {
namespace global {

  class TrackMatching {
  public:

    /// Default constructor
    TrackMatching() {}

    /// Destructor
    ~TrackMatching() {}

    /**  @brief Main process, constructing global tracks from space points
     *   @param global_event The Global Event, which will be changed
     */
    void FormTracks(MAUS::GlobalEvent* global_event, std::string mapper_name);

  private:
    /// Disallow copy constructor as unnecessary
    TrackMatching(const TrackMatching&);

    /// Disallow operator= as unnecessary
    void operator=(const TrackMatching);

    void MakeTOFTracks(MAUS::GlobalEvent* global_event,
		       std::vector<MAUS::DataStructure::Global::SpacePoint*>
		       *GlobalSpacePointArray,
		       MAUS::DataStructure::Global::TrackPArray& TOFTrackArray,
		       std::string mapper_name);

    void MakeKLTracks(MAUS::GlobalEvent* global_event,
		      std::vector<MAUS::DataStructure::Global::SpacePoint*>
		      *GlobalSpacePointArray,
		      MAUS::DataStructure::Global::Track* KLTrack,
		      std::string mapper_name);
  }; // ~class TrackMatching
} // ~namespace global
} // ~namespace recon
} // ~namespace MAUS

#endif

