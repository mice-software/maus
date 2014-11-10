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

/** @class SciFiDataMomentumPR
 *
 *  Class for producing SciFi pattern recognition reduced momentum data
 *
 */

#ifndef _SRC_COMMON_CPP_RECON_SCIFI_SCIFIDATAMOMENTUMPR_
#define _SRC_COMMON_CPP_RECON_SCIFI_SCIFIDATAMOMENTUMPR_

// C++ headers
#include <vector>

// Root headers
#include "TTree.h"

// MAUS headers
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/Analysis/SciFi/SciFiDataMomentum.hh"


namespace MAUS {

struct MomentumDataPR {
  int TrackerNumber;
  int NumberOfPoints;
  int Charge;
  double PtMc;
  double PzMc;
  double PtRec;
  double PzRec;
};

class SciFiDataMomentumPR : public SciFiDataMomentum {
  friend class SciFiDisplayMomentumResidualsPR;
  friend class SciFiDisplayMomentumResolutionsPR;

  public:
    /** Default constructor */
    SciFiDataMomentumPR();

    /** Destructor */
    virtual ~SciFiDataMomentumPR();

    /** Get the flag determining if secondaries take part in the analyses */
    bool GetUseSecondaries() { return mUseSecondaries; }

    /** Set all the member variables to zero */
    void Clear();

    /** Calculate the reduced momentum data for an event */
    virtual void ReduceData(MCEvent *aMcEvent, SciFiEvent* aSFEvent);

    /** Get the flag determining if secondaries take part in the analyses */
    void SetUseSecondaries(bool aUseSecondaries) { mUseSecondaries = aUseSecondaries; }

  protected:
    bool mUseSecondaries;   /** Do we want to use secondaries in the analysis (default yes) */
    int mNUnmatchedTracks;  /** No. of tracks which could not associate with MC track */
    int mNMatchedTracks;    /** No. of recon tracks matched to an MC track */
    std::vector<MomentumDataPR> mDataPR;  /** Vector holding reduced data for one track */
};
} // ~namespace MAUS

#endif
