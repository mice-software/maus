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

#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/Converter/DataConverters/JsonCppSpillConverter.hh"
#include "src/common_cpp/Converter/DataConverters/CppJsonSpillConverter.hh"

#include "Recon/Global/ImportSciFiRecon.hh"

namespace MAUS {
MapCppGlobalRecon::MapCppGlobalRecon() {
  _classname = "MapCppGlobalRecon";
}

bool MapCppGlobalRecon::birth(std::string argJsonConfigDocument) {
  // Check if the JSON document can be parsed, else return error only.
  bool parsingSuccessful = _reader.parse(argJsonConfigDocument, _configJSON);
  if (!parsingSuccessful) {
    return false;
  }

  return true;
}

bool MapCppGlobalRecon::death() {
  return true;
}

std::string MapCppGlobalRecon::process(std::string document) const {
  // Prepare converters, spill and json objects
  JsonCppSpillConverter json2cppconverter;
  CppJsonSpillConverter cpp2jsonconverter;
  Json::Value *data_json;
  MAUS::Spill *spill;

  // Read string and convert to a Json object
  Json::Value imported_json = JsonWrapper::StringToJson(document);
  data_json = &imported_json;

  if(!data_json || data_json->isNull()){
    return std::string("{\"errors\":{\"bad_json_document\":")+
           std::string("\"Failed to parse input document\"}}");
  }

  if(data_json->empty()){
    return std::string("{\"errors\":{\"bad_json_document\":")+
           std::string("\"Failed to parse input document\"}}");
  }

  // Convert Json into MAUS::Spill object.  In future, this will all
  // be done for me, and process will take/return whichever object we
  // prefer.
  spill = json2cppconverter(data_json);

  if(!spill){
    return std::string("{\"errors\":{\"failed_json_cpp_conversion\":")+
           std::string("\"Failed to convert Json to Cpp Spill object\"}}");
  }

  MAUS::ReconEventArray* recon_events = spill->GetReconEvents();

  if(!recon_events) {
    return document;
  }

  std::cout << "Recon Event Size:\t" << recon_events->size() << std::endl;

  MAUS::ReconEventArray::iterator recon_event_iter;
  for(recon_event_iter = recon_events->begin();
      recon_event_iter != recon_events->end();
      ++recon_event_iter) {
    // Load the ReconEvent, and import it into the GlobalEvent
    MAUS::ReconEvent* recon_event = (*recon_event_iter);
    MAUS::GlobalEvent* global_event = Import(recon_event);
    recon_event->SetGlobalEvent(global_event);
  }

  MAUS::Data *data_cpp = new MAUS::Data();
  data_cpp->SetSpill(spill);
  data_json = cpp2jsonconverter(data_cpp);

  if(!data_json){
    return std::string("{\"errors\":{\"failed_cpp_json_conversion\":")+
           std::string("\"Failed to convert Cpp to Json Spill object\"}}");
  }

  return JsonWrapper::JsonToString(*data_json);
}

MAUS::GlobalEvent*
MapCppGlobalRecon::Import(MAUS::ReconEvent* recon_event) const {
  if(!recon_event) {
    throw(Squeal(Squeal::recoverable,
                 "Trying to import an empty recon event.",
                 "MapCppGlobalRecon::Import"));
  }

  if(recon_event->GetGlobalEvent()) {
    throw(Squeal(Squeal::recoverable,
                 "Trying to replace GlobalEvent in ReconEvent",
                 "MapCppGlobalRecon::Import"));
  }
  
  // Create our new GlobalEvent
  MAUS::GlobalEvent* global_event = new MAUS::GlobalEvent();

  MAUS::DataStructure::Global::SpacePoint* spacepoint =
      new MAUS::DataStructure::Global::SpacePoint();

  spacepoint->set_charge(1234.0);
  spacepoint->set_detector(MAUS::DataStructure::Global::kTracker1);
  spacepoint->set_geometry_path("FakeGeomPath");
  
  global_event->add_spacepoint_check(spacepoint);
  
  spacepoint = new MAUS::DataStructure::Global::SpacePoint();

  spacepoint->set_charge(2345.0);
  spacepoint->set_detector(MAUS::DataStructure::Global::kTracker2);
  spacepoint->set_geometry_path("RealGeomPath");
  
  MAUS::DataStructure::Global::TrackPoint* trackpoint =
      new MAUS::DataStructure::Global::TrackPoint();

  trackpoint->set_charge(3456.0);
  trackpoint->set_spacepoint(spacepoint);

  global_event->add_trackpoint_recursive(trackpoint);

  // // Import the various reconstruction elements
  // MAUS::SciFiEvent* scifi_event = recon_event->GetSciFiEvent();
  // if(scifi_event) {
  //   MAUS::recon::global::ImportSciFiRecon scifirecon_importer;
  //   scifirecon_importer.process((*scifi_event), global_event, _classname);
  // }

  // Return the new GlobalEvent, to be added to the ReconEvent
  return global_event;
}


// void MapCppGlobalRecon::print_event_info(GlobalEvent &event) {
//   // std::cout << event.digits().size() << " "
//   //           << event.clusters().size() << " "
//   //           << event.spacepoints().size() << " "
//   //             << event.straightprtracks().size() << " " << std::endl;
// }

} // ~MAUS
