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

  MapCppGlobalReconImport::~MapCppGlobalReconImport() {
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

    char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
    if (!pMAUS_ROOT_DIR) {
      throw MAUS::Exception(Exception::recoverable,
	          std::string("Could not find the $MAUS_ROOT_DIR env variable. ")+\
            std::string("Did you try running: source env.sh?"),
            "MapCppGlobalReconImport::_birth");
    }

    _configCheck = true;
    _classname = "MapCppGlobalReconImport";
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

    MAUS::ReconEventPArray::iterator recon_event_iter;
    for (recon_event_iter = recon_events->begin();
	 recon_event_iter != recon_events->end();
	 ++recon_event_iter) {
      // Load the ReconEvent, and import it into the GlobalEvent
      MAUS::ReconEvent* recon_event = (*recon_event_iter);
      if (recon_event->GetGlobalEvent()) {
	MAUS::GlobalEvent* global_event = recon_event->GetGlobalEvent();
	if (recon_event->GetTOFEvent()) {
	  MAUS::TOFEvent* tof_event = recon_event->GetTOFEvent();
	  MAUS::recon::global::ImportTOFRecon tofrecon_importer;
	  tofrecon_importer.process((*tof_event), global_event, _classname);
	}
	if (recon_event->GetSciFiEvent()) {
	  MAUS::SciFiEvent* scifi_event = recon_event->GetSciFiEvent();
	  MAUS::recon::global::ImportSciFiRecon scifirecon_importer;
	  scifirecon_importer.process((*scifi_event), global_event, _classname);
	}
	if (recon_event->GetCkovEvent()) {
	  MAUS::CkovEvent* ckov_event = recon_event->GetCkovEvent();
	  MAUS::recon::global::ImportCkovRecon ckovrecon_importer;
	  ckovrecon_importer.process((*ckov_event), global_event, _classname);
	}
	if (recon_event->GetKLEvent()) {
	  MAUS::KLEvent* kl_event = recon_event->GetKLEvent();
	  MAUS::recon::global::ImportKLRecon klrecon_importer;
	  klrecon_importer.process((*kl_event), global_event, _classname);
	}
	if (recon_event->GetEMREvent()) {
	  MAUS::EMREvent* emr_event = recon_event->GetEMREvent();
	  MAUS::recon::global::ImportEMRRecon emrrecon_importer;
	  emrrecon_importer.process((*emr_event), global_event, _classname);
	}
      }
    }
  }
} // ~MAUS
