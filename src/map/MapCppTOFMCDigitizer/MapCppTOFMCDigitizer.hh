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

/** @class MapCppTOFMCDigitizer 
 *
 */

#ifndef _COMPONENTS_MAP_MAPCPPTOFMCDIGITIZER_H_
#define _COMPONENTS_MAP_MAPCPPTOFMCDIGITIZER_H_

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
#include "Utils/TOFCalibrationMap.hh"
#include "API/MapBase.hh"

namespace MAUS {

class MapCppTOFMCDigitizer : public MapBase<Json::Value> {
 public:
  MapCppTOFMCDigitizer();

 private:
  void _birth(const std::string& argJsonConfigDocument);

  void _death();

  void _process(Json::Value* data) const;

  Json::Value check_sanity_mc(Json::Value& document) const;

  double get_npe(double dist, double edep) const;

  std::vector<Json::Value> make_tof_digits(Json::Value hits,
                                           double gentime,
                                           Json::Value& root) const;

  std::string findTriggerPixel(std::vector<Json::Value> _alldigits) const;
  Json::Value fill_tof_evt(int evnum, int snum,
                           std::vector<Json::Value> _alldigits,
                           std::string strig) const;

  bool check_param(Json::Value* hit1, Json::Value* hit2) const;

 private:
  MiceModule* geo_module;
  std::vector<const MiceModule*> tof_modules;

  Json::Value _configJSON;

  TOFCalibrationMap _map;
  std::map<int, std::string> _triggerhit_pixels;

  std::vector<std::string> _stationKeys;

  bool fDebug;
};
}

#endif  //  _COMPONENTS_MAP_MAPCPPTOFMCDIGITIZER_H_
