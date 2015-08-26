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

/** @class MapCppCkovMCDigitizer 
 *
 */

#ifndef _COMPONENTS_MAP_MAPCPPCKOVMCDIGITIZER_H_
#define _COMPONENTS_MAP_MAPCPPCKOVMCDIGITIZER_H_

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

#include "TRandom3.h"
#include "Config/MiceModule.hh"
#include "API/MapBase.hh"

namespace MAUS {

  class MapCppCkovMCDigitizer : public MapBase<Json::Value> 
  {
    
    public:
      MapCppCkovMCDigitizer();
  
    
    private:
    
      void _birth(const std::string& argJsonConfigDocument);
  
      void _death();
  
      void _process(Json::Value* data) const;
  
      Json::Value check_sanity_mc(Json::Value& document) const;
  
      double get_npe(Json::Value hit) const;
  
      std::vector<Json::Value> make_ckov_digits(Json::Value hits,
                                                double gentime,
                                                Json::Value& root) const;
  
      Json::Value fill_ckov_evt(int evnum, int snum,
                                std::vector<Json::Value> _alldigits,
                                int i) const;
  
      bool check_param(Json::Value* hit1, Json::Value* hit2) const;
 
   private:
      MiceModule* geo_module;
    
      std::vector<const MiceModule*> ckov_modules;
  
      Json::Value _configJSON;
  
      std::vector<std::string> _station_index;
  
      bool fDebug ;

      TRandom3* rnd;
  };
}

#endif  //  _COMPONENTS_MAP_MAPCPPCkovMCDIGITIZER_H_
