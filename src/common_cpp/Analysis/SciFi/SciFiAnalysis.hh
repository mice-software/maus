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

/** @class SciFiAnalysis
 *
 *  Class for use in analysing scifi data
 *
 */

#ifndef _SRC_COMMON_CPP_RECON_SCIFI_SCIFIANALYSIS_
#define _SRC_COMMON_CPP_RECON_SCIFI_SCIFIANALYSIS_

// C++ headers
#include <vector>

// ROOT headers
#include "TCanvas.h"

// MAUS headers
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/Analysis/SciFi/SciFiDataBase.hh"
#include "src/common_cpp/Analysis/SciFi/SciFiDisplayBase.hh"


namespace MAUS {

class SciFiAnalysis {
  public:

    /** Default constructor */
    SciFiAnalysis();

    /** Destructor */
    virtual ~SciFiAnalysis();

    /** Add a data module */
    void AddSciFiData(SciFiDataBase* aData) { mSciFiData.push_back(aData); }

    /** Add a data display module */
    void AddDisplay(SciFiDisplayBase* aDisplay) { mDisplays.push_back(aDisplay); }

    /** Get the SciFiData member */
    std::vector<SciFiDataBase*> GetSciFiData() { return mSciFiData; }

    /** Produce the plots */
    void Plot();

    /** Takes in the data, one spill at a time */
    bool Process(Spill* aSpill);

    /** Save the data to a ROOT File */
    void Save();

    /** Setup the member variables by initialising the displays,
     *  and setting the SciFiData pointer */
    void SetUpDisplays();

    /** Set the SciFiData, which particular derived class depends on what analysis is required */
    void SetSciFiData(std::vector<SciFiDataBase*> aSciFiData) { mSciFiData = aSciFiData; }

  private:
    std::vector<SciFiDataBase*> mSciFiData;    /** The SciFiData objects */
    std::vector<SciFiDisplayBase*> mDisplays;  /** Vector of the different data displays */
};

} // ~namespace MAUS

#endif
