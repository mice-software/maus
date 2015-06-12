/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_CANVASWRAPPER_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_CANVASWRAPPER_HH_

#include <string>

#include "src/common_cpp/Utils/VersionNumber.hh"

class TCanvas;
class TRootEmbeddedCanvas;

namespace MAUS {
class CanvasWrapper {
  public:
    /** Default constructor initialises everything to NULL */
    CanvasWrapper();

    /** Copy constructor (deepcopy) */
    CanvasWrapper(const CanvasWrapper& data);

    /** Deepcopy from data to *this */
    CanvasWrapper& operator=(const CanvasWrapper& data);

    /** Deletes the canvas */
    virtual ~CanvasWrapper();

    /** Set the canvas description */
    void SetDescription(std::string description) {_description = description;}

    /** Get the canvas description */
    std::string GetDescription() const {return _description;}

    /** Set the ROOT canvas
     *
     *  Image takes ownership of memory allocated to the TCanvas object
     */
    void SetCanvas(TCanvas* canvas);

    /** Get the ROOT canvas
     *
     *  Image still owns memory allocated to the TCanvas object
     */
    TCanvas* GetCanvas() {return _canvas;}

    /** Embed the ROOT canvas in a TGEmbeddedCanvas, and set _canvas to NULL
     *
     *  Convenience function to embed TCanvas* in a TGEmbeddedCanvas. This is
     *  implemented here because there is no other way to manage the python
     *  reference counting (can't notify python that we no longer own TCanvas
     *  memory from python interpreter)
     *
     *  WARNING: This will make a segmentation violation if ROOT TApplication
     *  has not been initialised. I (Rogers) don't know how to fix this, it
     *  seems to be a ROOT bug.
     */
    void EmbedCanvas(TRootEmbeddedCanvas* target);

  private:
    std::string _description;
    TCanvas* _canvas;

    MAUS_VERSIONED_CLASS_DEF(CanvasWrapper);
};
} // namespace MAUS
#endif

