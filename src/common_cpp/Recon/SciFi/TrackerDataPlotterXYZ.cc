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

// ROOT headers
#include "TAxis.h"

// MAUS headers
#include "src/common_cpp/Recon/SciFi/TrackerDataPlotterXYZ.hh"
#include "src/common_cpp/Recon/SciFi/TrackerDataPlotterSpoints.hh"
#include "src/common_cpp/Recon/SciFi/TrackerDataPlotterTracks.hh"


namespace MAUS {

TrackerDataPlotterXYZ::TrackerDataPlotterXYZ()
  : TrackerDataPlotterBase() {
  _spointsPlotter = new TrackerDataPlotterSpoints();
  _trksPlotter = new TrackerDataPlotterTracks();
}

TrackerDataPlotterXYZ::~TrackerDataPlotterXYZ() {
  delete _spointsPlotter;
  delete _trksPlotter;
}

TCanvas* TrackerDataPlotterXYZ::operator() ( TrackerData &t1, TrackerData &t2, TCanvas* aCanvas) {
  // If canvas if passed in use it, otherwise initialise the member canvas
  TCanvas* lCanvas(NULL);
  if ( aCanvas ) {
    // TODO check number of pads = 6
    lCanvas = aCanvas;
  } else {
    if (_Canvas) delete _Canvas;
    _Canvas = new TCanvas("sp_xy", "Spacepoint x-y Projections", 200, 10, 700, 500);
    _Canvas->Divide(3, 2);
    lCanvas = _Canvas;
  }

  // Plot the spacepoints
  std::cout << "Calling spacepoint plotting...\n";
  (*_spointsPlotter)(t1, t2, lCanvas);

  // Plot the track fits
  std::cout << "Calling track plotting...\n";
  (*_trksPlotter)(t1, t2, lCanvas);

  return lCanvas;
} // ~operator()

} // ~namespace MAUS

