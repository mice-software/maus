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

#include "src/map/MapCppGlobalRecon/MapCppGlobalRecon.hh"

#include "src/common_cpp/API/APIExceptions.hh"

#include "Recon/Global/ImportSciFiRecon.hh"

namespace MAUS {
MapCppGlobalRecon::MapCppGlobalRecon()
    : MapBase<MAUS::Data, MAUS::Data>("MapCppGlobalRecon") {}

void MapCppGlobalRecon::_birth(const std::string& argJsonConfigDocument) {
  // Check if the JSON document can be parsed, else return error only.
  bool parsingSuccessful = _reader.parse(argJsonConfigDocument, _configJSON);
  if (!parsingSuccessful) {
    throw(Squeal(Squeal::nonRecoverable,
                 "Unable to Parse Json Config",
                 "MapCppGlobalRecon::_birth"));
  }
}

void MapCppGlobalRecon::_death() {}

MAUS::Data* MapCppGlobalRecon::_process(MAUS::Data* data) const {
  if (!data) {
    throw(Squeal(Squeal::nonRecoverable,
                 "NULL MAUS::Data* passed to process.",
                 "MapCppGlobalRecon::_process"));
  }

  const MAUS::Spill* spill = data->GetSpill();
  if (!spill) {
    throw(Squeal(Squeal::recoverable,
                 "No MAUS::Spill in MAUS::Data, required!",
                 "MapCppGlobalRecon::_process"));
  }

  MAUS::ReconEventArray* recon_events = spill->GetReconEvents();

  if (!recon_events) {
    throw(Squeal(Squeal::recoverable,
                 "No MAUS::ReconEventArray in MAUS::Spill!",
                 "MapCppGlobalRecon::_process"));
  }

  MAUS::ReconEventArray::iterator recon_event_iter;
  for (recon_event_iter = recon_events->begin();
      recon_event_iter != recon_events->end();
      ++recon_event_iter) {
    // Load the ReconEvent, and import it into the GlobalEvent
    MAUS::ReconEvent* recon_event = (*recon_event_iter);
    MAUS::GlobalEvent* global_event = Import(recon_event);
    recon_event->SetGlobalEvent(global_event);
  }

  return data;
}

MAUS::GlobalEvent*
MapCppGlobalRecon::Import(MAUS::ReconEvent* recon_event) const {
  if (!recon_event) {
    throw(Squeal(Squeal::recoverable,
                 "Trying to import an empty recon event.",
                 "MapCppGlobalRecon::Import"));
  }

  if (recon_event->GetGlobalEvent()) {
    throw(Squeal(Squeal::recoverable,
                 "Trying to replace GlobalEvent in ReconEvent",
                 "MapCppGlobalRecon::Import"));
  }

  // Create our new GlobalEvent
  MAUS::GlobalEvent* global_event = new MAUS::GlobalEvent();

  MAUS::DataStructure::Global::SpacePoint* space_point =
      new MAUS::DataStructure::Global::SpacePoint();

  space_point->set_charge(1234.0);
  space_point->set_detector(MAUS::DataStructure::Global::kTracker1);
  space_point->set_geometry_path("FakeGeomPath");

  global_event->add_space_point_check(space_point);

  space_point = new MAUS::DataStructure::Global::SpacePoint();

  space_point->set_charge(2345.0);
  space_point->set_detector(MAUS::DataStructure::Global::kTracker2);
  space_point->set_geometry_path("RealGeomPath");

  MAUS::DataStructure::Global::TrackPoint* track_point =
      new MAUS::DataStructure::Global::TrackPoint();

  track_point->set_charge(3456.0);
  track_point->set_space_point(space_point);

  global_event->add_track_point_recursive(track_point);

  // // Import the various reconstruction elements
  // MAUS::SciFiEvent* scifi_event = recon_event->GetSciFiEvent();
  // if (scifi_event) {
  //   MAUS::recon::global::ImportSciFiRecon scifirecon_importer;
  //   scifirecon_importer.process((*scifi_event), global_event, _classname);
  // }

  // Return the new GlobalEvent, to be added to the ReconEvent
  return global_event;
}
} // ~MAUS
