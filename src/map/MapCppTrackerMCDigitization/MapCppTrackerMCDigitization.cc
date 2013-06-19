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

/*
NOTE: Commented out code that needs to pass a code review.
*/

namespace MAUS {
MapCppTrackerMCDigitization::MapCppTrackerMCDigitization()
    : _spill_json(NULL), _spill_cpp(NULL) {
}

MapCppTrackerMCDigitization::~MapCppTrackerMCDigitization() {
    if (_spill_json != NULL) {
        delete _spill_json;
    }
    if (_spill_cpp != NULL) {
        delete _spill_cpp;
    }
}

bool MapCppTrackerMCDigitization::birth(std::string argJsonConfigDocument) {
  _classname = "MapCppTrackerMCDigitization";

  try {
    if (!Globals::HasInstance()) {
      GlobalsManager::InitialiseGlobals(argJsonConfigDocument);
    }
    static MiceModule* mice_modules = Globals::GetMonteCarloMiceModules();
    modules = mice_modules->findModulesByPropertyString("SensitiveDetector", "SciFi");
    Json::Value *json = Globals::GetConfigurationCards();
    // Load constants.
    _SciFiNPECut        = (*json)["SciFiDigitNPECut"].asDouble();
    _SciFivlpcEnergyRes = (*json)["SciFivlpcEnergyRes"].asDouble();
    _SciFiadcFactor     = (*json)["SciFiadcFactor"].asDouble();
    _SciFitdcBits       = (*json)["SciFitdcBits"].asDouble();
    _SciFivlpcTimeRes   = (*json)["SciFivlpcTimeRes"].asDouble();
    _SciFitdcFactor     = (*json)["SciFitdcFactor"].asDouble();
    _SciFitdcBits       = (*json)["SciFitdcBits"].asDouble();
    _SciFiFiberConvFactor  = (*json)["SciFiFiberConvFactor"].asDouble();
    _SciFiFiberTrappingEff = (*json)["SciFiFiberTrappingEff"].asDouble();
    _SciFiFiberMirrorEff   = (*json)["SciFiFiberMirrorEff"].asDouble();
    _SciFivlpcQE           = (*json)["SciFivlpcQE"].asDouble();
    _SciFiFiberTransmissionEff = (*json)["SciFiFiberTransmissionEff"].asDouble();
    _SciFiMUXTransmissionEff   = (*json)["SciFiMUXTransmissionEff"].asDouble();
    _eV_to_phe = _SciFiFiberConvFactor *
                 _SciFiFiberTrappingEff *
                 ( 1.0 + _SciFiFiberMirrorEff ) *
                 _SciFiFiberTransmissionEff *
                 _SciFiMUXTransmissionEff *
                 _SciFivlpcQE;
    // ______________________________________________
    _configJSON = *json;
    assert(_configJSON.isMember("SciFiPerChanFlag"));
    if (_configJSON["SciFiPerChanFlag"].asInt()) {
      std::ifstream calib_file;
      calib_file.open("SciFiChanCal.txt");
      while ( calib_file.good() ) {
        std::string temp;
        getline(calib_file, temp);
        argCal += temp;
      }
      calib_reader.parse(argCal, _calib_list);
      // std::cerr << _calib_list["entries"][1]["good"] <<"\n";
    }
    // ______________________________________________
    return true;
  } catch(Squeal& squee) {
    MAUS::CppErrorHandler::getInstance()->HandleSquealNoJson(squee, _classname);
  } catch(std::exception& exc) {
    MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, _classname);
  }
  return false;
}

bool MapCppTrackerMCDigitization::death() {
  return true;
}

std::string MapCppTrackerMCDigitization::process(std::string document) {
  Json::FastWriter writer;

  // Set up a spill object, then continue only if MC event array is initialised
  read_in_json(document);
  Spill& spill = *_spill_cpp;

  if ( spill.GetMCEvents() ) {
  } else {
    std::cerr << "MC event array not initialised, aborting digitisation for this spill\n";
    MAUS::ErrorsMap errors = _spill_cpp->GetErrors();
    std::stringstream ss;
    ss << _classname << " says:" << "MC event array not initialised, aborting digitisation";
    errors["missing_branch"] = ss.str();
    save_to_json(spill);
    return writer.write(*_spill_json);
  }

  // ================= Reconstruction =========================

  // Check the Recon event array is initialised, and if not make it so
  if ( !spill.GetReconEvents() ) {
    ReconEventArray* revts = new ReconEventArray();
    spill.SetReconEvents(revts);
  }
  // Construct digits from hits for each MC event
  for ( unsigned int event_i = 0; event_i < spill.GetMCEventSize(); event_i++ ) {
    MCEvent *mc_evt = spill.GetMCEvents()->at(event_i);
    SciFiDigitPArray digits;
    int seed = mc_evt->GetPrimary()->GetRandomSeed();
    if ( mc_evt->GetSciFiHits() ) {
      construct_digits(mc_evt->GetSciFiHits(), spill.GetSpillNumber(),
                       static_cast<int>(event_i), digits);
    }

    // Adds Effects of Noise from Electrons to MC
    // if (_configJSON["SciFiNoiseFlag"].asInt()) {
    //  add_elec_noise(digits, seed, spill.GetSpillNumber(), static_cast<int>(event_i));
    // }

    // Make a SciFiEvent to hold the digits produced
    SciFiEvent *sf_evt = new SciFiEvent();
    sf_evt->set_digits(digits);

    // If there is already a Recon event associated with this MC event, add the SciFiEvent to it,
    // otherwise make a new Recon event to hold the SciFiEvent
    if ( spill.GetReconEvents()->size() > event_i ) {
      spill.GetReconEvents()->at(event_i)->SetSciFiEvent(sf_evt);
    } else {
      ReconEvent *revt = new ReconEvent();
      revt->SetPartEventNumber(event_i);
      revt->SetSciFiEvent(sf_evt);
      spill.GetReconEvents()->push_back(revt);
    }
  }
  // ==========================================================
  save_to_json(spill);
  return writer.write(*_spill_json);
}

void MapCppTrackerMCDigitization::read_in_json(std::string json_data) {
  Json::FastWriter writer;
  Json::Value json_root;
  if (_spill_cpp != NULL) {
    delete _spill_cpp;
    _spill_cpp = NULL;
  }

  try {
    json_root = JsonWrapper::StringToJson(json_data);
    SpillProcessor spill_proc;
    _spill_cpp = spill_proc.JsonToCpp(json_root);
  } catch(...) {
    Squeak::mout(Squeak::error) << "Bad json document" << std::endl;
    _spill_cpp = new Spill();
    MAUS::ErrorsMap errors = _spill_cpp->GetErrors();
    std::stringstream ss;
    ss << _classname << " says:" << reader.getFormatedErrorMessages();
    errors["bad_json_document"] = ss.str();
    _spill_cpp->GetErrors();
  }
}

void MapCppTrackerMCDigitization::construct_digits(SciFiHitArray *hits, int spill_num,
                                                   int event_num, SciFiDigitPArray &digits) {

  for ( unsigned int hit_i = 0; hit_i < hits->size(); hit_i++ ) {
    if ( !hits->at(hit_i).GetChannelId()->GetUsed() ) {
      SciFiHit *a_hit = &hits->at(hit_i);
      a_hit->GetChannelId()->SetUsed(true);

      int chanNo = compute_chan_no(a_hit);

       // Get nPE from this hit.
      double edep = a_hit->GetEnergyDeposited();
      double nPE  = edep*_eV_to_phe;
      // double nPE = compute_npe(edep, chanNo, a_hit);

      // Compute tdc count.
      double time1   = a_hit->GetTime();
      // int tdcCounts = compute_tdc_counts(time1);

      // Loop over all the other hits.
      for ( unsigned int hit_j = hit_i+1; hit_j < hits->size(); hit_j++ ) {
        if ( check_param(&(*hits)[hit_i], &(*hits)[hit_j]) ) {
          MAUS::SciFiHit *same_digit = &(*hits)[hit_j];
          double edep_j = same_digit->GetEnergyDeposited();
          nPE += edep_j*_eV_to_phe;
          same_digit->GetChannelId()->SetUsed(true);
        }
      } // ends loop over all the array
      int tracker = a_hit->GetChannelId()->GetTrackerNumber();
      int station = a_hit->GetChannelId()->GetStationNumber();
      int plane   = a_hit->GetChannelId()->GetPlaneNumber();

      SciFiDigit *a_digit = new SciFiDigit(spill_num, event_num,
                                           tracker, station, plane, chanNo, nPE, time1);
      // .start. TO BE REMOVED .start.//
      ThreeVector position = a_hit->GetPosition();
      ThreeVector momentum = a_hit->GetMomentum();
      a_digit->set_true_position(position);
      a_digit->set_true_momentum(momentum);
      // .end. TO BE REMOVED .end.//
      digits.push_back(a_digit);
    }
  } // ends 'for' loop over hits
}

void MapCppTrackerMCDigitization::add_elec_noise(SciFiDigitPArray &digits, int seed,
                                                 int spill_num, int event_num) {
  double numPE;
  int exist_flag, entry;
  double cross_sigma, cross_amp, dark_prob;
  double time1 = 0.;

  srand(seed);
  for ( int i = 0; i < modules.size(); i++ ) {
    int nChannels = 2*((modules[i]->propertyDouble("CentralFibre"))+0.5);
    for ( int j = 0; j < nChannels; j++ ) {
      numPE = 0.;
      exist_flag = 0;
      int tracker = modules[i]->propertyInt("Tracker");
      int station = modules[i]->propertyInt("Station");
      int plane   = modules[i]->propertyInt("Plane");

      for ( int k = 0; k < digits.size(); k++ ) {
        if ( digits[k]->get_tracker() == tracker &&
             digits[k]->get_station() == station &&
             digits[k]->get_plane()   == plane   &&
             digits[k]->get_channel() == j ) {
          numPE = digits[k]->get_npe();
          entry = k;
          exist_flag = 1;
          continue;
        }
      }

      if (_configJSON["SciFiPerChanFlag"].asInt()) {
        for ( int l = 0; l < _calib_list.size(); l++ ) {
          if ( _calib_list[l]["tracker"].asInt() == tracker &&
               _calib_list[l]["station"].asInt() == station &&
               _calib_list[l]["plane"].asInt()   == plane   &&
               _calib_list[l]["channel"].asInt() == j ) {
            if (_calib_list[l].isMember("SciFiCrossTalkSigma")) {
              cross_sigma = _calib_list[l]["SciFiCrossTalkSigma"].asDouble();
            } else { cross_sigma = _configJSON["SciFiCrossTalkSigma"].asDouble(); }
            if (_calib_list[l].isMember("SciFiCrossTalkAmplitude")) {
              cross_amp   = _calib_list[l]["SciFiCrossTalkAmplitude"].asDouble();
            } else { cross_amp   = _configJSON["SciFiCrossTalkAmplitude"].asDouble(); }
            if (_calib_list[l].isMember("SciFiDarkCountProababilty")) {
              dark_prob   = _calib_list[l]["SciFiDarkCountProababilty"].asDouble();
            } else { dark_prob   = _configJSON["SciFiDarkCountProababilty"].asDouble(); }
            continue;
          }
        }
      }  else {
         cross_sigma = _configJSON["SciFiCrossTalkSigma"].asDouble();
         cross_amp   = _configJSON["SciFiCrossTalkAmplitude"].asDouble();
         dark_prob   = _configJSON["SciFiDarkCountProababilty"].asDouble();
      }

      double param = (static_cast<double>(rand()) / RAND_MAX);
      double cross_param = -cross_sigma*(param - 1.0)*(param - 1.0);
      numPE += cross_amp * exp(cross_param);

      double dark_count = (static_cast<double>(rand()) / RAND_MAX) + dark_prob;
      while (dark_count > 1) {
        numPE += 1.0;
        dark_count = (static_cast<double>(rand()) / RAND_MAX) + dark_prob;
      }
      if ( exist_flag ) {
        digits[entry]->set_npe(numPE);
        continue;
      }
      std::cerr << _SciFiNPECut << " " << numPE << std::endl;
      if (numPE > _SciFiNPECut) {
        SciFiDigit *a_digit = new SciFiDigit(spill_num, event_num,
                                             tracker, station, plane,
                                             j, numPE, time1);
        digits.push_back(a_digit);
      }
    }
  }
}

int MapCppTrackerMCDigitization::compute_tdc_counts(double time1) {
  double tmpcounts;

  tmpcounts = CLHEP::RandGauss::shoot(time1, _SciFivlpcTimeRes)*_SciFitdcFactor;

  // Check for saturation of TDCs
  if ( tmpcounts > pow(2.0, _SciFitdcBits) - 1.0 ) {
    tmpcounts = pow(2.0, _SciFitdcBits) - 1.0;
  }

  int tdcCounts = static_cast <int> (floor(tmpcounts));
  return tdcCounts;
}

int MapCppTrackerMCDigitization::compute_chan_no(MAUS::SciFiHit *ahit) {
  // This is the channel number computed from the fibre number.
  int fiberNumber = ahit->GetChannelId()->GetFibreNumber();
  int chanNo      = static_cast<int> (floor(fiberNumber/7.0));

  return chanNo;
}

double MapCppTrackerMCDigitization::compute_npe(double edep,
                                                int chanNo,
                                                MAUS::SciFiHit *ahit) {
  int tracker, plane, station, channel;
  Json::Value _calibrations;
  tracker = ahit->GetChannelId()->GetTrackerNumber();
  station = ahit->GetChannelId()->GetStationNumber();
  plane   = ahit->GetChannelId()->GetPlaneNumber();

  if (_configJSON["SciFiPerChanFlag"].asInt()) {
    for (int i = 0; i < _calib_list.size(); i++) {
      if (_calib_list[i]["tracker"].asInt() == tracker &&
          _calib_list[i]["station"].asInt() == station &&
          _calib_list[i]["plane"].asInt()   == plane   &&
          _calib_list[i]["channel"].asInt() == chanNo) {
        if (_calib_list[i].isMember("SciFiFiberConvFactor")) {
          _calibrations["SciFiFiberConvFactor"] =
          _calib_list[i]["SciFiFiberConvFactor"];
        } else {_calibrations["SciFiFiberConvFactor"] =
                _configJSON["SciFiFiberConvFactor"];}
        if (_calib_list[i].isMember("SciFiFiberTrappingEff")) {
          _calibrations["SciFiFiberTrappingEff"] =
          _calib_list[i]["SciFiFiberTrappingEff"];
        } else {_calibrations["SciFiFiberTrappingEff"] =
                _configJSON["SciFiFiberTrappingEff"];}
        if (_calib_list[i].isMember("SciFiFiberMirrorEff")) {
          _calibrations["SciFiFiberMirrorEff"] =
          _calib_list[i]["SciFiFiberMirrorEff"];
        } else {_calibrations["SciFiFiberMirrorEff"] =
                _configJSON["SciFiFiberMirrorEff"];}
        if (_calib_list[i].isMember("SciFiFiberTransmissionEff")) {
          _calibrations["SciFiFiberTransmissionEff"] =
          _calib_list[i]["SciFiFiberTransmissionEff"];
        } else {_calibrations["SciFiFiberTransmissionEff"] =
                _configJSON["SciFiFiberTransmissionEff"];}
        if (_calib_list[i].isMember("SciFiMUXTransmissionEff")) {
          _calibrations["SciFiMUXTransmissionEff"] =
          _calib_list[i]["SciFiMUXTransmissionEff"];
        } else {_calibrations["SciFiMUXTransmissionEff"] =
                _configJSON["SciFiMUXTransmissionEff"];}
        if (_calib_list[i].isMember("SciFiMUXTransmissionEff")) {
          _calibrations["SciFivlpcQE"] =
          _calib_list[i]["SciFivlpcQE"];
        } else {_calibrations["SciFivlpcQE"] =
                _configJSON["SciFivlpcQE"];}
        continue;
      }
    }
  } else {
      _calibrations["SciFiFiberConvFactor"]      = _configJSON["SciFiFiberConvFactor"];
      _calibrations["SciFiFiberTrappingEff"]     = _configJSON["SciFiFiberTrappingEff"];
      _calibrations["SciFiFiberMirrorEff"]       = _configJSON["SciFiFiberMirrorEff"];
      _calibrations["SciFiFiberTransmissionEff"] = _configJSON["SciFiFiberTransmissionEff"];
      _calibrations["SciFiMUXTransmissionEff"]   = _configJSON["SciFiMUXTransmissionEff"];
      _calibrations["SciFivlpcQE"]               = _configJSON["SciFivlpcQE"];
  }

  double numbPE = edep*_eV_to_phe;

  return numbPE;
}

int MapCppTrackerMCDigitization::compute_adc_counts(double numb_pe) {
  double tmpcounts;
  if ( numb_pe == 0 ) return 0;

  // Throw the dice and generate the ADC count
  // value for the energy summed for each channel.
  tmpcounts = CLHEP::RandGauss::shoot(numb_pe, _SciFivlpcEnergyRes) *
              _SciFiadcFactor;

  // Check for saturation of ADCs
  if ( tmpcounts > pow(2.0, _SciFitdcBits) - 1.0 ) {
    tmpcounts = pow(2.0, _SciFitdcBits) - 1.0;
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
    if (_spill_json != NULL) {
        delete _spill_json;
        _spill_json = NULL;
    }
    _spill_json = spill_proc.CppToJson(spill, "");
}

} // ~namespace MAUS
