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

/** @class MapCppKLMCDigitizer 
 *
 */

#ifndef _COMPONENTS_MAP_MAPCPPKLMCDIGITIZER_H_
#define _COMPONENTS_MAP_MAPCPPKLMCDIGITIZER_H_

#include <assert.h>
#include <json/json.h>

#include <CLHEP/Random/RandPoisson.h>
#include <CLHEP/Random/RandGauss.h>
#include <CLHEP/Random/RandExponential.h>
#include <CLHEP/Units/PhysicalConstants.h>

#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

#include "TRandom.h"
#include "Config/MiceModule.hh"


namespace MAUS {

class MapCppKLMCDigitizer {
 public:
  bool birth(std::string argJsonConfigDocument);

  bool death();

  std::string process(std::string document);

  bool check_sanity_mc(std::string document);

  double calculate_nphe_at_pmt(double dist, double edep);

  std::vector<Json::Value> make_kl_digits(Json::Value hits);

  Json::Value fill_kl_evt(int evnum,
                           std::vector<Json::Value> _alldigits);

  bool check_param(Json::Value* hit1, Json::Value* hit2);

  // Calculates the time of the calorimeter trigger.
  double calcTriggerTime(Json::Value hits);

 private:
  MiceModule* geo_module;
  std::vector<const MiceModule*> kl_modules;

  std::string _classname;
  Json::Value _configJSON;
  Json::Value root;
  Json::Reader reader;
  Json::Value mc;
  Json::Value digit;

  double gentime;
  bool fDebug;
};
}

#endif  //  _COMPONENTS_MAP_MAPCPPKLMCDIGITIZER_H_
