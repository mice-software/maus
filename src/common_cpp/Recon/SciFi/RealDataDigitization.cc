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

#define MIN_ADC 0.000000001

RealDataDigitization::RealDataDigitization() : _npe_cut(0.0) {
  // Do nothing
}

RealDataDigitization::~RealDataDigitization() {
  // Do nothing
}

void RealDataDigitization::initialise(double npe_cut,
                                      const std::string& map_file,
                                      const std::string& calib_file) {
  // -------------------------------------------------
  // Load calibration, mapping and bad channel list.
  // These calls are to be replaced by CDB interface.
  _npe_cut = npe_cut;
  // bool map = load_mapping("scifi_mapping_2015-06-16.txt");
  // bool calib = load_calibration("scifi_calibration_2015-06-16.txt";
  bool map = load_mapping(map_file.c_str());
  bool calib = load_calibration(calib_file.c_str());
  bool bad_channels = load_bad_channels();
  if ( !calib || !map || !bad_channels ) {
    throw(Exception(Exception::recoverable,
          "Could not load Tracker calibration, mapping or bad channel list.",
          "RealDataDigitization::process"));
  }
}

void RealDataDigitization::process(Spill *spill) {
  // Check for DAQData
  if ( spill->GetDAQData() == NULL ) {
    // std::cerr << "RealDataDigitization::process No DAQ data found, aborting" << std::endl;
    return;
  }

  // Pull out the tracker daq objects
  Tracker0DaqArray tracker0 = spill->GetDAQData()->GetTracker0DaqArray();
  Tracker1DaqArray tracker1 = spill->GetDAQData()->GetTracker1DaqArray();

  if (!spill->GetReconEvents()) spill->SetReconEvents(new std::vector<ReconEvent*>());
  std::vector<ReconEvent*>* revts = spill->GetReconEvents();

  if ( (tracker0.size() != revts->size()) || (tracker1.size() != revts->size()) ) {
    std::cout << "WARNING: Tracker RealDataDigitization: "
              << "DAQ data size does not match Recon data size, aborting\n";
    return;
  }

  // Process the VLSB data to produce SciFiDigits
  for (size_t i = 0; i < tracker0.size(); ++i) {
    if (!revts->at(i)->GetSciFiEvent()) revts->at(i)->SetSciFiEvent(new SciFiEvent());
    std::vector<SciFiDigit*> digits0 = process_VLSB(spill->GetSpillNumber(), tracker0[i]);
    std::vector<SciFiDigit*> digits1 = process_VLSB(spill->GetSpillNumber(), tracker1[i]);
    digits0.insert(digits0.end(), digits1.begin(), digits1.end());
    revts->at(i)->GetSciFiEvent()->set_digits(digits0);
    // digits.insert(digits.end(), new_digits.begin(), new_digits.end());
  }
}

std::vector<SciFiDigit*> RealDataDigitization::process_VLSB(int SpillNum, TrackerDaq* td) {
  std::vector<SciFiDigit*> digits;

  // Loop over the VLSB channels of this event.
  unsigned int xVLSBArraySize = td->GetVLSBArraySize();
  for ( unsigned int i = 0; i < xVLSBArraySize; ++i ) {
    VLSB vlsb1 = td->GetVLSBArrayElement(i);


//    if ( bank < 32 ) {
//      vlsb_tracker0_array.push_back(vlsb);
//    } else {
//      vlsb_tracker1_array.push_back(vlsb);
//    }

    int bank = vlsb1.GetBankID();
    int channel_ro = vlsb1.GetChannel();
    double adc = vlsb1.GetADC();
    if ( !is_good_channel(bank, channel_ro) ) {
      continue;
    }

    // Get pedestal and gain from calibration.
    // int new_bank = bank + 4*board;
    double adc_pedestal = _calibration[bank][channel_ro]["adc_pedestal"].asDouble();
    double adc_gain     = _calibration[bank][channel_ro]["adc_gain"].asDouble();
    // double tdc_pedestal = _calibration[bank][channel_ro]["tdc_pedestal"].asDouble();
    // double tdc_gain     = _calibration[bank][channel_ro]["tdc_gain"].asDouble();

    // Calculate the number of photoelectrons.
    double pe;
    if ( adc_pedestal > MIN_ADC && adc_gain > MIN_ADC ) {
      pe = (adc-adc_pedestal)/adc_gain;
    } else {
      pe = -10.0;
    }

    // Get the time
    double time = -10.0;
    /* No TDC calibration yet
    if ( tdc_pedestal > tdc_pedestal_min && tdc_gain > 0 ) {
      time = (tdc-tdc_pedestal)/tdc_gain;
    } else {
      time = -10.0;
    }
    */

    // Find tracker, station, plane, channel.
    int board = floor(bank/4);
    int old_bank = bank%4;
    int tracker(-1);
    int station(-1);
    int plane(-1);
    int channel(-1);
    int extWG(-1);
    int inWG(-1);
    int WGfib(-1);
    bool found = get_StatPlaneChannel(board, old_bank, channel_ro,
                                      tracker, station, plane, channel,
                                      extWG, inWG, WGfib);
     // Exclude missing modules.
    if ( found && (pe > _npe_cut ) ) {
      SciFiDigit *digit = new SciFiDigit(SpillNum, vlsb1.GetPartEventNumber(),
                                         tracker, station, plane, channel, pe, time);
      digits.push_back(digit);
    }
  }  // ends loop over channels
  return digits;
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
    double adc_pedestal = _calibration[new_bank][channel_ro]["adc_pedestal"].asDouble();
    double adc_gain     = _calibration[new_bank][channel_ro]["adc_gain"].asDouble();
    double tdc_pedestal = _calibration[new_bank][channel_ro]["tdc_pedestal"].asDouble();
    double tdc_gain     = _calibration[new_bank][channel_ro]["tdc_gain"].asDouble();
    // Calculate the number of photoelectrons.
    double pe;
    if ( adc_pedestal > MIN_ADC && adc_gain > MIN_ADC ) {
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
    bool found = get_StatPlaneChannel(board, bank, channel_ro,
                                      tracker, station, plane,
                                      channel, extWG, inWG, WGfib);

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
    throw(Exception(Exception::recoverable,
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
    _calibration[bank][channel_n] = channel;
  }

  return true;
}

int RealDataDigitization::calc_uid(int chan_ro, int bank, int board) const {
  return (chan_ro + (bank*_number_channels) + (board*_banks_per_board*_number_channels));
}

bool RealDataDigitization::load_mapping(std::string file) {
  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
  std::string fname = std::string(pMAUS_ROOT_DIR)+"/files/cabling/"+file;
  std::cerr << "Filename: " << fname << "\n";

  std::ifstream inf(fname.c_str());
  if (!inf) {
    throw(Exception(Exception::recoverable,
          "Could not load Tracker Mapping.",
          "RealDataDigitization::load_mapping"));
  }

  std::string line;
  // for ( int i = 1; i < _total_number_channels; ++i ) {
    // getline(inf, line);
  while ( getline(inf, line) ) {
    std::istringstream ist1(line.c_str());
    ChanMap cmap;
    ist1 >> cmap.board >> cmap.bank >> cmap.chan_ro >> cmap.tracker >> cmap.station
         >> cmap.plane >> cmap.channel >> cmap.extWG >> cmap.inWG >> cmap.WGfib;
    int UId = calc_uid(cmap.chan_ro, cmap.bank, cmap.board);
    if (_chan_map.count(UId) != 0) {
      std::cerr << "WARNING: UId " << UId << " not unique! ";
      std::cerr << "chan_ro: " << cmap.chan_ro << ", bank: " << cmap.bank
                << ", board: " << cmap.board << "\n";
    }
    _chan_map[UId] = cmap;
  }
  return true;
}

bool RealDataDigitization::get_StatPlaneChannel(int& board, int& bank, int& chan_ro,
                           int& tracker, int& station, int& plane, int& channel,
                           int &extWG, int &inWG, int &WGfib) {
  bool found = false;
  tracker = station = plane = channel = -1;
  int UId = calc_uid(chan_ro, bank, board);

  if (_chan_map.count(UId) != 1) {
    std::cerr << "WARNING: UId " << UId << " not present! ";
    std::cerr << "chan_ro: " << chan_ro << ", bank: " << bank << ", board: " <<  board << "\n";
    return false;
  }

  tracker = _chan_map[UId].tracker;
  station = _chan_map[UId].station;
  plane = _chan_map[UId].plane;
  channel = _chan_map[UId].channel;
  extWG   = _chan_map[UId].extWG;
  inWG    = _chan_map[UId].inWG;
  WGfib   = _chan_map[UId].WGfib;
  found = true;

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
    throw(Exception(Exception::recoverable,
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
    _good_chan[bad_bank][bad_chan_ro] = false;
  }

  return true;
}

} // ~namespace MAUS
