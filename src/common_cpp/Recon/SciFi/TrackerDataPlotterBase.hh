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

/** @class TrackerDataPlotterBase
 *
 *  Abstract base class for various classes which use ROOT to plot tracker data.
 *
 */

#ifndef _SRC_COMMON_CPP_RECON_SCIFI_TRACKERDATAPLOTTERBASE_
#define _SRC_COMMON_CPP_RECON_SCIFI_TRACKERDATAPLOTTERBASE_

// C++ headers
#include <string>

// ROOT headers
#include "TCanvas.h"

// MAUS headers
#include "src/common_cpp/Recon/SciFi/TrackerData.hh"

namespace MAUS {

class TrackerDataPlotterBase {
  public:

    /** Default constructor */
    TrackerDataPlotterBase();

    /** Destructor */
    virtual ~TrackerDataPlotterBase();

    /** Virtual void overloaded brackets operator, used to set the data & optionally the canvas */
    virtual TCanvas* operator() ( TrackerData &t1, TrackerData &t2, TCanvas* aCanvas = NULL ) = 0;

    void SetSaveOutput(bool SaveOutput) { _SaveOutput = SaveOutput; }
    bool GetSaveOutput() { return _SaveOutput; }

    void SetOutputName(std::string OutputName) { _OutputName = OutputName; }
    std::string GetOutputName() { return _OutputName; }

    /** Return the member Canvas */
    TCanvas* get_canvas() { return _Canvas; }

  protected:
    bool _SaveOutput;
    std::string _OutputName;
    TCanvas* _Canvas;
};

} // ~namespace MAUS

#endif
