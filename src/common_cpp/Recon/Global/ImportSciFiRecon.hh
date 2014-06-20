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

/** @class ImportSciFiRecon
 *  @author Celeste Pidcott, University of Warwick
 *  Imports the track points from the SciFi detectors into
 *  the MAUS::recon::global format.  
 */

#ifndef _SRC_COMMON_CPP_RECON_GLOBAL_IMPORTSCIFIRECON_HH_
#define _SRC_COMMON_CPP_RECON_GLOBAL_IMPORTSCIFIRECON_HH_

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
     *   track points and tracks into an existing MAUS::GlobalEvent
     *  @param scifi_event  The reconstructed SciFi Event
     *  @param global_event The Global Event, which will be changed
     */
    void process(const MAUS::SciFiEvent &scifi_event,
		 MAUS::GlobalEvent* global_event,
		 std::string mapper_name);

  private:
    /// Disallow copy constructor as unnecessary
    ImportSciFiRecon(const ImportSciFiRecon&);

    /// Disallow operator= as unnecessary
    void operator=(const ImportSciFiRecon);

    /**  @brief Add Scifi tracks to global event
     *   
     *  @param scifi_track  The reconstructed SciFi Track
     *  @param global_track The Global Track, which will be changed
     *  @param mapper_name 
     */
    void ImportSciFiTrack(const MAUS::SciFiTrack* scifi_track,
			  MAUS::DataStructure::Global::Track* global_track,
			  std::string mapper_name);
    /**  @brief Obtain detector enum for a given tracker and station
     *   
     *  @param tracker  Tracker number
     *  @param station  Station number
     *  @param detector  Detector enum, which will be changed
     */
    void SetStationEnum(
      MAUS::DataStructure::Global::TrackPoint* GlobalSciFiTrackPoint,
      MAUS::DataStructure::Global::Track* GlobalSciFiTrack, int tracker,
      int station);
  }; // ~class ImportSciFiRecon
} // ~namespace global
} // ~namespace recon
} // ~namespace MAUS

#endif
