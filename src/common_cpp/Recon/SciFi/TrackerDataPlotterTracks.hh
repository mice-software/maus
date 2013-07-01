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

/** @class TrackerDataPlotterTracks
 *
 *  Plots the xy, zx, zy projections of tracks
 *
 */

#ifndef _SRC_COMMON_CPP_RECON_SCIFI_TRACKERDATAPLOTTERTRACKS_
#define _SRC_COMMON_CPP_RECON_SCIFI_TRACKERDATAPLOTTERTRACKS_

// ROOT headers
#include "TCanvas.h"

// MAUS headers
#include "src/common_cpp/Recon/SciFi/TrackerData.hh"
#include "src/common_cpp/Recon/SciFi/TrackerDataPlotterBase.hh"


namespace MAUS {

class TrackerDataPlotterTracks : public TrackerDataPlotterBase {
  public:

    /** Default constructor */
    TrackerDataPlotterTracks();

    /** Destrcutor */
    virtual ~TrackerDataPlotterTracks();

    /** Overloaded brackets operator, takes in the data, does all the work */
    TCanvas* operator() ( TrackerData &t1, TrackerData &t2, TCanvas* aCanvas = NULL );
};

} // ~namespace MAUS

#endif

