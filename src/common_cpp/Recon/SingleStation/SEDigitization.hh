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

/** @class SEDigitization
 *  D...
 *
 */

#ifndef SEDigitization_HH
#define SEDigitization_HH
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

#include "src/common_cpp/Recon/SingleStation/SEDigit.hh"
#include "src/common_cpp/Recon/SingleStation/SEEvent.hh"
#include "src/common_cpp/Recon/SingleStation/SESpill.hh"

// namespace MAUS {

class SEDigitization {
 public:
  SEDigitization();

  ~SEDigitization();

  /** @brief Processes a spill from DAQ
   *  @params spill A SESpill to be filled
   *  @params input_event The DAQ JSON Tracker Event
   */
  void process(SESpill &spill, Json::Value const &input_event);

  /** @brief Reads in the calibration.
   */
  bool load_calibration(std::string filename);

  /** @brief Saves calibration to vectors.
   */
  void read_in_all_Boards(std::ifstream &inf);

  /** @brief Loads the mapping.
   */
  bool load_mapping(std::string file);

  /** @brief Converts read-out map into SE Fibre map
   */
  void get_StatPlaneChannel(int &board, int &bank, int &chan_ro,
                            int &plane, int &channel);

  /** @brief Reads the bad channel list from file.
   */
  bool load_bad_channels();

  /** @brief Returns value depends on the goodness of the channel.
   */
  bool is_good_channel(int bank, int chan_ro);

 private:
  /// A vector containing calibration values for every 4 banks of the 16 boards.
  std::vector<Json::Value> _calibration[2][4];

  /// This is a vector storing the goodness of each channel.
  bool good_chan[2][4][128];

  // std::vector<int> _cryo;
  // std::vector<int> _cass;
  std::vector<int> _board;
  std::vector<int> _bank;
  std::vector<int> _chan_ro;
  std::vector<int> _view;
  std::vector<int> _fibre;
  std::vector<int> _extWG;
  std::vector<int> _inWG;
  std::vector<int> _WGfib;
};  // Don't forget this trailing colon!!!!
// } // ~namespace MAUS

#endif
