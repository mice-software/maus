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

#include "src/map/MapCppGlobalReconImport/MapCppGlobalReconImport.hh"

#include "src/common_cpp/DataStructure/Data.hh"
#include "Interface/Squeak.hh"
#include "src/common_cpp/Converter/DataConverters/JsonCppSpillConverter.hh"
#include "src/common_cpp/Converter/DataConverters/CppJsonSpillConverter.hh"
#include "src/common_cpp/API/PyWrapMapBase.hh"

namespace MAUS {

  PyMODINIT_FUNC init_MapCppGlobalReconImport(void) {
    PyWrapMapBase<MAUS::MapCppGlobalReconImport>::PyWrapMapBaseModInit
                                    ("MapCppGlobalReconImport", "", "", "", "");
  }

  MapCppGlobalReconImport::MapCppGlobalReconImport()
    : MapBase<Data>("MapCppGlobalReconImport"), _configCheck(false) {
  }

  void MapCppGlobalReconImport::_birth(const std::string& argJsonConfigDocument) {
    // Check if the JSON document can be parsed, else return error only.
    _configCheck = false;
    bool parsingSuccessful = _reader.parse(argJsonConfigDocument, _configJSON);
    if (!parsingSuccessful) {
        throw MAUS::Exception(Exception::recoverable,
                              "Failed to parse configuration",
                              "MapCppGlobalReconImport::birth");
    }
    _configCheck = true;
  }

  void MapCppGlobalReconImport::_death() {
  }

  void MapCppGlobalReconImport::_process(MAUS::Data* data_cpp) const {
    // Read string and convert to a Json object
    if (!data_cpp) {
        throw MAUS::Exception(Exception::recoverable,
                              "data_cpp was NULL",
                              "MapCppGlobalReconImport::_process");
    }
    if (!_configCheck) {
        throw MAUS::Exception(Exception::recoverable,
                              "Birth has not been successfully called",
                              "MapCppGlobalReconImport::_process");
    }

    const MAUS::Spill* spill = data_cpp->GetSpill();

    MAUS::ReconEventPArray* recon_events = spill->GetReconEvents();

    if (!recon_events) {
      return;
    }

    MAUS::GlobalEvent* global_event = NULL;
    MAUS::ReconEventPArray::iterator recon_event_iter;
    for (recon_event_iter = recon_events->begin();
	 recon_event_iter != recon_events->end();
	 ++recon_event_iter) {
      // Load the ReconEvent, and import it into the GlobalEvent
      MAUS::ReconEvent* recon_event = (*recon_event_iter);
      global_event = recon_event->GetGlobalEvent();
      std::cerr << "Pre import" << std::endl;
      global_event = Import(recon_event);
    }
  }

  MAUS::GlobalEvent*
  MapCppGlobalReconImport::Import(MAUS::ReconEvent* recon_event) const {
    if (!recon_event) {
      throw(Exception(Exception::recoverable,
		      "Trying to import an empty recon event.",
		      "MapCppGlobalReconImport::Import"));
    }

    MAUS::GlobalEvent* global_event = recon_event->GetGlobalEvent();
    MAUS::TOFEvent* tof_event = recon_event->GetTOFEvent();
    MAUS::SciFiEvent* scifi_event = recon_event->GetSciFiEvent();
    MAUS::CkovEvent* ckov_event = recon_event->GetCkovEvent();
    MAUS::KLEvent* kl_event = recon_event->GetKLEvent();

    if (tof_event) {
      std::cerr << "Happily TOFing" << std::endl;
      MAUS::recon::global::ImportTOFRecon tofrecon_importer;
      tofrecon_importer.process((*tof_event), global_event, _classname);
      std::cerr << "Finished TOFing" << std::endl;
    }
    if (scifi_event) {
      std::cerr << "SCIFI time" << std::endl;
      MAUS::recon::global::ImportSciFiRecon scifirecon_importer;
      scifirecon_importer.process((*scifi_event), global_event, _classname);
      std::cerr << "I doubt I'll see this message" << std::endl;
    }
    if (ckov_event) {
      MAUS::recon::global::ImportCkovRecon ckovrecon_importer;
      ckovrecon_importer.process((*ckov_event), global_event, _classname);
    }
    if (kl_event) {
      MAUS::recon::global::ImportKLRecon klrecon_importer;
      klrecon_importer.process((*kl_event), global_event, _classname);
    }
    // Return the new GlobalEvent, to be added to the ReconEvent
    return global_event;
  }
} // ~MAUS
