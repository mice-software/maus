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
#include "DataStructure/Primary.hh"
#include "Utils/TOFCalibrationMap.hh"
#include "API/MapBase.hh"

namespace MAUS {

/** @brief structure to hold -a temporary digit-
 */
typedef struct fTofDig {
    TOFChannelId* fChannelId;
    int fStation;
    int fPlane;
    int fSlab;
    int fPmt;
    double fTime;
    ThreeVector fMom;
    Hep3Vector fPos;
    double fEdep;
    bool fIsUsed;
    int fNpe;
    std::string fTofKey;
    int fLeadingTime;
    int fTrailingTime;
    int fRawTime;
    int fTriggerRequestLeadingTime;
    int fTriggerTimeTag;
    int fTimeStamp;
} tmpThisDigit;

/** @brief vector storage for -all temporary digits
 */
typedef std::vector<fTofDig> TofTmpDigits;

class MapCppTOFMCDigitizer : public MapBase<Data> {
 public:
  MapCppTOFMCDigitizer();

 private:
  typedef std::vector<TOF0DigitArray*> fDigsArrayPtr;

  /** @brief birth initialize and read data cards
   */
  void _birth(const std::string& argJsonConfigDocument);

  /** @brief death does nothing
   */
  void _death();

  /** @brief processes MC TOF hits */
  void _process(MAUS::Data* data) const;

  /** @brief digitizes tof hits
   * finds the tof slab corresponding to the hit
   * propagates hit to PMTs at either end of slab
   * digitizes the hit time
   * the digitized hits are stored in a temporary array
   * @param TofTmpDigits is the temporary vector storage for all digits
   * @param the vector is of type struct tmpThisDigit 
   */
  TofTmpDigits make_tof_digits(TOFHitArray* hits,
                               double gentime) const;

  /** @brief converts energy deposit to photoelectrons
   * energy deposit is converted to photoelectrons
   * the npe yield is poisson-smeared
   * then attenuated
   * then an efficiency is applied to yield the final npe
   */
  double get_npe(double dist, double edep) const;

  /** @brief sets the pixel that would have triggered this hit
   */
  std::string findTriggerPixel(TofTmpDigits& tmpDigits) const;


  /** @brief checks for multiple hits in the same slab
   * there is only digit created per slab, 
   * photoelectrons from hits in the same slab are added up
   * the time of the earliest hit is taken as the slab digit time
   */
  bool check_param(tmpThisDigit& hit1, tmpThisDigit& hit2) const;

  /** @brief sets the digit for each tof station
   * goes through all the temporary digits
   * weed out multiple hits
   * set the digit data structure corresponding to the station hit
   * @param TofTmpDigits contains all the digits
   * @param fDigsArrayPtr: vector of pointers to TOF0/1/2 digit datastructure
   */
  void fill_tof_evt(int evnum, int snum,
                           TofTmpDigits& tmpDigits,
                           fDigsArrayPtr& digitArrayPtr,
                           std::string strig) const;

 private:
  MiceModule* geo_module;
  std::vector<const MiceModule*> tof_modules;

  Json::Value _configJSON;

  TOFCalibrationMap _map;

  bool fDebug;
};
}

#endif  //  _COMPONENTS_MAP_MAPCPPTOFMCDIGITIZER_H_
