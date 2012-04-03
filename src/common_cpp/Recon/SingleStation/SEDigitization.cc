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

// namespace MAUS {

SEDigitization::SEDigitization() {}

SEDigitization::~SEDigitization() {}

void SEDigitization::process(SESpill &spill, Json::Value const &daq) {
  // -------------------------------------------------
  // Load calibration, mapping and bad channel list.
  // These calls are to be replaced by CDB interface...
  bool calib = load_calibration("se_calibration.txt");
  assert(calib);
  bool map = load_mapping("se_mapping.txt");
  assert(map);
  bool bad_channels = load_bad_channels();
  assert(bad_channels);
  // -------------------------------------------------

  // Pick up JSON daq event.
  Json::Value _event = daq["single_station"];

  for ( unsigned int i = 1; i < _event.size(); ++i ) {
    SEEvent* event = new SEEvent();

    Json::Value input_event = _event[i]["VLSB"];

    // Loop over the digits of this event.
    for ( unsigned int j = 0; j < input_event.size(); ++j ) {
      Json::Value channel_in = input_event[j];
      assert(channel_in.isMember("phys_event_number"));
      assert(channel_in.isMember("part_event_number"));
      // assert(channel_in.isMember("geo"));
      assert(channel_in.isMember("bank"));
      assert(channel_in.isMember("channel"));
      assert(channel_in.isMember("adc"));
      assert(channel_in.isMember("tdc"));
      // assert(channel_in.isMember("discr"));
      int board =3;
      // int spill = channel_in["phys_event_number"].asInt();
      // int eventNo = channel_in["part_event_number"].asInt();
      // int board = channel_in["geo"].asInt()-1;
      int bank = channel_in["bank"].asInt();
      int channel_ro = channel_in["channel"].asInt();
      int adc = channel_in["adc"].asInt();
      int tdc = channel_in["tdc"].asInt();

      if ( !is_good_channel(bank, channel_ro) ) {
        continue;
      }

      // Get pedestal and gain from calibration.
      assert(_calibration[board][bank][channel_ro].isMember("pedestal"));
      assert(_calibration[board][bank][channel_ro].isMember("gain"));
      double pedestal =  _calibration[board][bank][channel_ro]["pedestal"].asDouble();
      double gain     = _calibration[board][bank][channel_ro]["gain"].asDouble();

      // Calculate the number of photoelectrons.
      double pe;
      if ( pedestal > 0. && gain > 0 ) {
        pe = (adc-pedestal)/gain;
      } else {
        pe = -10.0;
      }
      // int unique_chan  = _calibration[board][bank][channel_ro]["unique_chan"].asDouble();

      // Find plane and channel.
      int plane, channel;
      get_StatPlaneChannel(board, bank, channel_ro, plane, channel);

      // Exclude missing modules.
      if ( pe > 1.0 && plane != -1 ) {
        SEDigit *digit = new SEDigit(plane, channel, pe, tdc);
        event->add_digit(digit);
      }
    }  // ends loop over channels (j)
    spill.add_event(event);
  }  // ends loop over events (i)
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

  // run over all boards
  for ( int i = 0; i < 2; ++i ) {
    // run over banks
    for ( int j = 0; j < 4; ++j ) {
      // run over channels
      for ( int k = 0; k < 128; ++k ) { // running over channels
        int unique_chan_no, board, bank, chan;
        double p, g;

        getline(inf, line);
        std::istringstream ist1(line.c_str());
        ist1 >> unique_chan_no >> board >> bank >> chan >> p >> g;

        // int temp = unique_chan_no;

        Json::Value channel;
        channel["pedestal"]=p;
        channel["gain"]=g;
        channel["unique_chan"]=unique_chan_no;
        _calibration[i][j].push_back(channel);
      }
    }
  }
}

bool SEDigitization::load_mapping(std::string file) {
  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
  std::string fname = std::string(pMAUS_ROOT_DIR)+"/src/map/MapCppTrackerRecon/"+file;

  std::ifstream inf(fname.c_str());
  if (!inf) {
    std::cout << "Unable to open file " << fname << std::endl;
    return false;
  }

  std::string line;
  for ( int i = 1; i < 6403; ++i ) {
    getline(inf, line);
    std::istringstream ist1(line.c_str());

    int board, bank, chan_ro, tracker, station, view, fibre, extWG, inWG, WGfib;
    ist1 >> board >> bank >> chan_ro >> tracker >> station >>
            view >> fibre >> extWG >> inWG >> WGfib;

    _board.push_back(board);
    _bank.push_back(bank);
    _chan_ro.push_back(chan_ro);
    _view.push_back(view);
    _fibre.push_back(fibre);
    _extWG.push_back(extWG);
    _inWG.push_back(inWG);
    _WGfib.push_back(WGfib);
  }
  return true;
}

void SEDigitization::
     get_StatPlaneChannel(int& board, int& bank, int& chan_ro,
                          int& plane, int& channel) {
  bool found = false;
  plane = channel = -1;

  for ( unsigned int i = 0; !found && i < _board.size(); ++i ) {
    if ( (board == _board[i]) && (bank == _bank[i]) && (chan_ro == _chan_ro[i]) ) {
      plane = _view[i];
      channel = _fibre[i];
      found = true;
    }
  }

  // assert(found);
}

bool SEDigitization::is_good_channel(const int bank, const int chan_ro) {
  return good_chan[bank][chan_ro];
}

bool SEDigitization::load_bad_channels() {
  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
  std::string fname = std::string(pMAUS_ROOT_DIR)+"/src/map/MapCppTrackerRecon/bad_chan_list.txt";

  std::ifstream inf(fname.c_str());
  if (!inf) {
    std::cout << "Unable to open file " << fname << std::endl;
    return false;
  }

  for ( int board = 0; board < 2; ++board ) {
    for ( int bank = 0; bank < 4; ++bank ) {
      for ( int chan_ro = 0; chan_ro < 128; ++chan_ro ) {
        good_chan[board][bank][chan_ro] = true;
      }
    }
  }

  int bad_board, bad_bank, bad_chan_ro;

  while ( !inf.eof() ) {
    inf >> bad_board >> bad_bank >> bad_chan_ro;
    good_chan[bad_board][bad_bank][bad_chan_ro] = false;
  }
  return true;
}
// } // ~namespace MAUS
