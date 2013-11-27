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

/** @class TrackerDataPlotterXYZ
 *
 *  Calls the spacepoints and tracks plotting classes, drawing both on the same canvas,
 *  to give an full set of XYZ views
 *
 */

#ifndef _SRC_COMMON_CPP_RECON_SCIFI_TRACKERDATAPLOTTERXYZ_
#define _SRC_COMMON_CPP_RECON_SCIFI_TRACKERDATAPLOTTERXYZ_

// ROOT headers
#include "TCanvas.h"
#include "TGraph.h"

// MAUS headers
#include "src/common_cpp/Plotting/SciFi/TrackerData.hh"
#include "src/common_cpp/Plotting/SciFi/TrackerDataPlotterBase.hh"
#include "src/common_cpp/Plotting/SciFi/TrackerDataPlotterSpoints.hh"
#include "src/common_cpp/Plotting/SciFi/TrackerDataPlotterTracks.hh"


namespace MAUS {

class TrackerDataPlotterXYZ : public TrackerDataPlotterBase {
  public:

    /** Default constructor, initialise the abstract base class, and set pointers to NULL. */
    TrackerDataPlotterXYZ();

    /** Destructor. Delete graph objects (member canvas is deleted by base class). */
    virtual ~TrackerDataPlotterXYZ();

    /** Overloaded brackets operator, takes in the data, does all the work */
    TCanvas* operator() ( TrackerData &t1, TrackerData &t2, TCanvas* aCanvas = NULL);

  protected:
    bool _boolSave;
    TrackerDataPlotterSpoints *_spointsPlotter;
    TrackerDataPlotterTracks *_trksPlotter;
};

} // ~namespace MAUS

#endif
