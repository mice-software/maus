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

    /** Destructor. Note, the mSpillData pointer memory is not owned by the display class,
     *  and so is not removed by the destructor  */
    virtual ~SciFiDisplayBase();

    /** Return the flag which sets whether only tracks which produced spacepoints in every
     *  station should be used when producing the plots.  The ROOT tree will continue to be
     *  filled with all the data.
     */
    bool Get5StationOnly() { return m5StationOnly; }

    /** Return the member Canvas */
    TCanvas* GetCanvas() { return mCanvas; }

    /** Returns a derived class pointer to the reduced data object associated with the display */
    virtual SciFiDataBase* GetData() = 0;

    /** Create a new SciFiData object of the correct derived type */
    virtual SciFiDataBase* MakeDataObject() = 0;

    /** Plot the data on a ROOT TCanvas */
    virtual void Plot(TCanvas* aCanvas = NULL) = 0;

    /** Save the data and plots (e.g. as pdfs, to ROOT file, ...) */
    virtual void Save() = 0;

    /** Set the flag which sets whether only tracks which produced spacepoints in every
     *  station should be used when producing the plots.  The ROOT tree will continue to be
     *  filled with all the data.
     */
    void Set5StationOnly(bool a5StationOnly) { m5StationOnly = a5StationOnly; }

    /** Set the Canvas pointer, up to user to delete previous Canvas if needed */
    void SetCanvas(TCanvas* aCanvas) { mCanvas = aCanvas; }

    /** Set the SciFi object */
    virtual SciFiDataBase* SetData(SciFiDataBase* data) = 0;

    /** Set up the display class */
    virtual SciFiDataBase* SetUp() = 0;

    /** Ensures the SciFiData member is set up.  If the member is all ready set up it changes
     *  nothing, if it is not, then MakeDataObject gets called, setting up a new data object for
     *  the member.
     */
    SciFiDataBase* SetUpData();

    /** Update the internal data used to make the plots using the pointer to the SciFiData object.
     *  Data may accumulate inside the derived class (e.g. in a ROOT TTree) for plotting
     *  cumulative data at the end of a run, or simply replace the existing data to allow plotting
     *  spill by spill.
     */
    virtual void Fill() = 0;

  protected:
    bool m5StationOnly;    /** Only use 5 spacepoint tracks when producing plots */
    /** The reduced data object, covering 1 spill. Memory is NOT owned by the display class! */
    SciFiDataBase* mSpillData;
    TCanvas* mCanvas;           /** The ROOT TCanvas on which to plot the data */
};

} // ~namespace MAUS

#endif
