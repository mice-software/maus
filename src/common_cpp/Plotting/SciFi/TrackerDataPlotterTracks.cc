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

// MAUS headers
#include "src/common_cpp/Plotting/SciFi/TrackerDataPlotterTracks.hh"

namespace MAUS {

TrackerDataPlotterTracks::TrackerDataPlotterTracks()
                         : TrackerDataPlotterBase() {
  // Do nothing
}

TrackerDataPlotterTracks::~TrackerDataPlotterTracks() {}

TCanvas* TrackerDataPlotterTracks::operator() (TrackerData &t1, TrackerData &t2, TCanvas* aCanvas) {

  // If canvas is passed in use it, otherwise initialise the member canvas
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

  // Draw the straight tracks
  for (size_t i = 0; i < t1._trks_str_xz.size(); ++i) {
    lCanvas->cd(2);
    t1._trks_str_xz[i].Draw("same");
    lCanvas->Update();
  }
  for (size_t i = 0; i < t1._trks_str_yz.size(); ++i) {
    lCanvas->cd(3);
    t1._trks_str_yz[i].Draw("same");
    lCanvas->Update();
  }
  for (size_t i = 0; i < t2._trks_str_xz.size(); ++i) {
    lCanvas->cd(5);
    t2._trks_str_xz[i].Draw("same");
    lCanvas->Update();
  }
  for (size_t i = 0; i < t2._trks_str_yz.size(); ++i) {
    lCanvas->cd(6);
    t2._trks_str_yz[i].Draw("same");
    lCanvas->Update();
  }

  // Draw the helical tracks
  for (size_t i = 0; i < t1._trks_xy.size(); ++i) {
    lCanvas->cd(1);
    t1._trks_xy[i].Draw("same");
    lCanvas->Update();
  }
  for (size_t i = 0; i < t1._trks_xz.size(); ++i) {
    lCanvas->cd(2);
    t1._trks_xz[i].Draw("same");
    lCanvas->Update();
  }
  for (size_t i = 0; i < t1._trks_yz.size(); ++i) {
    lCanvas->cd(3);
    t1._trks_yz[i].Draw("same");
    lCanvas->Update();
  }

  for (size_t i = 0; i < t2._trks_xy.size(); ++i) {
    lCanvas->cd(4);
    t2._trks_xy[i].Draw("same");
    lCanvas->Update();
  }
  for (size_t i = 0; i < t2._trks_xz.size(); ++i) {
    lCanvas->cd(5);
    t2._trks_xz[i].Draw("same");
    lCanvas->Update();
  }
  for (size_t i = 0; i < t2._trks_yz.size(); ++i) {
    lCanvas->cd(6);
    t2._trks_yz[i].Draw("same");
    lCanvas->Update();
  }

  return lCanvas;
} // ~operator()

} // ~namespace MAUS
