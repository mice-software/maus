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

#include "src/map/MapCppTrackerMCDigitization/MapCppTrackerMCDigitization.hh"

bool MapCppTrackerMCDigitization::birth(std::string argJsonConfigDocument) {
  _classname = "MapCppTrackerMCDigitization";

  // JsonCpp string -> JSON::Value converter
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

  assert(_configJSON.isMember("SciFiNPECut"));
  SciFiNPECut = _configJSON["SciFiNPECut"].asDouble();

  return true;
}

bool MapCppTrackerMCDigitization::death() {
  return true;
}

std::string MapCppTrackerMCDigitization::process(std::string document) {
  // writes a line in the JSON document
  Json::FastWriter writer;
  // Check if the JSON document can be parsed, else return error only
  bool parsingSuccessful = reader.parse(document, root);
  if (!parsingSuccessful) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << reader.getFormatedErrorMessages();
    errors["bad_json_document"] = ss.str();
    root["errors"] = errors;
    return writer.write(root);
  }
  // Check if the JSON document has a 'mc' branch, else return error
  if (!root.isMember("mc")) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << "I need an MC branch to simulate.";
    errors["missing_branch"] = ss.str();
    root["errors"] = errors;
    return writer.write(root);
  }

  Json::Value mc;
  mc = root.get("mc", 0);
  // check sanity of json input file and mc brach
  if ( !check_sanity_mc(mc) ) {
    // if bad, write error file
    return writer.write(root);
  }
  SciFiSpill spill;

  // ==========================================================
  //  Loop over particle events and fill Event object with digits.
  for ( unsigned int i = 0; i < mc.size(); i++ ) {  // i-th particle
    Json::Value json_event = mc[i];

    if ( !json_event.isMember("hits") )
      continue; // if there are no MC hits, skip

    json_to_cpp(json_event, spill);
  } // ends loop particles
  // ================= Reconstruction =========================
  for ( unsigned int k = 0; k < spill.events().size(); k++ ) {
    SciFiEvent event = *(spill.events()[k]);

    // std::cout << "Hits in event: " << event.hits().size() << std::endl;
    if ( event.hits().size() ) {
      // for each fiber-hit, make a digit
      construct_digits(event);
    }
    // std::cout << "Digits in Event: " << event.digits().size() << " " << std::endl;

    save_to_json(event);
  }

  // ==========================================================
  return writer.write(root);
}

void MapCppTrackerMCDigitization::
     json_to_cpp(Json::Value js_event, SciFiSpill &spill) {
  SciFiEvent* event = new SciFiEvent();
  Json::Value _hits = js_event["hits"];
  // std::cout << "Number of hits fed in: " << _hits.size() << std::endl;
  for ( unsigned int j = 0; j < _hits.size(); j++ ) {
    Json::Value hit = _hits[j];
    assert(hit.isMember("channel_id"));
    Json::Value channel_id = hit["channel_id"];

    // assert this is a tracker hit
    assert(channel_id.isMember("type"));
    if (channel_id["type"].asString() != "Tracker") {
      continue;
    }

    int tracker, plane, station, fibre;
    double edep, time;

    tracker = channel_id["tracker_number"].asInt();
    station = channel_id["station_number"].asInt();
    plane   = channel_id["plane_number"].asInt();
    fibre   = channel_id["fibre_number"].asInt();
    edep    = hit["energy_deposited"].asDouble();
    time    = hit["time"].asDouble();
    SciFiHit *a_hit = new SciFiHit(tracker, station, plane, fibre, edep, time);
    event->add_hit(a_hit);
    // std::cout << "Number of hits stored in event: " << event->hits().size() << std::endl;
  }
  spill.add_event(event);
}

bool MapCppTrackerMCDigitization::check_sanity_mc(Json::Value mc) {
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


void MapCppTrackerMCDigitization::construct_digits(SciFiEvent &evt) {
  int number_of_hits = evt.hits().size();
  for ( int hit_i = 0; hit_i < number_of_hits; hit_i++ ) {
    if ( !evt.hits()[hit_i]->is_used() ) {
      SciFiHit *a_hit = evt.hits()[hit_i];

      // Get nPE from this hit.
      double edep = a_hit->get_edep();
      // std::cout << "Edep: " << edep << std::endl;
      double nPE  = compute_npe(edep);
      // Check whether the energy deposited is below the threshold;
      // if it is, skip this hit.
      if ( nPE < SciFiNPECut ) {
        continue;
      }

      // Compute tdc count.
      double time   = a_hit->get_time();
      // std::cout << "Time: " << time << std::endl;
      // int tdcCounts = compute_tdc_counts(time);
      int chanNo = compute_chan_no(a_hit);

      // loop over all the other hits
      for ( int hit_j = hit_i; hit_j < number_of_hits; hit_j++ ) {
        if ( check_param(evt.hits()[hit_i], evt.hits()[hit_j]) ) {
          SciFiHit *same_digit = evt.hits()[hit_j];
          double edep_j = same_digit->get_edep();
          nPE  += compute_npe(edep_j);
          same_digit->set_used();
        } // if-statement
      } // ends l-loop over all the array
      int tracker = a_hit->get_tracker();
      int station = a_hit->get_station();
      int plane = a_hit->get_plane();
      SciFiDigit *a_digit = new SciFiDigit(tracker, station, plane, chanNo, nPE, time);
      evt.add_digit(a_digit);
    }
  }  // ends 'for' loop over hits
}

int MapCppTrackerMCDigitization::compute_tdc_counts(double time) {
  double tmpcounts;
  assert(_configJSON.isMember("SciFivlpcTimeRes"));
  assert(_configJSON.isMember("SciFitdcFactor"));
  tmpcounts = CLHEP::RandGauss::shoot(time,
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

int MapCppTrackerMCDigitization::compute_chan_no(SciFiHit *ahit) {
  int tracker = ahit->get_tracker();
  int station = ahit->get_station();
  int plane   = ahit->get_plane();

  // std::cout << "Time: " << ahit->get_time() << std::endl;
  // std::cout << tracker << " " << station << " " << plane << std::endl;
  const MiceModule* this_plane = NULL;
  for ( unsigned int j = 0; !this_plane && j < modules.size(); j++ ) {
    if ( modules[j]->propertyExists("Tracker", "int") &&
         modules[j]->propertyInt("Tracker") == tracker &&
         modules[j]->propertyExists("Station", "int") &&
         modules[j]->propertyInt("Station") == station &&
         modules[j]->propertyExists("Plane", "int") &&
         modules[j]->propertyInt("Plane") == plane )
      // save the module corresponding to this plane
      this_plane = modules[j];
  }

  assert(this_plane != NULL);

  int numberFibres = 7*2*(this_plane->propertyDouble("CentralFibre")+0.5);
  int fiberNumber = ahit->get_fibre();
  int chanNo;

  if ( tracker == 0 ) {
    // start counting from the other end
    chanNo = floor((numberFibres-fiberNumber)/7.0);
  } else {
    chanNo = floor(fiberNumber/7.0);
  }

  return chanNo;
}

double MapCppTrackerMCDigitization::compute_npe(double edep) {
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

int MapCppTrackerMCDigitization::compute_adc_counts(double numb_pe) {
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

bool MapCppTrackerMCDigitization::check_param(SciFiHit *hit1, SciFiHit *hit2) {
  if ( hit2->is_used() ) {
    return false;
  } else {
    // two hits passed to the check_param function
    int tracker1 = hit1->get_tracker();
    int tracker2 = hit2->get_tracker();
    int station1 = hit1->get_station();
    int station2 = hit2->get_station();
    int plane1   = hit1->get_plane();
    int plane2   = hit2->get_plane();
    double chan1 = compute_chan_no(hit1);
    double chan2 = compute_chan_no(hit2);

    // check whether the hits belong to the same tracker, station, plane and channel
    if ( tracker1 == tracker2 && station1 == station2 &&
         plane1 == plane2 && chan1 == chan2 ) {
      return true;
    } else {
      return false;
    }
  }
}

void MapCppTrackerMCDigitization::save_to_json(SciFiEvent &evt) {
  Json::Value js_event;
  Json::Value digits_in_event;
  for ( unsigned int evt_i = 0; evt_i < evt.digits().size(); evt_i++ ) {
    Json::Value digits_in_event;
    digits_in_event["tracker"]= evt.digits()[evt_i]->get_tracker();
    digits_in_event["station"]= evt.digits()[evt_i]->get_station();
    digits_in_event["plane"]  = evt.digits()[evt_i]->get_plane();
    digits_in_event["channel"]= evt.digits()[evt_i]->get_channel();
    digits_in_event["npe"]    = evt.digits()[evt_i]->get_npe();
    digits_in_event["time"]   = evt.digits()[evt_i]->get_time();
    js_event.append(digits_in_event);
  }
  if (!js_event.isNull())
    root["digits"].append(js_event);
}
