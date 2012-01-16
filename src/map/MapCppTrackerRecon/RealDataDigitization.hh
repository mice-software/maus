/** @class RealDataDigitization 
 *  D...
 *
 */

#ifndef REALDATADIGITIZATION_HH
#define REALDATADIGITIZATION_HH
// C headers
#include <assert.h>
#include <json/json.h>

// C++ headers  
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <cstdio>
#include <cstdlib>

// G4MICE from commonCpp
#include "Interface/dataCards.hh"
#include "Interface/MICEEvent.hh"

// from old file
#include "Config/MiceModule.hh"
#include <CLHEP/Random/RandPoisson.h>
#include <CLHEP/Random/RandGauss.h>
#include <CLHEP/Random/RandExponential.h>
#include <CLHEP/Units/PhysicalConstants.h>
#include <cmath>


#include "src/common_cpp/Recon/SciFiDigit.hh"
#include "src/common_cpp/Recon/TrackerEvent.hh"
#include "src/common_cpp/Recon/TrackerSpill.hh"


class RealDataDigitization {
 public:
  RealDataDigitization(TrackerSpill &spill, Json::Value input_event);

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
