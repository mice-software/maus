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
#include "src/common_cpp/Recon/SciFi/TrackerDataPlotterSpoints.hh"


namespace MAUS {

TrackerDataPlotterSpoints::TrackerDataPlotterSpoints()
  : TrackerDataPlotterBase(),
  _gr_xy1(NULL),
  _gr_zx1(NULL),
  _gr_zy1(NULL),
  _gr_xy2(NULL),
  _gr_zx2(NULL),
  _gr_zy2(NULL) {
  // Do nothing
}

TrackerDataPlotterSpoints::~TrackerDataPlotterSpoints() {
  if ( _gr_xy1 ) delete _gr_xy1;
  if ( _gr_zx1 ) delete _gr_zx1;
  if ( _gr_zy1 ) delete _gr_zy1;
  if ( _gr_xy2 ) delete _gr_xy2;
  if ( _gr_zx2 ) delete _gr_zx2;
  if ( _gr_zy2 ) delete _gr_zy2;
}

TCanvas* TrackerDataPlotterSpoints::operator() ( TrackerData &t1, TrackerData &t2,
                                                 TCanvas* aCanvas) {
  // If canvas is passed in use it, otherwise use the member canvas
  TCanvas* lCanvas(NULL);
  if ( aCanvas ) {
    // TODO check number of pads = 6
    lCanvas = aCanvas;
    std::cerr << "Using input canvas at " << aCanvas << std::endl;
  } else { // If the local canvas is setup, delete it, and make it fresh
    if ( _Canvas ) delete _Canvas;
    _Canvas = new TCanvas("sp_xy", "Spacepoint x-y Projections", 200, 10, 700, 500);
    _Canvas->Divide(3, 2);
    lCanvas = _Canvas;
    std::cerr << "Creating new local canvas at " << _Canvas << std::endl;
  }

  // Draw the spacepoints
  lCanvas->cd(1);
  if ( _gr_xy1  ) delete _gr_xy1;
  _gr_xy1 = new TGraph(t1._spoints_x.size(), &(t1._spoints_x[0]), &(t1._spoints_y[0]));
  _gr_xy1->SetTitle("Tracker 1 X-Y Projection");
  _gr_xy1->GetXaxis()->SetTitle("x(mm)");
  _gr_xy1->GetYaxis()->SetTitle("y(mm)");
  _gr_xy1->SetMarkerStyle(20);
  _gr_xy1->SetMarkerColor(kBlack);
  _gr_xy1->Draw("AP same");
  lCanvas->Update();

  lCanvas->cd(2);
  if ( _gr_zx1  ) delete _gr_zx1;
  TGraph * gr_zx1 = new TGraph(t1._spoints_z.size(), &(t1._spoints_z[0]), &(t1._spoints_x[0]));
  gr_zx1->SetTitle("Tracker 1 Z-X Projection");
  gr_zx1->GetXaxis()->SetTitle("z(mm)");
  gr_zx1->GetYaxis()->SetTitle("x(mm)");
  gr_zx1->SetMarkerStyle(20);
  gr_zx1->SetMarkerColor(kBlack);
  gr_zx1->Draw("AP same");
  lCanvas->Update();

  lCanvas->cd(3);
  if ( _gr_zy1  ) delete _gr_zy1;
  TGraph * gr_zy1 = new TGraph(t1._spoints_z.size(), &(t1._spoints_z[0]), &(t1._spoints_y[0]));
  gr_zy1->SetTitle("Tracker 1 Z-Y Projection");
  gr_zy1->GetXaxis()->SetTitle("z(mm)");
  gr_zy1->GetYaxis()->SetTitle("y(mm)");
  gr_zy1->SetMarkerStyle(20);
  gr_zy1->SetMarkerColor(kBlack);
  gr_zy1->Draw("AP same");
  lCanvas->Update();

  lCanvas->cd(4);
  if ( _gr_xy2  ) delete _gr_xy2;
  TGraph * gr_xy2 = new TGraph(t2._spoints_x.size(), &(t2._spoints_x[0]), &(t2._spoints_y[0]));
  gr_xy2->SetTitle("Tracker 2 X-Y Projection");
  gr_xy2->GetXaxis()->SetTitle("x(mm)");
  gr_xy2->GetYaxis()->SetTitle("y(mm)");
  gr_xy2->SetMarkerStyle(20);
  gr_xy2->SetMarkerColor(kBlack);
  gr_xy2->Draw("AP same");
  lCanvas->Update();

  lCanvas->cd(5);
  if ( _gr_zx2  ) delete _gr_zx2;
  TGraph * gr_zx2 = new TGraph(t2._spoints_z.size(), &(t2._spoints_z[0]), &(t2._spoints_x[0]));
  gr_zx2->SetTitle("Tracker 2 Z-X Projection");
  gr_zx2->GetXaxis()->SetTitle("z(mm)");
  gr_zx2->GetYaxis()->SetTitle("x(mm)");
  gr_zx2->SetMarkerStyle(20);
  gr_zx2->SetMarkerColor(kBlack);
  gr_zx2->Draw("AP same");
  lCanvas->Update();

  lCanvas->cd(6);
  if ( _gr_zy2  ) delete _gr_zy2;
  TGraph * gr_zy2 = new TGraph(t2._spoints_z.size(), &(t2._spoints_z[0]), &(t2._spoints_y[0]));
  gr_zy2->SetTitle("Tracker 2 Z-X Projection");
  gr_zy2->GetXaxis()->SetTitle("z(mm)");
  gr_zy2->GetYaxis()->SetTitle("y(mm)");
  gr_zy2->SetMarkerStyle(20);
  gr_zy2->SetMarkerColor(kBlack);
  gr_zy2->Draw("AP same");
  lCanvas->Update();

  return lCanvas;
} // ~operator()

} // ~namespace MAUS

