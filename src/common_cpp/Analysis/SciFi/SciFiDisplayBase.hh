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

/** @class SciFiDisplayBase
 *
 *  Abstract base class for other SciFi reduced data classes
 *
 */

#ifndef _SRC_COMMON_CPP_RECON_SCIFI_SCIFIDISPLAYBASE_
#define _SRC_COMMON_CPP_RECON_SCIFI_SCIFIDISPLAYBASE_

// ROOT headers
#include "TCanvas.h"

// MAUS headers
#include "src/common_cpp/Analysis/SciFi/SciFiDataBase.hh"


namespace MAUS {

class SciFiDisplayBase  {
  public:
    /** Default constructor */
    SciFiDisplayBase();

    /** Destructor */
    virtual ~SciFiDisplayBase();

    /** Return the member Canvas */
    TCanvas* GetCanvas() { return mCanvas; }

    /** Plot the data on a ROOT TCanvas */
    virtual void Plot(TCanvas* aCanvas = NULL) = 0;

    /** Save the data and plots (e.g. as pdfs, to ROOT file, ...) */
    virtual void Save() = 0;

    /** Sets up the SciFiData object needed by the display, and any other objects (e.g. ROOT Trees).
     *  The display does not own the SciFiData object, but rather this should be called by
     *  the SciFiAnalysis class which then assumes ownership.
     */
    virtual SciFiDataBase* SetUp() = 0;

    /** Update the internal data used to make the plots using the pointer to the SciFiData object.
     *  Data may accumulate inside the derived class (e.g. in a ROOT TTree) for plotting
     *  cumulative data at the end of a run, or simply replace the existing data to allow plotting
     *  spill by spill.
     */
    virtual void Fill() = 0;

  protected:
    TCanvas* mCanvas; /** The ROOT TCanvas on which to plot the data */
};

} // ~namespace MAUS

#endif
