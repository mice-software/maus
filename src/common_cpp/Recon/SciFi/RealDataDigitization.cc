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

// NOTE: Equipment list to be changed
// when tracker moved to MICE Hall.
// A few assertions to be removed.
// ES

#include "src/common_cpp/Recon/SciFi/RealDataDigitization.hh"

#include "src/common_cpp/DataStructure/DAQData.hh"

namespace MAUS {

RealDataDigitization::RealDataDigitization() {}

RealDataDigitization::~RealDataDigitization() {}

void RealDataDigitization::initialise() {
  // -------------------------------------------------
  // Load calibration, mapping and bad channel list.
  // These calls are to be replaced by CDB interface...
  bool map = load_mapping("mapping_7.txt");
  bool calib = load_calibration("scifi_calibration_jan2013.txt");
  bool bad_channels = load_bad_channels();
  if ( !calib || !map || !bad_channels ) {
    throw(Squeal(Squeal::recoverable,
          "Could not load Tracker calibration, mapping or bad channel list.",
          "RealDataDigitization::process"));
  }
}

void RealDataDigitization::process(Spill *spill, Json::Value const &daq) {
  // Check for existant pointers to ReconEvents and DAQData
  if ( spill->GetDAQData() == NULL )
    spill->SetDAQData(new DAQData());

  if (spill->GetReconEvents() == NULL)
    spill->SetReconEvents(new ReconEventArray());

  // Pick up JSON daq events.
  Json::Value tracker_event = daq["tracker1"];

  Tracker0DaqArray tracker0;
  Tracker1DaqArray tracker1;
  for ( unsigned int i = 0; i < tracker_event.size(); ++i ) { // loop over events
    SciFiEvent* event = new SciFiEvent();
    TrackerDaq *tracker0daq_event = new TrackerDaq();
    TrackerDaq *tracker1daq_event = new TrackerDaq();

    if ( tracker_event[i].isMember("VLSB_C") ) {
      Json::Value input_event = tracker_event[i]["VLSB_C"];
      // Merge tracker events.
      for ( size_t idig = 0; idig < daq["tracker2"][i]["VLSB_C"].size(); ++idig ) {
        input_event[input_event.size()] = daq["tracker2"][i]["VLSB_C"][idig];
      }
      process_VLSB_c(input_event, event, tracker0daq_event, tracker1daq_event);
    } else if ( tracker_event[i].isMember("VLSB") ) {
      Json::Value input_event = tracker_event[i]["VLSB"];
      // Merge tracker events.
      for ( size_t idig = 0; idig < daq["tracker2"][i]["VLSB"].size(); ++idig ) {
        input_event[input_event.size()] = daq["tracker2"][i]["VLSB"][idig];
      }
      process_VLSB(input_event, event, tracker0daq_event, tracker1daq_event);
    } else {
      continue;
    }

    tracker0.push_back(tracker0daq_event); // end of event. push back.
    tracker1.push_back(tracker1daq_event); // end of event. push back.

    ReconEvent * revt = new ReconEvent();
    // revt->SetSciFiEvent(new SciFiEvent(*event));
    revt->SetSciFiEvent(event);
    spill->GetReconEvents()->push_back(revt);
    //delete event;
  }  // ends loop over events (i)
  spill->GetDAQData()->SetTracker0DaqArray(tracker0);
  spill->GetDAQData()->SetTracker1DaqArray(tracker1);
}

void RealDataDigitization::process_VLSB(Json::Value input_event,
                                          SciFiEvent* event,
                                          TrackerDaq *tracker0daq_event,
                                          TrackerDaq *tracker1daq_event) {
  VLSBArray vlsb_tracker0_array;
  VLSBArray vlsb_tracker1_array;

  // Loop over the VLSB channels of this event.
  for ( unsigned int j = 0; j < input_event.size(); ++j ) {
    Json::Value channel_in = input_event[j];
    int ldc = channel_in["ldc_id"].asInt();
    std::string detector = channel_in["detector"].asString();
    int discriminator = channel_in["discriminator"].asInt();
    int equip_type = channel_in["equip_type"].asInt();
    int time_stamp = channel_in["time_stamp"].asInt();
    int spill = channel_in["phys_event_number"].asInt();
    int eventNo = channel_in["part_event_number"].asInt();
    int bank = channel_in["bank"].asInt();
    int channel_ro = channel_in["channel"].asInt();
    int adc = channel_in["adc"].asInt();
    int tdc = channel_in["tdc"].asInt();

    VLSB vlsb;
    vlsb.SetEquipType(equip_type);
    vlsb.SetPhysEventNumber(spill);
    vlsb.SetTimeStamp(time_stamp);
    vlsb.SetDetector(detector);
    vlsb.SetPartEventNumber(eventNo);
    vlsb.SetChannel(channel_ro);
    vlsb.SetBankID(bank);
    vlsb.SetADC(adc);
    vlsb.SetTDC(tdc);
    vlsb.SetDiscriminator(discriminator);
    vlsb.SetLdcId(ldc);

    if ( bank < 32 ) {
      vlsb_tracker0_array.push_back(vlsb);
    } else {
      vlsb_tracker1_array.push_back(vlsb);
    }

    if ( !is_good_channel(bank, channel_ro) ) {
      continue;
    }

    // Get pedestal and gain from calibration.
    // int new_bank = bank + 4*board;
    double adc_pedestal = calibration_[bank][channel_ro]["adc_pedestal"].asDouble();
    double adc_gain     = calibration_[bank][channel_ro]["adc_gain"].asDouble();
    double tdc_pedestal = calibration_[bank][channel_ro]["tdc_pedestal"].asDouble();
    double tdc_gain     = calibration_[bank][channel_ro]["tdc_gain"].asDouble();
    // Calculate the number of photoelectrons.
    double pe;
    if ( adc_pedestal > _min && adc_gain > _min ) {
      pe = (adc-adc_pedestal)/adc_gain;
    } else {
      pe = -10.0;
    }
    double time = -10.0;
    /* No TDC calibration yet.
    if ( tdc_pedestal > tdc_pedestal_min && tdc_gain > 0 ) {
      time = (tdc-tdc_pedestal)/tdc_gain;
    } else {
      time = -10.0;
    }
    */
    // Find tracker, station, plane, channel.
    int board = floor(bank/4);
    int old_bank = bank%4;
    int tracker, station, plane, channel;
    bool found = false; // get_StatPlaneChannel(board, old_bank, channel_ro,
                        //                      tracker, station, plane, channel);
     // Exclude missing modules.
    if ( found ) { // pe > 1.0 &&
      SciFiDigit *digit = new SciFiDigit(spill, eventNo,
                                         tracker, station, plane, channel, pe, time);
      event->add_digit(digit);
    }
  }  // ends loop over channels (j)
  tracker0daq_event->SetVLSBArray(vlsb_tracker0_array); // fill event with all vlsb digits
  tracker1daq_event->SetVLSBArray(vlsb_tracker1_array); // fill event with all vlsb digits

  // std::cerr << "VLSB array sizes: " << std::endl;
  // std::cerr << tracker0daq_event->GetVLSBArraySize() << std::endl;
  // std::cerr << tracker1daq_event->GetVLSBArraySize() << std::endl;
}

void RealDataDigitization::process_VLSB_c(Json::Value input_event,
                                          SciFiEvent* event,
                                          TrackerDaq *tracker0daq_event,
                                          TrackerDaq *tracker1daq_event) {

  VLSB_CArray vlsb_c_tracker0_array;
  VLSB_CArray vlsb_c_tracker1_array;
  // Loop over the VLSB channels of this event.
  for ( unsigned int j = 0; j < input_event.size(); ++j ) {
    Json::Value channel_in = input_event[j];
    int ldc = channel_in["ldc_id"].asInt();
    std::string detector = channel_in["detector"].asString();
    int discriminator = channel_in["discriminator"].asInt();
    int equip_type = channel_in["equip_type"].asInt();
    int time_stamp = channel_in["time_stamp"].asInt();
    int spill = channel_in["phys_event_number"].asInt();
    int eventNo = channel_in["part_event_number"].asInt();
    int board = channel_in["geo"].asInt()-1;
    int bank = channel_in["bank"].asInt();
    int channel_ro = channel_in["channel"].asInt();
    int adc = channel_in["adc"].asInt();
    int tdc = channel_in["tdc"].asInt();

    VLSB_C vlsb_c;
    vlsb_c.SetEquipType(equip_type);
    vlsb_c.SetPhysEventNumber(spill);
    vlsb_c.SetTimeStamp(time_stamp);
    vlsb_c.SetDetector(detector);
    vlsb_c.SetPartEventNumber(eventNo);
    vlsb_c.SetChannel(channel_ro);
    vlsb_c.SetBankID(bank);
    vlsb_c.SetADC(adc);
    vlsb_c.SetTDC(tdc);
    vlsb_c.SetDiscriminator(discriminator);
    vlsb_c.SetLdcId(ldc);
    vlsb_c.SetGeo(board);

    if ( board < 4 ) {
      vlsb_c_tracker0_array.push_back(vlsb_c);
    } else {
      vlsb_c_tracker1_array.push_back(vlsb_c);
    }

    if ( !is_good_channel(bank, channel_ro) ) {
      // continue;
    }

    // Get pedestal and gain from calibration.
    int new_bank = bank + 4*board;
    double adc_pedestal = calibration_[new_bank][channel_ro]["adc_pedestal"].asDouble();
    double adc_gain     = calibration_[new_bank][channel_ro]["adc_gain"].asDouble();
    double tdc_pedestal = calibration_[new_bank][channel_ro]["tdc_pedestal"].asDouble();
    double tdc_gain     = calibration_[new_bank][channel_ro]["tdc_gain"].asDouble();
    // Calculate the number of photoelectrons.
    double pe;
    if ( adc_pedestal > _min && adc_gain > _min ) {
      pe = (adc-adc_pedestal)/adc_gain;
    } else {
      pe = -10.0;
    }
    double time = -10.0;
    /* No TDC calibration yet.
    if ( tdc_pedestal > tdc_pedestal_min && tdc_gain > 0 ) {
      time = (tdc-tdc_pedestal)/tdc_gain;
    } else {
      time = -10.0;
    }
    */
    // Find tracker, station, plane, channel.
    int tracker, station, plane, channel;
    int extWG, inWG, WGfib;
    bool found = get_StatPlaneChannel(board, bank, channel_ro, tracker, station, plane, channel, extWG, inWG, WGfib);

    std::ofstream myfile;
    myfile.open ("realdata.txt", std::ios::app);
    myfile << board << " " << bank << " " << channel_ro << " "
           << extWG << " " << inWG << " " << WGfib << " "
           << tracker << " " << station << " " << plane << " "
           << channel << " " << pe << "\n";
    myfile.close();

     // Exclude missing modules.
    if ( found ) { // pe > 1.0 &&
      SciFiDigit *digit = new SciFiDigit(spill, eventNo,
                                         tracker, station, plane, channel, pe, time);
      event->add_digit(digit);
    }
  }  // ends loop over channels (j)
  // Fill event with all vlsb digits.
  tracker0daq_event->SetVLSB_CArray(vlsb_c_tracker0_array);
  tracker1daq_event->SetVLSB_CArray(vlsb_c_tracker1_array);
}

bool RealDataDigitization::load_calibration(std::string file) {
  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
  std::string fname = std::string(pMAUS_ROOT_DIR)+"/files/calibration/"+file;

  std::ifstream inf(fname.c_str());

  if (!inf) {
    throw(Squeal(Squeal::recoverable,
          "Could not load Tracker Calibration.",
          "RealDataDigitization::load_calibration"));
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
    calibration_[bank][channel_n] = channel;
  }

  return true;
}

bool RealDataDigitization::load_mapping(std::string file) {
  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
  std::string fname = std::string(pMAUS_ROOT_DIR)+"/src/map/MapCppTrackerDigits/"+file;

  std::ifstream inf(fname.c_str());
  if (!inf) {
    throw(Squeal(Squeal::recoverable,
          "Could not load Tracker Mapping.",
          "RealDataDigitization::load_mapping"));
  }

  std::string line;
  for ( int i = 1; i < _total_number_channels; ++i ) {
    getline(inf, line);
    std::istringstream ist1(line.c_str());

    int board, bank, chan_ro, tracker, station, view, fibre, extWG, inWG, WGfib;
    ist1 >> board >> bank >> chan_ro >> tracker >> station >>
            view >> fibre >> extWG >> inWG >> WGfib;

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

bool RealDataDigitization::get_StatPlaneChannel(int& board, int& bank, int& chan_ro,
                           int& tracker, int& station, int& plane, int& channel,
                           int &extWG, int &inWG, int &WGfib) const {
  bool found = false;
  tracker = station = plane = channel = -1;

  for ( size_t i = 0; !found && i < _board.size(); ++i ) {
    if ( (board == _board[i]) && (bank == _bank[i]) && (chan_ro == _chan_ro[i]) ) {
      station = _station[i];
      plane = _view[i];
      channel = _fibre[i];
      tracker = _tracker[i];
      extWG   = _extWG[i];
      inWG    = _inWG[i];
      WGfib   = _WGfib[i];
      found = true;
    }
  }
  return found;
}

bool RealDataDigitization::is_good_channel(const int bank,
                                           const int chan_ro) const {
  if ( bank < _number_banks && chan_ro < _number_channels ) {
    return _good_chan[bank][chan_ro];
  } else {
    return false;
  }
}

bool RealDataDigitization::load_bad_channels() {
  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
  std::string fname = std::string(pMAUS_ROOT_DIR)+"/src/map/MapCppTrackerDigits/bad_chan_list.txt";

  std::ifstream inf(fname.c_str());
  if (!inf) {
    throw(Squeal(Squeal::recoverable,
          "Could not load Tracker bad channel list.",
          "RealDataDigitization::load_bad_channels"));
  }

  for ( int bank = 0; bank < _number_banks; ++bank ) {
    for ( int chan_ro = 0; chan_ro < _number_channels; ++chan_ro ) {
      _good_chan[bank][chan_ro] = true;
    }
  }


  int bad_bank, bad_chan_ro;

  while ( !inf.eof() ) {
    inf >> bad_bank >> bad_chan_ro;
    //_good_chan[bad_bank][bad_chan_ro] = false;
  }

  return true;
}

} // ~namespace MAUS
