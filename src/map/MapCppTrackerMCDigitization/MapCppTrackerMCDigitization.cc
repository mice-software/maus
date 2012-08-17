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
#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"
#include "src/map/MapCppTrackerMCDigitization/MapCppTrackerMCDigitization.hh"


namespace MAUS {

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
  assert(_configJSON.isMember("simulation_geometry_filename"));
  std::string filename;
  filename = _configJSON["simulation_geometry_filename"].asString();
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
  if (!root.isMember("mc_events")) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << "I need an MC branch to simulate.";
    errors["missing_branch"] = ss.str();
    root["errors"] = errors;
    return writer.write(root);
  }

  Json::Value mc;
  mc = root.get("mc_events", 0);
  // check sanity of json input file and mc brach
  if ( !check_sanity_mc(mc) ) {
    // if bad, write error file
    return writer.write(root);
  }
  MAUS::SciFiSpill spill;
  Spill maus_spill;
  maus_spill.SetReconEvents(new ReconEventArray);

  // ==========================================================
  //  Loop over particle events and fill Event object with digits.
  for ( unsigned int i = 0; i < mc.size(); i++ ) {  // i-th particle
    Json::Value json_event = mc[i];

    if ( !json_event.isMember("sci_fi_hits") )
      continue;

    json_to_cpp(json_event, spill);
  } // ends loop particles
  // ================= Reconstruction =========================
  for ( unsigned int event_i = 0; event_i < spill.events().size(); event_i++ ) {
    MAUS::SciFiEvent * event = spill.events()[event_i];

    // std::cerr << "Hits in event: " << event.hits().size() << std::endl;
    if ( event->hits().size() ) {
      // for each fiber-hit, make a digit
      int _nSpill = root["spill_number"].asInt();
      construct_digits(*event, _nSpill, event_i);
    }
    ReconEvent *revt = new ReconEvent();
    revt->SetSciFiEvent(event);
    maus_spill.GetReconEvents()->push_back(revt);
    // std::cerr << "Digits in Event: " << event.digits().size() << " " << std::endl;
  }

  save_to_json(maus_spill);
  // ==========================================================
  return writer.write(root);
}

void MapCppTrackerMCDigitization::
     json_to_cpp(Json::Value js_event, MAUS::SciFiSpill &spill) {
  MAUS::SciFiEvent* event = new MAUS::SciFiEvent();
  Json::Value _hits = js_event["sci_fi_hits"];
  // std::cout << "Number of hits fed in: " << _hits.size() << std::endl;
  for ( unsigned int j = 0; j < _hits.size(); j++ ) {
    Json::Value hit = _hits[j];
    assert(hit.isMember("channel_id"));
    Json::Value channel_id = hit["channel_id"];

    int tracker, plane, station, fibre;
    double edep, time;

    tracker = channel_id["tracker_number"].asInt();
    station = channel_id["station_number"].asInt();
    plane   = channel_id["plane_number"].asInt();
    fibre   = channel_id["fibre_number"].asInt();
    edep    = hit["energy_deposited"].asDouble();
    time    = hit["time"].asDouble();
    MAUS::SciFiHit *a_hit = new MAUS::SciFiHit();
    a_hit->GetChannelId()->SetTrackerNumber(tracker);
    a_hit->GetChannelId()->SetStationNumber(station);
    a_hit->GetChannelId()->SetPlaneNumber(plane);
    a_hit->GetChannelId()->SetFibreNumber(fibre);
    a_hit->SetEnergyDeposited(edep);
    a_hit->SetTime(time);
    event->add_hit(a_hit);

    // .start. TO BE REMOVED .start.//
    double px, py, pz, x, y, z;
    px = hit["momentum"]["x"].asDouble();
    py = hit["momentum"]["y"].asDouble();
    pz = hit["momentum"]["z"].asDouble();
    x  = hit["position"]["x"].asDouble();
    y  = hit["position"]["y"].asDouble();
    z  = hit["position"]["z"].asDouble();
    ThreeVector position(x, y, z);
    ThreeVector momentum(px, py, pz);
    a_hit->SetPosition(position);
    a_hit->SetMomentum(momentum);
    // .end. TO BE REMOVED .end.//
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


void MapCppTrackerMCDigitization::construct_digits
    (MAUS::SciFiEvent &evt, int spill_num, int evnt_num) {
  int number_of_hits = evt.hits().size();
  for ( int hit_i = 0; hit_i < number_of_hits; hit_i++ ) {
    if ( !evt.hits()[hit_i]->GetChannelId()->GetUsed() ) {
      MAUS::SciFiHit *a_hit = evt.hits()[hit_i];
      a_hit->GetChannelId()->SetUsed(true);

      // Get nPE from this hit.
      double edep = a_hit->GetEnergyDeposited();
      double nPE  = compute_npe(edep);

      // Compute tdc count.
      double time   = a_hit->GetTime();
      // int tdcCounts = compute_tdc_counts(time);
      int chanNo = compute_chan_no(a_hit);

      // loop over all the other hits
      for ( int hit_j = hit_i+1; hit_j < number_of_hits; hit_j++ ) {
        if ( check_param(evt.hits()[hit_i], evt.hits()[hit_j]) ) {
          MAUS::SciFiHit *same_digit = evt.hits()[hit_j];
          double edep_j = same_digit->GetEnergyDeposited();
          nPE  += compute_npe(edep_j);
          same_digit->GetChannelId()->SetUsed(true);
        } // if-statement
      } // ends l-loop over all the array
      int tracker = a_hit->GetChannelId()->GetTrackerNumber();
      int station = a_hit->GetChannelId()->GetStationNumber();
      int plane = a_hit->GetChannelId()->GetPlaneNumber();
      int spill = spill_num;
      int event = evnt_num;
      SciFiDigit *a_digit = new SciFiDigit(spill, event,
                                           tracker, station, plane, chanNo, nPE, time);
      // .start. TO BE REMOVED .start.//
      ThreeVector position = a_hit->GetPosition();
      ThreeVector momentum = a_hit->GetMomentum();
      a_digit->set_true_position(position);
      a_digit->set_true_momentum(momentum);
      // .end. TO BE REMOVED .end.//
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

int MapCppTrackerMCDigitization::compute_chan_no(MAUS::SciFiHit *ahit) {
  int tracker = ahit->GetChannelId()->GetTrackerNumber();
  int station = ahit->GetChannelId()->GetStationNumber();
  int plane   = ahit->GetChannelId()->GetPlaneNumber();

  // std::cout << "Time: " << ahit->GetTime() << std::endl;
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
  // std::cerr << tracker << " " << station << " " << plane << std::endl;
  assert(this_plane != NULL);

  int numberFibres = static_cast<int> (7*2*(this_plane->propertyDouble("CentralFibre")+0.5));
  int fiberNumber = ahit->GetChannelId()->GetFibreNumber();
  int chanNo;

  if ( tracker == 0 ) {
    // start counting from the other end
    chanNo = static_cast<int> (floor((numberFibres-fiberNumber)/7.0));
  } else {
    chanNo = static_cast<int> (floor(fiberNumber/7.0));
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

bool MapCppTrackerMCDigitization::check_param(MAUS::SciFiHit *hit1, MAUS::SciFiHit *hit2) {
  if ( hit2->GetChannelId()->GetUsed() ) {
    return false;
  } else {
    // two hits passed to the check_param function
    int tracker1 = hit1->GetChannelId()->GetTrackerNumber();
    int tracker2 = hit2->GetChannelId()->GetTrackerNumber();
    int station1 = hit1->GetChannelId()->GetStationNumber();
    int station2 = hit2->GetChannelId()->GetStationNumber();
    int plane1   = hit1->GetChannelId()->GetPlaneNumber();
    int plane2   = hit2->GetChannelId()->GetPlaneNumber();
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

void MapCppTrackerMCDigitization::save_to_json(Spill &spill) {
  SpillProcessor spill_proc;
  root = *spill_proc.CppToJson(spill);
}

} // ~namespace MAUS

