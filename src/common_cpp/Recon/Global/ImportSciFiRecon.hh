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

/** @class MAUS::recon::global::ImportSciFiRecon
 *  @ingroup globalrecon
 *  @brief Imports the reconstructed output of the SciFi Tracker
 *
 *  Imports reconstructed tracks from the SciFi Tracker detector into
 *  the MAUS::recon::global format.  This will be the standard
 *  starting point of the global reconstruction.
 */

#ifndef _SRC_COMMON_CPP_RECON_IMPORTSCIFI_HH_
#define _SRC_COMMON_CPP_RECON_IMPORTSCIFI_HH_

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

class ImportSciFiRecon {
 public:

  /// Default constructor
  ImportSciFiRecon() {}

  /// Destructor
  ~ImportSciFiRecon() {}

  /**  Main process, accepting the MAUS::SciFiEvent and importing
       track details into an existing MAUS::GlobalEvent
   *  @param[in]  scifi_event  The reconstructed Tracker Event
   *  @param[out] global_event The Global Event, which will be changed
   */
  void process(const MAUS::SciFiEvent &scifi_event,
               MAUS::GlobalEvent* global_event,
               std::string mapper_name);

 private:
  /// Disallow copy constructor as unnecessary
  ImportSciFiRecon(const ImportSciFiRecon&);

  /// Disallow operator= as unnecessary
  void operator=(const ImportSciFiRecon);

  /// Import Straight tracks, populating a
  /// MAUS::recon::global::PrimaryChain object with
  /// MAUS::recon::global::Track's, MAUS::recon::global::TrackPoint's
  /// and MAUS::recon::global::SpacePoint's,
  void ImportStraightTracks(
      const MAUS::SciFiStraightPRTrackArray straightarray,
      MAUS::GlobalEvent* global_event,
      std::string mapper_name);

  /// Import Straight tracks, populating a
  /// MAUS::recon::global::PrimaryChain object with
  /// MAUS::recon::global::Track's, MAUS::recon::global::TrackPoint's
  /// and MAUS::recon::global::SpacePoint's,
  void ImportHelicalTracks(
      const MAUS::SciFiHelicalPRTrackArray helicalarray,
      MAUS::GlobalEvent* global_event,
      std::string mapper_name);
}; // ~class ImportSciFiRecon
} // ~namespace global
} // ~namespace recon
} // ~namespace MAUS

#endif
