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

#include "API/MapBase.hh"

namespace MAUS {

class MapCppKLMCDigitizer : public MapBase<Json::Value> {
 public:
  MapCppKLMCDigitizer();

 private:
  void _birth(const std::string& argJsonConfigDocument);

  void _death();

  void _process(Json::Value* document) const;

  double calculate_nphe_at_pmt(double dist, double edep) const;

  Json::Value check_sanity_mc(const Json::Value& document) const;

  std::vector<Json::Value> make_kl_digits(Json::Value hits, Json::Value& root) const;

  Json::Value fill_kl_evt(int evnum,
                           std::vector<Json::Value> _alldigits) const;

  bool check_param(Json::Value* hit1, Json::Value* hit2) const;

  // Calculates the time of the calorimeter trigger.
  double calcTriggerTime(Json::Value hits) const;

 private:
  MiceModule* geo_module;
  std::vector<const MiceModule*> kl_modules;

  Json::Value _configJSON;

  bool fDebug;
};
}

#endif  //  _COMPONENTS_MAP_MAPCPPKLMCDIGITIZER_H_
