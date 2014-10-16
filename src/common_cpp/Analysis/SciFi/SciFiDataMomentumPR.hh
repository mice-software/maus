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

// Root headers
#include "TTree.h"

// MAUS headers
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/Analysis/SciFi/SciFiDataBase.hh"


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

class SciFiDataMomentumPR : public SciFiDataBase {
  friend class SciFiDisplayMomentumResidualsPR;

  public:
    /** Default constructor */
    SciFiDataMomentumPR();

    /** Destructor */
    virtual ~SciFiDataMomentumPR();

    /** Set all the member variables to zero */
    void Clear();

    /** Process data from this spill*/
    virtual bool Process(Spill* spill);

    /** Calculate the reduced momentum data for an event */
    void ReduceData(MCEvent *aMcEvent, SciFiEvent* aSFEvent);

  protected:
    int mNBadTracks;
    int mNMatchedTracks;                /** Number of recon tracks matched to an MC track */
    int mNMismatchedTracks;
    int mNMissedTracks;                 /** Number of tracks missed by the reconstruction */
    std::vector<MomentumDataPR> mData;  /** Vector whose elements hold reduced data for one track */

};

} // ~namespace MAUS

#endif
