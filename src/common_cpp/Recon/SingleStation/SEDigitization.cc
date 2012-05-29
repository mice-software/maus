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

#include "src/common_cpp/Recon/SingleStation/SEDigitization.hh"

SEDigitization::SEDigitization() {}

SEDigitization::~SEDigitization() {}

void SEDigitization::process(SESpill &spill, Json::Value const &daq) {
  // -------------------------------------------------
  // Load calibration, mapping and bad channel list.
  // These calls are to be replaced by CDB interface...
  // std::cout << "loading calibration" << std::endl;
  bool calib = load_calibration("se_calibration.txt");
  assert(calib);

  bool tdc_calib = load_tdc_calibration("se_tdc_calibration.txt");
  assert(tdc_calib);

  // std::cout << "loading mapping" << std::endl;
  bool map = load_mapping("se_mapping.txt");
  assert(map);
  // std::cout << "loading bad channel list" << std::endl;
  // bool bad_channels = load_bad_channels();
  // assert(bad_channels);
  // -------------------------------------------------
  // Pick up JSON daq event.
  Json::Value _events = daq["single_station"];
  std::cerr << daq["single_station"].size() << " " << daq["tof1"].size() << std::endl;
  // assert(daq["single_station"].size() == daq["tof1"].size());

  std::cout <<  _events.size() << std::endl;
  for ( unsigned int i = 0; i < _events.size(); ++i ) {

    SEEvent* event = new SEEvent();

    Json::Value input_event = _events[i]["VLSB"];
    // Loop over the digits of this event.
    for ( unsigned int j = 0; j < input_event.size(); ++j ) {
      Json::Value channel_in = input_event[j];
      assert(channel_in.isMember("phys_event_number"));
      assert(channel_in.isMember("part_event_number"));
      // assert(channel_in.isMember("geo"));
      assert(channel_in.isMember("bank_id"));
      assert(channel_in.isMember("channel"));
      assert(channel_in.isMember("adc"));
      assert(channel_in.isMember("tdc"));
      // assert(channel_in.isMember("discr"));

      int spill = channel_in["phys_event_number"].asInt();
      int eventNo = channel_in["part_event_number"].asInt();
      int bank = channel_in["bank_id"].asInt();
      int channel_ro = channel_in["channel"].asInt();
      int adc = channel_in["adc"].asInt();
      int tdc = channel_in["tdc"].asInt();

      // Get pedestal and gain from calibration.
      assert(_calibration[bank][channel_ro].isMember("pedestal"));
      assert(_calibration[bank][channel_ro].isMember("gain"));
      assert(_tdc_calibration[bank][channel_ro].isMember("pedestal"));
      assert(_tdc_calibration[bank][channel_ro].isMember("gain"));

      double pedestal =  _calibration[bank][channel_ro]["pedestal"].asDouble();
      double gain     = _calibration[bank][channel_ro]["gain"].asDouble();

      double tdc_y     = _tdc_calibration[bank][channel_ro]["pedestal"].asDouble();
      double tdc_slope = _tdc_calibration[bank][channel_ro]["gain"].asDouble();
      // Calculate the number of photoelectrons.
      double pe;
      if ( pedestal > 0. && gain > 0 ) {
        pe = (adc-pedestal)/gain;
      } else {
        pe = -10.0;
      }
      // Calculate the time
      double time;
      if ( tdc_y > 0. && tdc_slope > 0. ) {
        time = (tdc-tdc_y)/tdc_slope;
      } else {
        time = -10.0;
      }

      // int unique_chan  = _calibration[board][bank][channel_ro]["unique_chan"].asDouble();

      // Find plane and channel.
      int plane, channel;
      get_StatPlaneChannel(bank, channel_ro, plane, channel);

      int adc_ped = adc - pedestal;
      // Exclude missing modules.
      if ( plane != -1 ) { // pe > 1.0 &&
        SEDigit *digit = new SEDigit(spill, eventNo, plane, channel, pe, time, adc_ped);
        event->add_digit(digit);
      }
    }  // ends loop over channels (j)
    spill.add_event(event);
  }  // ends loop over events (i)
}

bool SEDigitization::load_tdc_calibration(std::string file) {
  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
  std::string fname = std::string(pMAUS_ROOT_DIR)+"/src/map/MapCppSingleStationRecon/"+file;

  std::ifstream inf(fname.c_str());

  if (!inf) {
    std::cout << "Unable to open file " << fname << std::endl;
    return false;
  }

  std::string line;

  int numBanks;
  getline(inf, line);
  std::istringstream ist1(line.c_str());
  ist1 >> numBanks;

  read_in_tdc_calib(inf);

  return true;
}

void SEDigitization::read_in_tdc_calib(std::ifstream &inf) {
  std::string line;

  // run over banks
  for ( int i = 0; i < 16; ++i ) {
    // run over channels
    for ( int j = 0; j < 128; ++j ) { // running over channels
      int bank, chan;
      double p, g;

      getline(inf, line);
      std::istringstream ist1(line.c_str());
      ist1 >> bank >> chan >> p >> g;

        // int temp = unique_chan_no;

      Json::Value channel;
      channel["pedestal"]=p;
      channel["gain"]=g;
      _tdc_calibration[i].push_back(channel);
    }
  }
}

bool SEDigitization::load_calibration(std::string file) {
  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
  std::string fname = std::string(pMAUS_ROOT_DIR)+"/src/map/MapCppSingleStationRecon/"+file;

  std::ifstream inf(fname.c_str());

  if (!inf) {
    std::cout << "Unable to open file " << fname << std::endl;
    return false;
  }

  std::string line;

  int numBoards;
  getline(inf, line);
  std::istringstream ist1(line.c_str());
  ist1 >> numBoards;

  read_in_all_Boards(inf);

  return true;
}

void SEDigitization::read_in_all_Boards(std::ifstream &inf) {
  std::string line;

  // run over banks
  for ( int i = 0; i < 16; ++i ) {
    // run over channels
    for ( int j = 0; j < 128; ++j ) { // running over channels
      int bank, chan;
      double p, g;

      getline(inf, line);
      std::istringstream ist1(line.c_str());
      ist1 >> bank >> chan >> p >> g;

        // int temp = unique_chan_no;

      Json::Value channel;
      channel["pedestal"]=p;
      channel["gain"]=g;
      _calibration[i].push_back(channel);
    }
  }
}

bool SEDigitization::load_mapping(std::string file) {
  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
  std::string fname = std::string(pMAUS_ROOT_DIR)+"/src/map/MapCppSingleStationRecon/"+file;

  std::ifstream inf(fname.c_str());
  if (!inf) {
    std::cout << "Unable to open file " << fname << std::endl;
    return false;
  }

  std::string line;
  for ( int i = 0; i < 640; ++i ) {
    getline(inf, line);
    std::istringstream ist1(line.c_str());

    int bank, chan_ro, view, fibre;
    ist1 >> bank >> chan_ro >> view >> fibre;

    _bank.push_back(bank);
    _chan_ro.push_back(chan_ro);
    _view.push_back(view);
    _fibre.push_back(fibre);
  }
  return true;
}

void SEDigitization::
     get_StatPlaneChannel(int& bank, int& chan_ro,
                          int& plane, int& channel) {
  bool found = false;
  plane = channel = -1;

  for ( unsigned int i = 0; !found && i < _view.size(); ++i ) {
    if ( (bank == _bank[i]) && (chan_ro == _chan_ro[i]) ) {
      plane = _view[i];
      channel = _fibre[i];
      found = true;
    }
  }
  // std::cerr << bank << " " << chan_ro << std::endl;
  // assert(found);
}
