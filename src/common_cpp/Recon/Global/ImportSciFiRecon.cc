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

#include "Recon/Global/ImportSciFiRecon.hh"

#include <algorithm>

#include "Interface/Squeak.hh"

namespace MAUS {
namespace recon {
namespace global {

void ImportSciFiRecon::process(const MAUS::SciFiEvent &scifi_event,
                               MAUS::GlobalEvent* global_event,
                               std::string mapper_name) {
  Squeak::mout(Squeak::debug) << "Import SciFi" << std::endl;

  // Import the straight tracks
  const MAUS::SciFiStraightPRTrackPArray straightarray
      = scifi_event.straightprtracks();

  if (!straightarray.empty()) {
    ImportStraightTracks(straightarray, global_event, mapper_name);
  }

  // Import the helical tracks
  const MAUS::SciFiHelicalPRTrackPArray helicalarray
      = scifi_event.helicalprtracks();

  if (!helicalarray.empty()) {
    ImportHelicalTracks(helicalarray, global_event, mapper_name);
  }
}

void ImportSciFiRecon::ImportStraightTracks(
    const MAUS::SciFiStraightPRTrackPArray straightarray,
    MAUS::GlobalEvent* global_event,
    std::string mapper_name) {
  if (straightarray.empty()) {
    Squeak::mout(Squeak::debug) << "\tStraight Array size:\tEmpty"
                                << std::endl;
    return;
  }

  Squeak::mout(Squeak::debug)
      << "\tStraight Array size:\t"
      << straightarray.size() << std::endl;

  // Create a new MAUS::recon::global::PrimaryChain for the output.
  std::string local_mapper_name = mapper_name + "/ImportStraight";
  MAUS::DataStructure::Global::PrimaryChain* pchain =
      new MAUS::DataStructure::Global::PrimaryChain(local_mapper_name);



  // Add the MAUS::recon::global::PrimaryChain result to the
  // MAUS::GlobalEvent
  global_event->add_primary_chain_recursive(pchain);
}

void ImportSciFiRecon::ImportHelicalTracks(
    const MAUS::SciFiHelicalPRTrackPArray helicalarray,
    MAUS::GlobalEvent* global_event,
    std::string mapper_name) {
  if (helicalarray.empty()) {
    Squeak::mout(Squeak::debug) << "\tHelical Array size:\tEmpty"
                                << std::endl;
    return;
  }

  Squeak::mout(Squeak::debug)
      << "\tHelical Array size:\t"
      << helicalarray.size() << std::endl;

  // Create a new MAUS::recon::global::PrimaryChain for the output.
  std::string local_mapper_name = mapper_name + "/ImportHelical";
  MAUS::DataStructure::Global::PrimaryChain* pchain =
      new MAUS::DataStructure::Global::PrimaryChain(local_mapper_name);

  // Loop over the input array, accessing each track
  MAUS::SciFiHelicalPRTrackPArray::const_iterator track_iter;
  for (track_iter = helicalarray.begin();
      track_iter != helicalarray.end();
      ++track_iter) {
    const SciFiHelicalPRTrack* helical_track = *track_iter;

    Squeak::mout(Squeak::debug)
        << "\t\tHelical Track:\t"
        << std::endl;
  }

  // Add the MAUS::recon::global::PrimaryChain result to the
  // MAUS::GlobalEvent
  global_event->add_primary_chain_recursive(pchain);
}




} // ~namespace global
} // ~namespace recon
} // ~namespace MAUS
