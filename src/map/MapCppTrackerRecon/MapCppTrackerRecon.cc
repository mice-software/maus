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

#include "src/map/MapCppTrackerRecon/MapCppTrackerRecon.hh"

bool MapCppTrackerRecon::birth(std::string argJsonConfigDocument) {
  _classname = "MapCppTrackerRecon";

  // JsonCpp string -> JSON::Value converter
  Json::Reader reader;

  // Check if the JSON document can be parsed, else return error only.
  bool parsingSuccessful = reader.parse(argJsonConfigDocument, _configJSON);
  if (!parsingSuccessful) {
    return false;
  }

  // Get the tracker modules.
  assert(_configJSON.isMember("reconstruction_geometry_filename"));
  std::string filename;
  filename = _configJSON["reconstruction_geometry_filename"].asString();
  MiceModule* _module;
  _module = new MiceModule(filename);
  modules = _module->findModulesByPropertyString("SensitiveDetector", "SciFi");

  // Get minPE cut value.
  assert(_configJSON.isMember("SciFiNPECut"));
  minPE = _configJSON["SciFiNPECut"].asDouble();

  // Get the value above which an Exception is thrown
  assert(_configJSON.isMember("SciFiClustExcept"));
  ClustException = _configJSON["SciFiClustExcept"].asDouble();

  return true;
}

bool MapCppTrackerRecon::death() {
  return true;
}

std::string MapCppTrackerRecon::process(std::string document) {
  // Writes a line in the JSON document.
  Json::FastWriter writer;
  SciFiSpill spill;
  spill.events_in_spill.clear();

  try {
    root = JsonWrapper::StringToJson(document);
    digitization(spill, root);
  } catch(...) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << reader.getFormatedErrorMessages();
    errors["bad_json_document"] = ss.str();
    root["errors"] = errors;
    return writer.write(root);
  }

  try { // ================= Reconstruction =========================
    for ( int k = 0; k < spill.events_in_spill.size(); k++ ) {
      SciFiEvent event = spill.events_in_spill[k];
      // Build Clusters.
      if ( event.scifidigits.size() ) {
        cluster_recon(event);
      }
      // Build SpacePoints.
      if ( event.scificlusters.size() ) {
        spacepoint_recon(event);
      }
      // Pattern Recognition.
      if ( event.scifispacepoints.size() ) {
        pattern_recognition(event);
      }
      // print_event_info(event);

      save_to_json(event);
    } // ==========================================================
  } catch(...) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << reader.getFormatedErrorMessages();
    errors["recon_failed"] = ss.str();
    root["errors"] = errors;
    return writer.write(root);
  }

  return writer.write(root);
}

// === The Digits vectors is filled, either by running real data ==========
// === digitization or by reading-in an already existing digits branch.====
void MapCppTrackerRecon::digitization(SciFiSpill &spill, Json::Value &root) {
  if ( root.isMember("daq_data") && !root["daq_data"].isNull() ) {
    Json::Value daq = root.get("daq_data", 0);
    RealDataDigitization real;
    real.process(spill, daq);
  } else if ( root.isMember("digits") ) {
    Json::Value digits = root.get("digits", 0);
    fill_digits_vector(digits, spill);
  } else {
    throw 0;
  }
}

void MapCppTrackerRecon::fill_digits_vector(Json::Value &digits_event, SciFiSpill &a_spill) {
  for ( unsigned int i = 0; i < digits_event.size(); i++ ) {
    SciFiEvent an_event;
    Json::Value digits;
    digits = digits_event[i];
    for ( unsigned int j = 0; j < digits.size(); j++ ) {
      Json::Value digit;
      digit = digits[j];
      int tracker, station, plane, channel, npe, time;
      tracker = digit["tracker"].asInt();
      station = digit["station"].asInt();
      plane   = digit["plane"].asInt();
      channel = digit["channel"].asInt();
      npe     = digit["npe"].asInt();
      time    = digit["time"].asInt();
      SciFiDigit* a_digit = new SciFiDigit(tracker, station, plane, channel, npe, time);
      an_event.scifidigits.push_back(a_digit);
    } // ends loop over digits in the event
    a_spill.events_in_spill.push_back(an_event);
  } // ends loop over events.
}

void MapCppTrackerRecon::cluster_recon(SciFiEvent &evt) {
  SciFiClusterRec clustering(ClustException, minPE);
  clustering.process(evt, modules);
}

void MapCppTrackerRecon::spacepoint_recon(SciFiEvent &evt) {
  SciFiSpacePointRec spacepoints;
  spacepoints.process(evt);
}

void MapCppTrackerRecon::pattern_recognition(SciFiEvent &evt) {
  PatternRecognition pr1;
  pr1.process(evt);
}

void MapCppTrackerRecon::save_to_json(SciFiEvent &evt) {
  Json::Value digits;
  for ( unsigned int dig_i = 0; dig_i < evt.scifidigits.size(); dig_i++ ) {
    Json::Value digits_in_event;
    digits_in_event["tracker"]= evt.scifidigits[dig_i]->get_tracker();
    digits_in_event["station"]= evt.scifidigits[dig_i]->get_station();
    digits_in_event["plane"]  = evt.scifidigits[dig_i]->get_plane();
    digits_in_event["channel"]= evt.scifidigits[dig_i]->get_channel();
    digits_in_event["npe"]    = evt.scifidigits[dig_i]->get_npe();
    digits_in_event["time"]   = evt.scifidigits[dig_i]->get_time();
    digits.append(digits_in_event);
  }
  root["digits"].append(digits);

  Json::Value tracker0;
  Json::Value tracker1;
  Json::Value channels;
  for ( unsigned int evt_i = 0; evt_i < evt.scifispacepoints.size(); evt_i++ ) {
    Json::Value spacepoints_in_event;
    channels = Json::Value(Json::arrayValue);
    channels.clear();
    int tracker = evt.scifispacepoints[evt_i]->get_tracker();
    spacepoints_in_event["tracker"]= tracker;
    spacepoints_in_event["station"]= evt.scifispacepoints[evt_i]->get_station();
    spacepoints_in_event["npe"]    = evt.scifispacepoints[evt_i]->get_npe();
    // spacepoints_in_event["time"]   = evt.scifispacepoints[evt_i]->get_time();
    spacepoints_in_event["type"]   = evt.scifispacepoints[evt_i]->get_type();
    Hep3Vector pos = evt.scifispacepoints[evt_i]->get_position();
    spacepoints_in_event["position"]["x"]   = pos.x();
    spacepoints_in_event["position"]["y"]   = pos.y();
    spacepoints_in_event["position"]["z"]   = pos.z();
    std::vector<SciFiCluster*> clusters_in_spacepoint = evt.scifispacepoints[evt_i]->get_channels();
    for ( unsigned int cl = 0; cl < clusters_in_spacepoint.size(); cl++ ) {
      Json::Value cluster;
      cluster["plane_number"] = clusters_in_spacepoint[cl]->get_plane();
      cluster["channel_number"] = clusters_in_spacepoint[cl]->get_channel();
      cluster["npe"] = clusters_in_spacepoint[cl]->get_npe();
      channels.append(cluster);
    }
    spacepoints_in_event["channels"] = channels;
    if ( tracker == 0 ) {
      tracker0.append(spacepoints_in_event);
    }
    if ( tracker == 1 ) {
      tracker1.append(spacepoints_in_event);
    }
  }
  root["space_points"]["tracker1"].append(tracker0);
  root["space_points"]["tracker2"].append(tracker1);
}

void MapCppTrackerRecon::print_event_info(SciFiEvent &event) {
  std::cout << event.scifidigits.size() << " "
            << event.scificlusters.size() << " "
            << event.scifispacepoints.size() << " "
	    << event.scifistraightprtracks.size() << " " << std::endl;
}

// the following two functions are added for testing purposes only
Json::Value MapCppTrackerRecon::ConvertToJson(std::string jsonString) {
  Json::Value newJson;
  // JsonCpp string -> JSON::Value converter
  Json::Reader reader;

  newJson = JsonWrapper::StringToJson(jsonString);
  return newJson;
}

std::string MapCppTrackerRecon::JsonToString(Json::Value json_in) {
  std::stringstream ss_io;
  JsonWrapper::Print(ss_io, json_in);
  return ss_io.str();
}