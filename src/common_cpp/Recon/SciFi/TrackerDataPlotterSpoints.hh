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

/** @class TrackerDataPlotterSpoints
 *
 *  Plots the tracker spacepoints in xy, zx, yz for both trackers, using ROOT
 *
 */

#ifndef _SRC_COMMON_CPP_RECON_SCIFI_TRACKERDATAPLOTTERSPOINTS_
#define _SRC_COMMON_CPP_RECON_SCIFI_TRACKERDATAPLOTTERSPOINTS_

// ROOT headers
#include "TCanvas.h"
#include "TGraph.h"

// MAUS headers
#include "src/common_cpp/Recon/SciFi/TrackerData.hh"
#include "src/common_cpp/Recon/SciFi/TrackerDataPlotterBase.hh"


namespace MAUS {

class TrackerDataPlotterSpoints : public TrackerDataPlotterBase {
  public:

    /** Default constructor, initialise the abstract base class, and set pointers to NULL. */
    TrackerDataPlotterSpoints();

    /** Destructor. Delete graph objects (member canvas is deleted by base class). */
    virtual ~TrackerDataPlotterSpoints();

    /** Overloaded brackets operator, takes in the data, does all the work */
    TCanvas* operator() ( TrackerData &t1, TrackerData &t2, TCanvas* aCanvas = NULL);

  private:
    TGraph *_gr_xy1;
    TGraph *_gr_zx1;
    TGraph *_gr_zy1;
    TGraph *_gr_xy2;
    TGraph *_gr_zx2;
    TGraph *_gr_zy2;
};

} // ~namespace MAUS

#endif
