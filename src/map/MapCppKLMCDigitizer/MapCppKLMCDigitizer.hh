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

/** @brief structure to hold -a temporary digit-
 */
typedef struct fKLDig {
    KLChannelId* fChannelId;
    std::string fKLKey;
    int fCell;
    int fPmt;
    double fTime;
    ThreeVector fMom;
    Hep3Vector fPos;
    double fEdep;
    bool fIsUsed;
    int fNpe;
} tmpThisDigit;

/** @brief vector storage for -all temporary digits
 */
typedef std::vector<fKLDig> KLTmpDigits;

class MapCppKLMCDigitizer : public MapBase<Data> {
 public:
  MapCppKLMCDigitizer();

 private:
  void _birth(const std::string& argJsonConfigDocument);

  void _death();

  /** @brief processes MC KL hits */
  void _process(MAUS::Data* data) const;

  int calculate_nphe_at_pmt(double dist, double edep) const;

  /** @brief digitizes kl hits
   * finds the kl slab corresponding to the hit
   * propagates hit to PMTs at either end of slab
   * digitizes the hit time
   * the digitized hits are stored in a temporary array
   * @param KLTmpDigits is the temporary vector storage for all digits
   * @param the vector is of type struct tmpThisDigit 
   */
  KLTmpDigits make_kl_digits(KLHitArray* hits) const;

  /** @brief checks for multiple hits and adds up npe yields
   */
  bool check_param(tmpThisDigit& hit1, tmpThisDigit& hit2) const;

  /** @brief sets the kl digit
   * goes through all the temporary digits
   * weed out multiple hits
   * set the digit data structure corresponding to the cell hit
   * @param KLTmpDigits contains all the digits
   */
  void fill_kl_evt(int evnum,
                   KLTmpDigits& tmpDigits,
                   KLDigitArray* digitArrayPtr) const;

 private:
  MiceModule* geo_module;
  std::vector<const MiceModule*> kl_modules;

  Json::Value _configJSON;

  bool fDebug;
};
}

#endif  //  _COMPONENTS_MAP_MAPCPPKLMCDIGITIZER_H_
