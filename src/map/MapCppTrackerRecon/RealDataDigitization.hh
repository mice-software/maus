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

#include "src/common_cpp/Recon/SciFiDigit.hh"
#include "src/common_cpp/Recon/TrackerEvent.hh"
#include "src/common_cpp/Recon/TrackerSpill.hh"


class RealDataDigitization {
 public:
  RealDataDigitization();

  void construct(TrackerSpill &spill, Json::Value input_event);

  bool load_calibration(std::string filename);

  void read_in_all_Boards(std::ifstream& inf);

  bool load_mapping(std::string file);

  void get_StatPlaneChannel(int& , int& , int& , int& , int& , int&, int&);

  bool load_bad_channels();
  bool is_good_channel(int, int, int);

 private:
  std::vector<Json::Value> _calibration[16][4];
  bool good_chan[16][4][128];

  int temp;

//  std::vector<int> _cryo;
//  std::vector<int> _cass;
  std::vector<int> _board;
  std::vector<int> _bank;
  std::vector<int> _chan_ro;
  std::vector<int> _tracker;
  std::vector<int> _station;
  std::vector<int> _view;
  std::vector<int> _fibre;
  std::vector<int> _extWG;
  std::vector<int> _inWG;
  std::vector<int> _WGfib;
};  // Don't forget this trailing colon!!!!

#endif
