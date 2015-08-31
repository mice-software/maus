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
#include "DataStructure/Spill.hh"
#include "DataStructure/Primary.hh"
#include "Config/MiceModule.hh"
#include "API/MapBase.hh"

namespace MAUS {
  /** @brief structure to hold -a temporary digit-
   */
  typedef struct fCkovTmp {
    CkovChannelId* fChannelId;
    int fStation;
    ThreeVector fMom;
    ThreeVector fPos;
    double fTime;
    double fEdep;
    bool fIsUsed;
    double fNpe;
    double fNpe0, fNpe1, fNpe2, fNpe3;
    double fNpe4, fNpe5, fNpe6, fNpe7;
    double fLeadingTime0, fLeadingTime1, fLeadingTime2, fLeadingTime3;
    double fRawTime0, fRawTime1, fRawTime2, fRawTime3;
  } aTmpCkovDigit;

  /** @brief vector storage for -all temporary digits
   */
  typedef std::vector<fCkovTmp> CkovTmpDigits;


  class MapCppCkovMCDigitizer : public MapBase<Data> {

    public:
      MapCppCkovMCDigitizer();


    private:

      void _birth(const std::string& argJsonConfigDocument);

      void _death();

      void _process(Data* data) const;

      /* return the number of photoelectrons for this mc hit
       * argument is a CkovHit
       */
      double get_npe(CkovHit& hit) const;

      /* go through the ckov mc hits 
       * and store the digitized hits in a temp vector
       */
      CkovTmpDigits make_ckov_digits(CkovHitArray* hits, double gentime) const;

      /* go through all the digitized hits in the temporary vector
       * and set the actual CkovA, CkovB hits and the CkovDigit for the event
       */
      void fill_ckov_evt(int evnum,
                         CkovTmpDigits& tmpAllDigits,
                         CkovDigitArray* CkovDigArray) const;

   private:
      MiceModule* geo_module;

      std::vector<const MiceModule*> ckov_modules;

      Json::Value _configJSON;

      /* debug flag, set to true for verbose prints */
      bool fDebug;

      TRandom3* rnd;

      // the muon threshold and the adc/npe conversion for each counter
      // these are initialized in birth
      double muon_thrhold[2];
      double charge_per_pe[2];

      // scaling factor to data
      double scaling;

      // #npe to adc conversion - maybe put it in a datacard
      double ckovNpeToAdc;
  };
}

#endif  //  _COMPONENTS_MAP_MAPCPPCkovMCDIGITIZER_H_
