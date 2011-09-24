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

#include "src/map/MapCppTrackerDigitization/MapCppTrackerDigitization.hh"
#include "Config/MiceModule.hh"

#include "Interface/Squeak.hh"

bool MapCppTrackerDigitization::birth(std::string argJsonConfigDocument) {
  _classname = "MapCppTrackerDigitization";

  //  JsonCpp string -> JSON::Value converter
  Json::Reader reader;

  // Check if the JSON document can be parsed, else return error only
  bool parsingSuccessful = reader.parse(argJsonConfigDocument, _configJSON);
  if (!parsingSuccessful) {
    return false;
  }

  // Get the tracker modules; they will be necessary
  // for the channel number calculation
  assert(_configJSON.isMember("reconstruction_geometry_filename"));
  std::string filename;
  filename = _configJSON["reconstruction_geometry_filename"].asString();
  _module = new MiceModule(filename);
  modules = _module->findModulesByPropertyString("SensitiveDetector", "SciFi");

  return true;
}

bool MapCppTrackerDigitization::death() {
  return true;
}

std::string MapCppTrackerDigitization::process(std::string document) {
  // writes a line in the JSON document
  Json::FastWriter writer;

  // check sanity of json input file and mc brach
  if ( !check_sanity_mc(document) ) {
    // if bad, write error file
    return writer.write(root);
  }

  // start processing the mc hits
  std::vector<Json::Value> _alldigits;
  _alldigits.push_back(Json::Value());
  Json::Value tracker_event;

  // ==========================================================
  //  Loop over particles and hits
  for ( unsigned int i = 0; i < mc.size(); i++ ) {  //  i-th particle
    Json::Value particle = mc[i];

    if ( !particle.isMember("hits") ) {
      continue; // if there are no MC hits, skip
    }
    // "_hits" contains all mc hits for this event
    Json::Value _hits = particle["hits"];

    // reset the digits array for each new event/particle
    _alldigits.clear();
    // for each fiber-hit, make a digit
    _alldigits = make_all_digits(_hits);

    // sum bundle of 7 fibers
    tracker_event.clear();
    tracker_event = make_bundle(_alldigits);

    root["digits"].append(tracker_event);
  } // end for with var 'i' to loop particles
  // ==========================================================
  return writer.write(root);
}

std::vector<Json::Value> MapCppTrackerDigitization::make_all_digits(Json::Value hits) {
  std::vector<Json::Value> _digits;
  _digits.clear();

  for ( unsigned int j = 0; j < hits.size(); j++ ) {  //  j-th hit
      // Json::StyledWriter writer;
      // picking one hit at a time
      Json::Value hit = hits[j];

      // a sanity check
      assert(hit.isMember("channel_id"));
      Json::Value channel_id = hit["channel_id"];

      // assert this is a tracker hit
      assert(channel_id.isMember("type"));
      if (channel_id["type"].asString() != "Tracker") {
        continue;
      }

      assert(hit.isMember("energy_deposited"));
      double edep = hit["energy_deposited"].asDouble();

      // get nPE from this hit
      double nPE = get_npe(edep);

      // compute tdc count
      int tdcCounts = get_tdc_counts(hit);

      assert(_configJSON.isMember("SciFiNPECut"));
      double SciFiNPECut = _configJSON["SciFiNPECut"].asDouble();
      // check whether the energy deposited is below the threshold;
      // if it is, skip this hit.
      if ( nPE < SciFiNPECut ) {
        continue;
      }

      if (!root.isMember("digits")) {
        root["digits"] = Json::Value(Json::arrayValue);
      }
      assert(root["digits"].isArray());

      assert(hit.isMember("channel_id"));
      assert(hit.isMember("momentum"));
      assert(hit.isMember("time"));
      // assert(hit.isMember("position"));

      int chanNo = get_chan_no(hit);

      Json::Value adigit;
      adigit["tdc_counts"] = tdcCounts;
      adigit["number_photoelectrons"] = nPE;
      adigit["channel_id"] = hit["channel_id"];
      // introducing the Channel Number calculated from the fibre number
      adigit["channel_id"]["channel_number"]=chanNo;
      adigit["mc_mom"] = hit["momentum"];
      adigit["time"] = hit["time"];
      adigit["mc_position"]=hit["position"];
      adigit["isUsed"] = 0;

      _digits.push_back(adigit);
    }  //  ends 'for' loop over hits
    return _digits;
}

bool MapCppTrackerDigitization::check_sanity_mc(std::string document) {
  // Check if the JSON document can be parsed, else return error only
  bool parsingSuccessful = reader.parse(document, root);
  if (!parsingSuccessful) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << reader.getFormatedErrorMessages();
    errors["bad_json_document"] = ss.str();
    root["errors"] = errors;
    return false;
  }

  // Check if the JSON document has a 'mc' branch, else return error
  if (!root.isMember("mc")) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << "I need an MC branch to simulate.";
    errors["missing_branch"] = ss.str();
    root["errors"] = errors;
    return false;
  }

  mc = root.get("mc", 0);
  // Check if JSON document is of the right type, else return error
  if (!mc.isArray()) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << "MC branch isn't an array";
    errors["bad_type"] = ss.str();
    root["errors"] = errors;
    return false;
  }
  return true;
}

int MapCppTrackerDigitization::get_tdc_counts(Json::Value ahit) {
  double tmpcounts;
  assert(_configJSON.isMember("SciFivlpcTimeRes"));
  assert(_configJSON.isMember("SciFitdcFactor"));
  tmpcounts = CLHEP::RandGauss::shoot(ahit["time"].asDouble(),
                                      _configJSON["SciFivlpcTimeRes"].asDouble())*
                                      _configJSON["SciFitdcFactor"].asDouble();

  // Check for saturation of TDCs
  assert(_configJSON.isMember("SciFitdcBits"));
  if ( tmpcounts > pow(2.0, _configJSON["SciFitdcBits"].asDouble()) - 1.0 ) {
    tmpcounts = pow(2.0, _configJSON["SciFitdcBits"].asDouble()) - 1.0;
  }

  int tdcCounts = static_cast <int> (floor(tmpcounts));
  return tdcCounts;
}

int MapCppTrackerDigitization::get_chan_no(Json::Value ahit) {
  assert(ahit.isMember("channel_id"));
  Json::Value channel_id = ahit["channel_id"];

  assert(channel_id.isMember("tracker_number"));
  assert(channel_id.isMember("station_number"));
  assert(channel_id.isMember("plane_number"));

  const MiceModule* this_plane = NULL;
  for ( unsigned int j = 0; !this_plane && j < modules.size(); ++j ) {
    if ( modules[j]->propertyExists("Tracker", "int") &&
         modules[j]->propertyInt("Tracker") == channel_id["tracker_number"].asInt() &&
         modules[j]->propertyExists("Station", "int") &&
         modules[j]->propertyInt("Station") == channel_id["station_number"].asInt() &&
         modules[j]->propertyExists("Plane", "int") &&
         modules[j]->propertyInt("Plane") == channel_id["plane_number"].asInt() )
      // save the module corresponding to this plane
      this_plane = modules[j];
  }

  assert(this_plane != NULL);

  int numberFibres = 7*2*(this_plane->propertyDouble("CentralFibre")+0.5);
  assert(channel_id.isMember("fiber_number"));
  int fiberNumber = channel_id["fiber_number"].asInt();
  int _chanNo;

  if ( channel_id["tracker_number"].asInt() == 0 ) {
    // start counting from the other end
    _chanNo = floor((numberFibres-fiberNumber)/7.0);
  } else {
       _chanNo = floor(fiberNumber/7.0);
  }
  return _chanNo;
}

double MapCppTrackerDigitization::get_npe(double edep) {
      // implement dead channels
      assert(_configJSON.isMember("SciFiFiberConvFactor"));
      double numbPE = _configJSON["SciFiFiberConvFactor"].asDouble() * edep;

      assert(_configJSON.isMember("SciFiFiberTrappingEff"));
      numbPE *= _configJSON["SciFiFiberTrappingEff"].asDouble();

      assert(_configJSON.isMember("SciFiFiberMirrorEff"));
      numbPE *= ( 1.0 + _configJSON["SciFiFiberMirrorEff"].asDouble());

      assert(_configJSON.isMember("SciFiFiberTransmissionEff"));
      numbPE *= _configJSON["SciFiFiberTransmissionEff"].asDouble();

      assert(_configJSON.isMember("SciFiMUXTransmissionEff"));
      numbPE *= _configJSON["SciFiMUXTransmissionEff"].asDouble();

      assert(_configJSON.isMember("SciFivlpcQE"));
      numbPE *= _configJSON["SciFivlpcQE"].asDouble();

      return numbPE;
}

int MapCppTrackerDigitization::compute_adc_counts(double numb_pe) {
  double tmpcounts;
  if ( numb_pe == 0 ) return 0;

  // Throw the dice and generate the ADC count
  // value for the energy summed for each channel
  assert(_configJSON.isMember("SciFivlpcEnergyRes"));

  // get adc count by generating a gaussian distribution with mean "numb_pe"
  tmpcounts = CLHEP::RandGauss::shoot(numb_pe,
                                      _configJSON["SciFivlpcEnergyRes"].asDouble());

  assert(_configJSON.isMember("SciFiadcFactor"));
  tmpcounts *= _configJSON["SciFiadcFactor"].asDouble();

  // Check for saturation of ADCs
  assert(_configJSON.isMember("SciFitdcBits"));
  if ( tmpcounts > pow(2.0, _configJSON["SciFitdcBits"].asDouble()) - 1.0 ) {
    tmpcounts = pow(2.0, _configJSON["SciFitdcBits"].asDouble()) - 1.0;
  }

  int adcCounts = static_cast <int> (floor(tmpcounts));
  return adcCounts;
}

Json::Value MapCppTrackerDigitization::make_bundle(std::vector<Json::Value> _alldigits) {
  Json::Value tracker_event;
  double npe;
  // pick an element of the array
  for (unsigned int digit_i = 0; digit_i < _alldigits.size(); digit_i++) {
    if ( _alldigits[digit_i]["isUsed"].asInt() == 0 ) {
      npe = _alldigits[digit_i]["number_photoelectrons"].asDouble();
      // loop over all the others
      for ( unsigned int digit_j = digit_i; digit_j < _alldigits.size(); digit_j++ ) {
        if ( check_param(&(_alldigits[digit_i]), &(_alldigits[digit_j])) ) {
          npe += _alldigits[digit_j]["number_photoelectrons"].asDouble();
          _alldigits[digit_j]["isUsed"]=1;
        } // if-statement
      } // ends l-loop over all the array

      // now, store this digit!
      digit["tdc_counts"] = _alldigits[digit_i]["tdc_counts"];
      digit["number_photoelectrons"] = npe;
      digit["channel_id"] = _alldigits[digit_i]["channel_id"];
      digit["mc_position"] = _alldigits[digit_i]["mc_position"];
      // assert((_alldigits[digit_i]).isMember("mc_mom"));
      digit["mc_mom"] = _alldigits[digit_i]["mc_mom"];
      digit["time"] = _alldigits[digit_i]["time"];
      digit["adc_counts"] = compute_adc_counts(npe);
      tracker_event.append(digit);
      _alldigits[digit_i]["isUsed"]=1;
    } // ends if-statement
  } // ends k-loop
  return tracker_event;
}

bool MapCppTrackerDigitization::check_param(Json::Value* hit1, Json::Value* hit2) {
  // two hits passed to the check_param function
  int tracker1 = (*hit1)["channel_id"]["tracker_number"].asInt();
  int tracker2 = (*hit2)["channel_id"]["tracker_number"].asInt();
  int station1 = (*hit1)["channel_id"]["station_number"].asInt();
  int station2 = (*hit2)["channel_id"]["station_number"].asInt();
  int plane1   = (*hit1)["channel_id"]["plane_number"].asInt();
  int plane2   = (*hit2)["channel_id"]["plane_number"].asInt();
  double chan1 = (*hit1)["channel_id"]["channel_number"].asDouble();
  double chan2 = (*hit2)["channel_id"]["channel_number"].asDouble();
  int hit_is_used = (*hit2)["isUsed"].asInt();

  // check whether the hits belong to the same tracker, station, plane and channel
  if ( tracker1 == tracker2 && station1 == station2 &&
       plane1 == plane2 && chan1 == chan2 && !hit_is_used ) {
    return true;
  } else {
    return false;
  }
}

// the following two functions are added for testing purposes only
Json::Value MapCppTrackerDigitization::ConvertToJson(std::string jsonString) {
  Json::Value newJson;
  // JsonCpp string -> JSON::Value converter
  Json::Reader reader;

  newJson = JsonWrapper::StringToJson(jsonString);
  return newJson;
}

std::string MapCppTrackerDigitization::JsonToString(Json::Value json_in) {
  std::stringstream ss_io;
  JsonWrapper::Print(ss_io, json_in);
  return ss_io.str();
}
