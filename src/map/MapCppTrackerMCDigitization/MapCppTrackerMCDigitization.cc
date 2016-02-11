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
#include <map>
#include "src/common_cpp/DataStructure/MCEvent.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"
#include "src/map/MapCppTrackerMCDigitization/MapCppTrackerMCDigitization.hh"
#include "src/common_cpp/Recon/SciFi/SciFiLookup.hh"
#include "src/common_cpp/API/PyWrapMapBase.hh"

namespace MAUS {
PyMODINIT_FUNC init_MapCppTrackerMCDigitization(void) {
  PyWrapMapBase<MAUS::MapCppTrackerMCDigitization>::PyWrapMapBaseModInit
                                ("MapCppTrackerMCDigitization", "", "", "", "");
}

MapCppTrackerMCDigitization::MapCppTrackerMCDigitization()
    : MapBase<Data>("MapCppTrackerMCDigitization") {
}

MapCppTrackerMCDigitization::~MapCppTrackerMCDigitization() {
}

void MapCppTrackerMCDigitization::_birth(const std::string& argJsonConfigDocument) {
  if (!Globals::HasInstance()) {
    GlobalsManager::InitialiseGlobals(argJsonConfigDocument);
  }
  static MiceModule* mice_modules = Globals::GetMonteCarloMiceModules();
  modules = mice_modules->findModulesByPropertyString("SensitiveDetector", "SciFi");
  Json::Value *json = Globals::GetConfigurationCards();
  // Load constants.
  _disc_sim_on = (*json)["SciFi_DiscOn"].asInt();
  _SciFiDisCut  = (*json)["SciFiDisCut"].asDouble();
  _SciFiadcBits = (*json)["SciFiadcBits"].asDouble();
  _SciFivlpcRes = (*json)["SciFivlpcRes"].asDouble();
  _SciFiNPECut        = (*json)["SciFiNoiseNPECut"].asDouble();
  _SciFitdcBits       = (*json)["SciFitdcBits"].asDouble();
  _SciFivlpcTimeRes   = (*json)["SciFivlpcTimeRes"].asDouble();
  _SciFitdcFactor     = (*json)["SciFitdcFactor"].asDouble();
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
  _mapping_file = (*json)["SciFiMappingFileName"].asString();
  _calibration_file = (*json)["SciFiCalibrationFileName"].asString();
  _bad_chan_file = (*json)["SciFiBadChannelsFileName"].asString();

  bool map = load_mapping(_mapping_file);
  bool calib = load_calibration(_calibration_file);
//  bool calib = load_calibration("scifi_calibration_jan2013.txt");
  bool bad_channels = load_bad_channels(_bad_chan_file);
  if ( !calib || !map || !bad_channels ) {
    throw(Exception(Exception::recoverable,
          "Could not load Tracker calibration, mapping or bad channel list.",
          "RealDataDigitization::process"));
  }
}

void MapCppTrackerMCDigitization::_death() {
}

void MapCppTrackerMCDigitization::_process(MAUS::Data* data) const {
  Spill& spill = *(data->GetSpill());

  if (!spill.GetMCEvents()) {
    throw MAUS::Exception(Exception::recoverable,
            "MC event array not initialised.",
            "MapCppTrackerMCDigitization::process");
  }

  // ================= Reconstruction =========================

  // Check the Recon event array is initialised, and if not make it so
  if ( !spill.GetReconEvents() ) {
    ReconEventPArray* revts = new ReconEventPArray();
    spill.SetReconEvents(revts);
  }
  // Construct digits from hits for each MC event
  for ( size_t event_i = 0; event_i < spill.GetMCEventSize(); event_i++ ) {
    MCEvent *mc_evt = spill.GetMCEvents()->at(event_i);
    SciFiDigitPArray digits;
    if ( mc_evt->GetSciFiHits() ) {
      construct_digits(mc_evt->GetSciFiHits(), spill.GetSpillNumber(), event_i, digits);
    }
    // Adding in effects from noise in VLPC
    if ( mc_evt->GetSciFiNoiseHits() ) {
      add_noise(mc_evt->GetSciFiNoiseHits(), digits);
    }
    // Smearing NPE results from ADC resolution
    for (size_t digit_j = 0; digit_j < digits.size(); digit_j++) {
      digits.at(digit_j)->set_npe(compute_adc_counts(digits.at(digit_j)));
    }
	  // For running with discriminators only
    if (_disc_sim_on == 1) {
  	  discriminator(digits);
    }
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
}

void MapCppTrackerMCDigitization::discriminator(SciFiDigitPArray &digits) const {
  std::vector<int> cut_pos;
  for (unsigned int i = 0; i < digits.size(); i++) {
    if (_SciFiDisCut < digits.at(i)->get_npe()) {
	  // std::cerr << "Good Point: " << digits.at(i)->get_npe() << "\n";
	  digits.at(i)->set_npe(10.0);
	} else {
	  // std::cerr << "Bad Point: " << digits.at(i)->get_npe() << "\n";
    std::cerr << "This shouldn't run.\n";
	  digits.at(i)->set_npe(-10.0);
	  cut_pos.push_back(i);
	}
  }
  for (unsigned int j = 0; j < cut_pos.size(); j++) {
    int pos_j = cut_pos.size() - j - 1;
	// std::cerr<< pos_j << " of " << cut_pos.size() << "\n";
	int k = cut_pos.at(pos_j);
    digits.erase(digits.begin()+k);
  }
}

void MapCppTrackerMCDigitization::construct_digits(SciFiHitArray *hits, int spill_num,
                                                   int event_num, SciFiDigitPArray &digits) const {
  SciFiLookup lookup;
  for ( unsigned int hit_i = 0; hit_i < hits->size(); hit_i++ ) {
    if ( !hits->at(hit_i).GetChannelId()->GetUsed() ) {

      SciFiHit *a_hit = &hits->at(hit_i);
      a_hit->GetChannelId()->SetUsed(true);

      int chanNo = compute_chan_no(a_hit);

       // Get nPE from this hit.
      double edep = a_hit->GetEnergyDeposited();
      double nPE  = edep*_eV_to_phe;

      // Compute tdc count.
      double time1   = a_hit->GetTime();
      // int tdcCounts = compute_tdc_counts(time1);

      // Pull tracker/station/plane information from geometry
      int tracker = a_hit->GetChannelId()->GetTrackerNumber();
      int station = a_hit->GetChannelId()->GetStationNumber();
      int plane   = a_hit->GetChannelId()->GetPlaneNumber();

      // Create Digit for use with MC lookup, will update NPE later
      SciFiDigit *a_digit = new SciFiDigit(spill_num, event_num,
                                           tracker, station, plane, chanNo, nPE, time1);

      a_hit->GetChannelId()->SetID(lookup.get_digit_id(a_digit));

      // Loop over all the other hits.
      for ( unsigned int hit_j = hit_i+1; hit_j < hits->size(); hit_j++ ) {
        if ( check_param(&(*hits)[hit_i], &(*hits)[hit_j]) ) {
          MAUS::SciFiHit *same_digit = &(*hits)[hit_j];
          double edep_j = same_digit->GetEnergyDeposited();
          nPE += edep_j*_eV_to_phe;
          same_digit->GetChannelId()->SetUsed(true);
          same_digit->GetChannelId()->SetID(lookup.get_digit_id(a_digit));
        }
      } // ends loop over all the array

      nPE = floor(nPE+0.5);
      a_digit->set_npe(nPE);
      a_digit->set_adc(150); // Just to test it
      digits.push_back(a_digit);
    }
  } // ends 'for' loop over hits
}

void MapCppTrackerMCDigitization::add_noise(SciFiNoiseHitArray *noises,
                                            SciFiDigitPArray &digits) const {

    /**************************************************************************
    *  Function checks which channel has noise against which channel has a
    *  digit.  If noise is in the same channel as a digit then the noise is
    *  added to the digit, else noise is added as a new digit.
    **************************************************************************/

  std::map<int, int> digit_map;
  for (size_t digit_i = 0; digit_i < digits.size(); digit_i++) {
    int track_id = digits.at(digit_i)->get_tracker();
    int stat_id  = digits.at(digit_i)->get_station();
    int plane_id = digits.at(digit_i)->get_plane();
    int chan_id  = digits.at(digit_i)->get_channel();
    int key = chan_id + 1000*plane_id + 10000*stat_id + 100000*track_id;
    digit_map[key] = digit_i;
  }
  for (size_t noise_j = 0; noise_j < noises->size(); noise_j++) {
    int track_id = noises->at(noise_j).GetTracker();
    int stat_id  = noises->at(noise_j).GetStation();
    int plane_id = noises->at(noise_j).GetPlane();
    int chan_id  = noises->at(noise_j).GetChannel();
    int key = chan_id + 1000*plane_id + 10000*stat_id + 100000*track_id;

    if (digit_map.find(key) != digit_map.end()) {
      double digit_npe = digits.at(digit_map[key])->get_npe();
      double noise_npe = noises->at(noise_j).GetNPE();
      digits.at(digit_map[key])->set_npe(digit_npe + noise_npe);
    } else {
      SciFiDigit* a_digit = new SciFiDigit(noises->at(noise_j).GetSpill(),
                                           noises->at(noise_j).GetEvent(),
                                           noises->at(noise_j).GetTracker(),
                                           noises->at(noise_j).GetStation(),
                                           noises->at(noise_j).GetPlane(),
                                           noises->at(noise_j).GetChannel(),
                                           noises->at(noise_j).GetNPE(),
                                           noises->at(noise_j).GetTime());
      a_digit->set_adc(57);
      digits.push_back(a_digit);
    }
  }
}

int MapCppTrackerMCDigitization::compute_tdc_counts(double time1) const {
  double tmpcounts;

  tmpcounts = CLHEP::RandGauss::shoot(time1, _SciFivlpcTimeRes)*_SciFitdcFactor;

  // Check for saturation of TDCs
  if ( tmpcounts > pow(2.0, _SciFitdcBits) - 1.0 ) {
    tmpcounts = pow(2.0, _SciFitdcBits) - 1.0;
  }

  int tdcCounts = static_cast <int> (floor(tmpcounts));
  return tdcCounts;
}

int MapCppTrackerMCDigitization::compute_chan_no(MAUS::SciFiHit *ahit) const {
  // This is the channel number computed from the fibre number.
  int fiberNumber = ahit->GetChannelId()->GetFibreNumber();
  int chanNo      = static_cast<int> (floor(fiberNumber/7.0));

  return chanNo;
}

double MapCppTrackerMCDigitization::compute_adc_counts(MAUS::SciFiDigit *digit) const {
  double numb_pe = digit->get_npe();
  if ( numb_pe == 0 ) return 0;

  // Throw the dice and generate VLPC output
  numb_pe = CLHEP::RandGauss::shoot(numb_pe, _SciFivlpcRes);
  if (numb_pe <= 0.0) return 0;

  // Gathering calibration information
  double _SciFiadcGain = 0;
  double _SciFiadcPed = 0;
  int test_flag = 0;
  for (int cal_i = 0; cal_i < _tracker.size(); cal_i++) {
    if (_tracker[cal_i] == digit->get_tracker() &&
        _station[cal_i] == digit->get_station() &&
        _view[cal_i]   == digit->get_plane() &&
        _fibre[cal_i] == digit->get_channel() ) {
      _SciFiadcGain =
	    _calibration[_bank[cal_i]][_chan_ro[cal_i]]["adc_gain"].asDouble();
      _SciFiadcPed =
	    _calibration[_bank[cal_i]][_chan_ro[cal_i]]["adc_pedestal"].asDouble();
	  test_flag = 1;
	  if (!_good_chan[_bank[cal_i]][_chan_ro[cal_i]]) {
		return numb_pe = -10.0;
	  }
	  continue;
	}
  }
  if ( test_flag == 0 ) {
	  return numb_pe = -10.0;
  }
  if (_SciFiadcGain == 0 || _SciFiadcPed == 0) {
    return numb_pe = -10.0;
  }

  // Check for saturation of ADCs
  double tmpcounts = (numb_pe * _SciFiadcGain) + _SciFiadcPed;
  if ( tmpcounts > pow(2.0, _SciFiadcBits) - 1.0 )
    tmpcounts = pow(2.0, _SciFiadcBits) - 1.0;
  tmpcounts = (floor(tmpcounts));
  numb_pe = (tmpcounts - _SciFiadcPed)/_SciFiadcGain;
  return numb_pe;
}

bool MapCppTrackerMCDigitization::check_param(MAUS::SciFiHit *hit1, MAUS::SciFiHit *hit2) const {
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

bool MapCppTrackerMCDigitization::load_calibration(std::string file) {
  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
  std::string fname = std::string(pMAUS_ROOT_DIR)+"/files/calibration/"+file;
  // std::string fname = std::string(pMAUS_ROOT_DIR)+"/files/calibration/"+file;

  std::ifstream inf(fname.c_str());

  if (!inf) {
    throw(Exception(Exception::recoverable,
          "Could not load Tracker Calibration.",
          "MapCppTrackerMCDigitization::load_calibration"));
  }

  std::string calib((std::istreambuf_iterator<char>(inf)), std::istreambuf_iterator<char>());

  Json::Reader reader;
  Json::Value calibration_data;
  if (!reader.parse(calib, calibration_data))
    return false;

  size_t n_channels = calibration_data.size();
  for ( Json::Value::ArrayIndex i = 0; i < n_channels; ++i ) {
    int bank            = calibration_data[i]["bank"].asInt();
    int channel_n       = calibration_data[i]["channel"].asInt();
    double adc_pedestal = calibration_data[i]["adc_pedestal"].asDouble();
    double adc_gain     = calibration_data[i]["adc_gain"].asDouble();
    double tdc_pedestal = calibration_data[i]["tdc_pedestal"].asDouble();
    double tdc_gain     = calibration_data[i]["tdc_gain"].asDouble();
    Json::Value channel;
    channel["adc_pedestal"] = adc_pedestal;
    channel["adc_gain"]     = adc_gain;
    channel["tdc_pedestal"] = tdc_pedestal;
    channel["tdc_gain"]     = tdc_gain;
    _calibration[bank][channel_n] = channel;
  }

  return true;
}

bool MapCppTrackerMCDigitization::load_mapping(std::string file) {
  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
  std::string fname = std::string(pMAUS_ROOT_DIR)+"/files/cabling/"+file;

  std::ifstream inf(fname.c_str());
  if (!inf) {
    throw(Exception(Exception::recoverable,
          "Could not load Tracker Mapping.",
          "MapCppTrackerMCDigitization::load_mapping"));
  }

  std::string line;
  for ( int i = 1; i < _total_number_channels; ++i ) {
    getline(inf, line);
    std::istringstream ist1(line.c_str());

    int board, bank, chan_ro, tracker, station, view, fibre, extWG, inWG, WGfib;
    ist1 >> board >> bank >> chan_ro >> tracker >> station >>
            view >> fibre >> extWG >> inWG >> WGfib;

// std::cerr<< "tracker: " << tracker << "   station: " << station << "   plane: " <<
// view << "   channel: " << chan_ro << "\n";

    _board.push_back(board);
    _bank.push_back(bank);
    _chan_ro.push_back(chan_ro);
    _tracker.push_back(tracker);
    _station.push_back(station);
    _view.push_back(view);
    _fibre.push_back(fibre);
    _extWG.push_back(extWG);
    _inWG.push_back(inWG);
    _WGfib.push_back(WGfib);
  }
  return true;
}

bool MapCppTrackerMCDigitization::load_bad_channels(std::string file) {
  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
  std::string fname = std::string(pMAUS_ROOT_DIR)+"/files/calibration/"+file;

  std::ifstream inf(fname.c_str());
  if (!inf) {
    throw(Exception(Exception::recoverable,
          "Could not load Tracker bad channel list.",
          "MapCppTrackerMCDigitization::load_bad_channels"));
  }

  for ( int bank = 0; bank < _number_banks; ++bank ) {
    for ( int chan_ro = 0; chan_ro < _number_channels; ++chan_ro ) {
      _good_chan[bank][chan_ro] = true;
    }
  }


  int bad_bank, bad_chan_ro;

  while ( !inf.eof() ) {
    inf >> bad_bank >> bad_chan_ro;
    _good_chan[bad_bank][bad_chan_ro] = false;
  }

  return true;
}
} // ~namespace MAUS
