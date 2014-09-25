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

/** @class SciFiDataMomentum
 *
 *  Class for SciFi reduced momentum data
 *
 */

#ifndef _SRC_COMMON_CPP_RECON_SCIFI_SCIFIDATAMOMENTUM_
#define _SRC_COMMON_CPP_RECON_SCIFI_SCIFIDATAMOMENTUM_

// Root headers
#include "TTree.h"

// MAUS headers
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/Analysis/SciFi/SciFiDataBase.hh"


namespace MAUS {

struct MomentumData {
  int SpillNumber;
  int TrackerNumber;
  int NumberOfPoints;
  int Charge;
  double PtMc;
  double PzMc;
  double PtRec;
  double PzRec;
};

class SciFiDataMomentum : public SciFiDataBase {
  friend class SciFiDisplayMomentumResiduals;

  public:

    /** Default constructor */
    SciFiDataMomentum();

    /** Destructor */
    virtual ~SciFiDataMomentum();

    /** Accuulate data from this spill*/
    virtual bool Accumulate(Spill* spill);

    /** Calculate the reduced momentum data for this spill */
    void ReduceData(MCEvent *aMcEvent, SciFiEvent* aSFEvent);

  protected:
    int mMcTrackId;
    int mNBadTracks;
    int mNMatchedTracks;
    int mNMismatchedTracks;
    int mNMissedTracks;
    MomentumData mDataStruct;
    TTree* mTree;
};

} // ~namespace MAUS

#endif
