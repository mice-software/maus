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

// std includes
#include <vector>

// ROOT headers
#include "TAxis.h"

// MAUS headers
#include "src/common_cpp/Recon/SciFi/TrackerDataPlotterSZ.hh"


namespace MAUS {

TrackerDataPlotterSZ::TrackerDataPlotterSZ()
  : TrackerDataPlotterBase(),
  _gr_sz1(NULL),
  _gr_sz2(NULL) {
  // Do nothing
}

TrackerDataPlotterSZ::~TrackerDataPlotterSZ() {
  if ( _gr_sz1 ) delete _gr_sz1;
  if ( _gr_sz2 ) delete _gr_sz2;
}

TCanvas* TrackerDataPlotterSZ::operator() ( TrackerData &t1, TrackerData &t2, TCanvas* aCanvas) {
  // If canvas is passed in use it, otherwise use the member canvas
  TCanvas* lCanvas(NULL);
  if ( aCanvas ) {
    // TODO check number of pads = 6
    lCanvas = aCanvas;
  } else { // If the local canvas is setup, delete it, and make it fresh
    if ( _Canvas ) delete _Canvas;
    _Canvas = new TCanvas("sz", "Track S-Z Projection", 200, 10, 700, 500);
    _Canvas->Divide(2);
    lCanvas = _Canvas;
  }

  // Combine the data from all tracks into a single set of 1D vectors
  std::vector<double> t1_z;
  std::vector<double> t1_s;
  std::vector<double> t2_z;
  std::vector<double> t2_s;
  for (size_t i = 0; i < t1._seeds_z.size(); ++i) {
    t1_z.insert(t1_z.end(), t1._seeds_z[i].begin(), t1._seeds_z[i].end());
  }
  for (size_t i = 0; i < t1._seeds_s.size(); ++i) {
    t1_s.insert(t1_s.end(), t1._seeds_s[i].begin(), t1._seeds_s[i].end());
  }
  for (size_t i = 0; i < t2._seeds_z.size(); ++i) {
    t2_z.insert(t2_z.end(), t2._seeds_z[i].begin(), t2._seeds_z[i].end());
  }
  for (size_t i = 0; i < t2._seeds_s.size(); ++i) {
    t2_s.insert(t2_s.end(), t2._seeds_s[i].begin(), t2._seeds_s[i].end());
  }

  // Draw the seed spacepoints
  lCanvas->cd(1);
  if ( _gr_sz1  ) delete _gr_sz1;
  _gr_sz1 = new TGraph(t1_z.size(), &(t1_z[0]), &(t1_s[0]));
  _gr_sz1->SetTitle("Tracker 1 S-Z Projection");
  _gr_sz1->GetXaxis()->SetTitle("z(mm)");
  _gr_sz1->GetYaxis()->SetTitle("s(mm)");
  _gr_sz1->SetMarkerStyle(20);
  _gr_sz1->SetMarkerColor(kBlack);
  _gr_sz1->Draw("AP same");
  lCanvas->Update();

  lCanvas->cd(2);
  if ( _gr_sz2  ) delete _gr_sz2;
  _gr_sz2 = new TGraph(t2_z.size(), &(t2_z[0]), &(t2_s[0]));
  _gr_sz2->SetTitle("Tracker 2 S-Z Projection");
  _gr_sz2->GetXaxis()->SetTitle("z(mm)");
  _gr_sz2->GetYaxis()->SetTitle("s(mm)");
  _gr_sz2->SetMarkerStyle(20);
  _gr_sz2->SetMarkerColor(kBlack);
  _gr_sz2->Draw("AP same");
  lCanvas->Update();

  // Draw the fits
  for (size_t i = 0; i < t1._trks_sz.size(); ++i) {
    lCanvas->cd(1);
    t1._trks_sz[i].Draw("same");
    lCanvas->Update();
  }
  for (size_t i = 0; i < t1._trks_sz.size(); ++i) {
    lCanvas->cd(2);
    t2._trks_sz[i].Draw("same");
    lCanvas->Update();
  }

  return lCanvas;
} // ~operator()

} // ~namespace MAUS
