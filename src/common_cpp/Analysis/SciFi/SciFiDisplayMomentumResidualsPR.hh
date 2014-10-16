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

/** @class SciFiDisplayMomentumResidualsPR
 *
 *  Class to pattern recognition momentum residuals as ROOT histograms
 *
 */

#ifndef _SRC_COMMON_CPP_RECON_SCIFI_SCIFIDISPLAYMOMENTUMRESIDUALSPR_
#define _SRC_COMMON_CPP_RECON_SCIFI_SCIFIDISPLAYMOMENTUMRESDIUALSPR_

// ROOT headers
#include "TCanvas.h"
#include "TTree.h"
#include "TH1D.h"

// MAUS headers
#include "src/common_cpp/Analysis/SciFi/SciFiDataBase.hh"
#include "src/common_cpp/Analysis/SciFi/SciFiDataMomentumPR.hh"
#include "src/common_cpp/Analysis/SciFi/SciFiDisplayBase.hh"


namespace MAUS {

class SciFiDisplayMomentumResidualsPR : public SciFiDisplayBase {
  public:
    /** Default constructor */
    SciFiDisplayMomentumResidualsPR();

    /** Destructor */
    virtual ~SciFiDisplayMomentumResidualsPR();

    /** Plot the data currently held */
    virtual void Plot(TCanvas* aCanvas = NULL);

    /** Set up the SciFiData object and ROOT tree */
    virtual SciFiDataBase* SetUp();

    /** Update the internal data used to make the plots using the pointer to the SciFiData object */
    virtual void Update();

 private:

    /** Set up the ROOT TTree, call this after setting up the SciFiData member */
    bool SetUpRoot();

    /** Sets up the SciFiData object needed by the display.
     *  The display does not own the memory, but rather this should be called by
     *  the SciFiAnalysis class which then assumes ownership.
     */
    SciFiDataBase* SetUpSciFiData();

    TTree* mTree;                     /** The ROOT tree used to accumulate the reduced data */
    SciFiDataMomentumPR* mSpillData;  /** The reduced data object, covering one spill */
    MomentumDataPR mTrackData;       /** Struct containing reduced data for one track in a spill */

    // Residual histograms
    TH1D* mResidualPtT1;         /** Pt residual histogram for tracker 1 */
    TH1D* mResidualPzT1;         /** Pz residual histogram for tracker 1 */
    TH1D* mResidualPzT1Log;      /** Pz residual histogram for tracker 1 (log scale) */
    TH1D* mResidualPtT2;         /** Pt residual histogram for tracker 2 */
    TH1D* mResidualPzT2;         /** Pz residual histogram for tracker 2 */
    TH1D* mResidualPzT2Log;      /** Pz residual histogram for tracker 2 (log scale) */
};

} // ~namespace MAUS

#endif
