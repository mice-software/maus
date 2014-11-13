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

/** @class SciFiDataMomentumKF
 *
 *  Class for producing SciFi kalman fit reduced momentum data
 *
 */

#ifndef _SRC_COMMON_CPP_RECON_SCIFI_SCIFIDATAKF_
#define _SRC_COMMON_CPP_RECON_SCIFI_SCIFIDATAKF_

// C++ headers
#include <vector>

// Root headers
#include "TTree.h"

// MAUS headers
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/Analysis/SciFi/SciFiDataMomentum.hh"


namespace MAUS {

struct DataKF {
  int TrackerNumber;
  int NDF;
  int Charge;
  double xMc;
  double xRec;
  double yMc;
  double yRec;
  double zMc;
  double zRec;
  double PxMc;
  double PxRec;
  double PyMc;
  double PyRec;
  double PtMc;
  double PtRec;
  double PzMc;
  double PzRec;
};

class SciFiDataKF : public SciFiDataMomentum {
  friend class SciFiDisplayPositionResidualsKF;
  friend class SciFiDisplayMomentumResidualsKF;
  friend class SciFiDisplayMomentumResolutionsKF;

  public:
    /** Default constructor */
    SciFiDataKF();

    /** Destructor */
    virtual ~SciFiDataKF();

    /** Set all the member variables to zero */
    void Clear();

    /** Calculate the reduced position andmomentum data for an event at the
     *  tracker reference surface (station 1, plane 0) */
    virtual void ReduceData(MCEvent *aMcEvent, SciFiEvent* aSFEvent);

  protected:
    std::vector<DataKF> mDataKF;  /** Vector holding reduced data for one track */
};
} // ~namespace MAUS

#endif
