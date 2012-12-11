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

/** @class RealDataDigitization 
 *  D...
 *
 */

#ifndef REALDATADIGITIZATION_HH
#define REALDATADIGITIZATION_HH
// C headers
#include <assert.h>
#include <json/json.h>
#include <CLHEP/Random/RandPoisson.h>
#include <CLHEP/Random/RandGauss.h>
#include <CLHEP/Random/RandExponential.h>
#include <CLHEP/Units/PhysicalConstants.h>

// C++ headers
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cmath>

// G4MICE from commonCpp
#include "Interface/dataCards.hh"
#include "Interface/MICEEvent.hh"

// from old file
#include "Config/MiceModule.hh"

#include "src/common_cpp/DataStructure/SciFiDigit.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/DataStructure/Spill.hh"

namespace MAUS {

class RealDataDigitization {
 public:
  typedef std::vector<int> IntChannelArray;

  RealDataDigitization();

  ~RealDataDigitization();

  /** @brief Processes a spill from DAQ
   *  @params spill A SciFiSpill to be filled
   *  @params input_event The DAQ JSON Tracker Event
   */
  void process(Spill &spill, Json::Value const &input_event);

  /** @brief Reads in the calibration.
   */
  bool load_calibration(std::string filename);

  /** @brief Saves calibration to vectors.
   */
  void read_in_all_Boards(std::ifstream &inf);

  /** @brief Loads the mapping.
   */
  bool load_mapping(std::string file);

  /** @brief Converts read-out map into SciFi Fibre map
   */
  bool get_StatPlaneChannel(int &board, int &bank, int &chan_ro,
                            int &tracker, int &station, int &plane, int &channel) const;

  /** @brief Reads the bad channel list from file.
   */
  bool load_bad_channels();

  /** @brief Returns value depends on the goodness of the channel.
   */
  bool is_good_channel(int board, int bank, int chan_ro) const;

  int get_calibration_unique_chan_numb(int board, int bank, int chan) const {
                                       return _calibration_unique_chan_number[board][bank][chan]; }

 private:
  static const int _number_channels       = 128;
  static const int _number_banks          = 4;
  static const int _number_boards         = 16;
  static const int _total_number_channels = 6403;
  static const double _pedestal_min       = 0.000000001;

  /// Arrays containing calibration values for every channel in the 4 banks of the 16 boards.
  double _calibration_pedestal[_number_boards][_number_banks][_number_channels];
  double _calibration_gain[_number_boards][_number_banks][_number_channels];
  int    _calibration_unique_chan_number[_number_boards][_number_banks][_number_channels];
  /// This is an array storing the goodness of each channel.
  bool _good_chan[_number_boards][_number_banks][_number_channels];

  /// This is for the mapping storage.
  IntChannelArray _board;
  IntChannelArray _bank;
  IntChannelArray _chan_ro;
  IntChannelArray _tracker;
  IntChannelArray _station;
  IntChannelArray _view;
  IntChannelArray _fibre;
  IntChannelArray _extWG;
  IntChannelArray _inWG;
  IntChannelArray _WGfib;
};  // Don't forget this trailing colon!!!!

} // ~namespace MAUS

#endif
