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

namespace MAUS {

RealDataDigitization::RealDataDigitization() {}

RealDataDigitization::~RealDataDigitization() {}

void RealDataDigitization::process(Spill &spill, Json::Value const &daq) {
  // Check to see if the spill ReconEventArray pointer has been initialised
  if (spill.GetReconEvents() == NULL)
    spill.SetReconEvents(new ReconEventArray());

  // -------------------------------------------------
  // Load calibration, mapping and bad channel list.
  // These calls are to be replaced by CDB interface...
  bool calib = load_calibration("scifi_calibration_30_09_2011.txt");
  bool map = load_mapping("mapping_7.txt");
  bool bad_channels = load_bad_channels();
  if ( !calib || !map || !bad_channels ) {
    throw(Squeal(Squeal::recoverable,
          "Could not load Tracker calibration, mapping or bad channel list.",
          "RealDataDigitization::process"));
  }
  // -------------------------------------------------

  // Pick up JSON daq event.
  Json::Value tracker_event = daq["tracker1"];

  for ( unsigned int i = 0; i < tracker_event.size(); ++i ) {
    SciFiEvent* event = new SciFiEvent();

    Json::Value input_event = tracker_event[i]["VLSB_C"];
    // Merge tracker events.
    for ( unsigned int idig = 0; idig < daq["tracker2"][i]["VLSB_C"].size(); ++idig ) {
      input_event[input_event.size()] = daq["tracker2"][i]["VLSB_C"][idig];
    }

    // Loop over the digits of this event.
    for ( unsigned int j = 0; j < input_event.size(); ++j ) {
      Json::Value channel_in = input_event[j];
      assert(channel_in.isMember("phys_event_number"));
      assert(channel_in.isMember("part_event_number"));
      assert(channel_in.isMember("geo"));
      assert(channel_in.isMember("bank"));
      assert(channel_in.isMember("channel"));
      assert(channel_in.isMember("adc"));
      assert(channel_in.isMember("tdc"));
      // assert(channel_in.isMember("discr"));

      int spill = channel_in["phys_event_number"].asInt();
      int eventNo = channel_in["part_event_number"].asInt();
      int board = channel_in["geo"].asInt()-1;
      int bank = channel_in["bank"].asInt();
      int channel_ro = channel_in["channel"].asInt();
      int adc = channel_in["adc"].asInt();
      int tdc = channel_in["tdc"].asInt();

      if ( !is_good_channel(board, bank, channel_ro) ) {
        continue;
      }

      // Get pedestal and gain from calibration.
      double pedestal =  _calibration_pedestal[board][bank][channel_ro];
      double gain     =  _calibration_gain[board][bank][channel_ro];

      // Calculate the number of photoelectrons.
      double pe;
      if ( pedestal > _pedestal_min && gain > 0 ) {
        pe = (adc-pedestal)/gain;
      } else {
        pe = -10.0;
      }
      // int unique_chan  = _calibration[board][bank][channel_ro]["unique_chan"].asDouble();

      // Find tracker, station, plane, channel.
      int tracker, station, plane, channel;
      bool found = get_StatPlaneChannel(board, bank, channel_ro, tracker, station, plane, channel);

      // Exclude missing modules.
      if ( found ) { // pe > 1.0 &&
        SciFiDigit *digit = new SciFiDigit(spill, eventNo,
                                           tracker, station, plane, channel, pe, tdc);
        event->add_digit(digit);
      }
    }  // ends loop over channels (j)
    ReconEvent * revt = new ReconEvent();
    revt->SetSciFiEvent(new SciFiEvent(*event));
    spill.GetReconEvents()->push_back(revt);
  }  // ends loop over events (i)
}

bool RealDataDigitization::load_calibration(std::string file) {
  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
  std::string fname = std::string(pMAUS_ROOT_DIR)+"/src/map/MapCppTrackerDigits/"+file;

  std::ifstream inf(fname.c_str());

  if (!inf) {
    throw(Squeal(Squeal::recoverable,
          "Could not load Tracker Calibration.",
          "RealDataDigitization::load_calibration"));
  }

  std::string line;

  int numBoards;
  getline(inf, line);
  std::istringstream ist1(line.c_str());
  ist1 >> numBoards;

  read_in_all_Boards(inf);

  return true;
}

void RealDataDigitization::read_in_all_Boards(std::ifstream &inf) {
  std::string line;
  // Run over all boards.
  for ( int i = 0; i < _number_boards; ++i ) {
    // Run over banks.
    for ( int j = 0; j < _number_banks; ++j ) {
      // Run over channels.
      for ( int k = 0; k < _number_channels; ++k ) {
        int unique_chan_no, board, bank, chan;
        double ped, gain;

        getline(inf, line);
        std::istringstream ist1(line.c_str());
        ist1 >> unique_chan_no >> board >> bank >> chan >> ped >> gain;
        _calibration_pedestal[board][bank][chan] = ped;
        _calibration_gain[board][bank][chan]     = gain;
        _calibration_unique_chan_number[board][bank][chan] = unique_chan_no;
      }
    }
  }
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
                           int& tracker, int& station, int& plane, int& channel) const {
  bool found = false;
  tracker = station = plane = channel = -1;

  for ( unsigned int i = 0; !found && i < _board.size(); ++i ) {
    if ( (board == _board[i]) && (bank == _bank[i]) && (chan_ro == _chan_ro[i]) ) {
      station = _station[i];
      plane = _view[i];
      channel = _fibre[i];
      tracker = _tracker[i];
      found = true;
    }
  }
  return found;
}

bool RealDataDigitization::is_good_channel(const int board, const int bank,
                                           const int chan_ro) const {
  if ( board < _number_boards && bank < _number_banks && chan_ro < _number_channels ) {
    return _good_chan[board][bank][chan_ro];
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

  for ( int board = 0; board < _number_boards; ++board ) {
    for ( int bank = 0; bank < _number_banks; ++bank ) {
      for ( int chan_ro = 0; chan_ro < _number_channels; ++chan_ro ) {
        _good_chan[board][bank][chan_ro] = true;
      }
    }
  }

  int bad_board, bad_bank, bad_chan_ro;

  while ( !inf.eof() ) {
    inf >> bad_board >> bad_bank >> bad_chan_ro;
    _good_chan[bad_board][bad_bank][bad_chan_ro] = false;
  }
  return true;
}

} // ~namespace MAUS
