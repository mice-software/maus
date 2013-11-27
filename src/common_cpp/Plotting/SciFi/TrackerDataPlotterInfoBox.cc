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

// C++ headers
#include <sstream>

// MAUS headers
#include "src/common_cpp/Plotting/SciFi/TrackerDataPlotterInfoBox.hh"


namespace MAUS {

TrackerDataPlotterInfoBox::TrackerDataPlotterInfoBox()
  : TrackerDataPlotterBase(),
    _setup_true(false),
    _tot_digits_t1(0),
    _tot_clusters_t1(0),
    _tot_spoints_t1(0),
    _tot_5htracks_t1(0),
    _tot_4htracks_t1(0),
    _tot_3htracks_t1(0),
    _tot_5stracks_t1(0),
    _tot_4stracks_t1(0),
    _tot_3stracks_t1(0),
    _tot_pos_tracks_t1(0),
    _tot_neg_tracks_t1(0),
    _tot_digits_t2(0),
    _tot_clusters_t2(0),
    _tot_spoints_t2(0),
    _tot_5htracks_t2(0),
    _tot_4htracks_t2(0),
    _tot_3htracks_t2(0),
    _tot_5stracks_t2(0),
    _tot_4stracks_t2(0),
    _tot_3stracks_t2(0),
    _tot_pos_tracks_t2(0),
    _tot_neg_tracks_t2(0),
    _canvas_width(300),
    _canvas_height(750),
    _line1_pos(0.585),
    _line2_pos(0.945) {
    _p_label = new TPaveText(.0, .0, 0.55, 1.0);
    _p_t1 = new TPaveText(.68, .0, 0.75, 1.0);
    _p_t2 = new TPaveText(.88, .0, 0.95, 1.0);
    _line1 = new TLine(_line1_pos, 0.0, _line1_pos, 1.0);
    _line2 = new TLine(0.0, _line2_pos, 1.0, _line2_pos);
}

TrackerDataPlotterInfoBox::TrackerDataPlotterInfoBox(int canvas_width, int canvas_height,
                                                     double line1_pos, double line2_pos)
  : TrackerDataPlotterBase(),
    _setup_true(false),
    _tot_digits_t1(0),
    _tot_clusters_t1(0),
    _tot_spoints_t1(0),
    _tot_5htracks_t1(0),
    _tot_4htracks_t1(0),
    _tot_3htracks_t1(0),
    _tot_5stracks_t1(0),
    _tot_4stracks_t1(0),
    _tot_3stracks_t1(0),
    _tot_pos_tracks_t1(0),
    _tot_neg_tracks_t1(0),
    _tot_digits_t2(0),
    _tot_clusters_t2(0),
    _tot_spoints_t2(0),
    _tot_5htracks_t2(0),
    _tot_4htracks_t2(0),
    _tot_3htracks_t2(0),
    _tot_5stracks_t2(0),
    _tot_4stracks_t2(0),
    _tot_3stracks_t2(0),
    _tot_pos_tracks_t2(0),
    _tot_neg_tracks_t2(0) {
    _canvas_width = canvas_width;
    _canvas_height = canvas_height;
    _line1_pos = line1_pos;
    _line2_pos = line2_pos;
    _p_label = new TPaveText(.0, .0, 0.55, 1.0);
    _p_t1 = new TPaveText(.68, .0, 0.75, 1.0);
    _p_t2 = new TPaveText(.88, .0, 0.95, 1.0);
    _line1 = new TLine(_line1_pos, 0.0, _line1_pos, 1.0);
    _line2 = new TLine(0.0, _line2_pos, 1.0, _line2_pos);
}

TrackerDataPlotterInfoBox::~TrackerDataPlotterInfoBox() {
  delete _p_label;
  delete _p_t1;
  delete _p_t2;
  delete _line1;
  delete _line2;
}

void TrackerDataPlotterInfoBox::setup_paves(TCanvas* aCanvas) {
  aCanvas->cd();
  _line1->SetLineWidth(3);
  _line1->Draw();
  _line2->SetLineWidth(3);
  _line2->Draw();
  _p_t1->SetTextAlign(22);
  _p_t1->SetFillColor(0);
  _p_t1->SetBorderSize(0);
  _p_t2->SetTextAlign(22);
  _p_t2->SetFillColor(0);
  _p_t2->SetBorderSize(0);
  _p_label->SetTextAlign(12);
  _p_label->SetFillColor(0);
  _p_label->SetBorderSize(0);
  _p_label->Clear();
  _p_label->AddText("Tracker");
  _p_label->AddText("Spill num");
  _p_label->AddText("Events");
  _p_label->AddText("Digits");
  _p_label->AddText("Clusters");
  _p_label->AddText("Spacepoints");
  _p_label->AddText("Positive Tracks");
  _p_label->AddText("Negative Tracks");
  _p_label->AddText("Str Tracks");
  _p_label->AddText("4pt Helical Tracks");
  _p_label->AddText("5pt Helical Tracks");
  _p_label->AddText("Total Digits");
  _p_label->AddText("Total Clusters");
  _p_label->AddText("Total Spacepoints");
  _p_label->AddText("Total Positive Tracks");
  _p_label->AddText("Total Negative Tracks");
  _p_label->AddText("Total Str Tracks");
  _p_label->AddText("Total 4pt Helical Tracks");
  _p_label->AddText("Total 5pt Helical Tracks");
  _p_label->Draw();
  _setup_true = true;
}

TCanvas* TrackerDataPlotterInfoBox::operator() ( TrackerData &t1, TrackerData &t2,
                                                 TCanvas* aCanvas) {
  // If canvas if passed in use it, otherwise initialise the member canvas
  TCanvas* lCanvas(NULL);
  if ( aCanvas ) {
    // std::cout << "Using input canvas at " << aCanvas << std::endl;
    lCanvas = aCanvas;
  } else {
    if (_Canvas) {
      // std::cout << "Using current local canvas at " << _Canvas << std::endl;
      _Canvas->Clear();
    } else {
    _Canvas = new TCanvas("c_info", "Info Box", 1500, 000, _canvas_width, _canvas_height);
    // std::cout << "Creating new canvas at " << _Canvas << std::endl;
    }
    lCanvas = _Canvas;
  }

  // Check the TPaves have been initialised, and if not do so
  setup_paves(lCanvas);

  // Update the TPave with the passed in info
  _p_t1->Clear();
  _p_t2->Clear();

  int num_stracks_t1 = t1._num_stracks_5pt + t1._num_stracks_4pt + t1._num_stracks_3pt;
  // int num_htracks_t1 = t1._num_htracks_5pt + t1._num_htracks_4pt + t1._num_htracks_3pt;
  int num_stracks_t2 = t2._num_stracks_5pt + t2._num_stracks_4pt + t2._num_stracks_3pt;
  // int num_htracks_t2 = t2._num_htracks_5pt + t2._num_htracks_4pt + t2._num_htracks_3pt;

  _tot_digits_t1 += t1._num_digits;
  _tot_clusters_t1 += t1._num_clusters;
  _tot_spoints_t1 += t1._num_spoints;
  _tot_pos_tracks_t1 += t1._num_pos_tracks;
  _tot_neg_tracks_t1 += t1._num_neg_tracks;
  _tot_5stracks_t1 += t1._num_stracks_5pt;
  _tot_4stracks_t1 += t1._num_stracks_4pt;
  _tot_3stracks_t1 += t1._num_stracks_3pt;
  _tot_5htracks_t1 += t1._num_htracks_5pt;
  _tot_4htracks_t1 += t1._num_htracks_4pt;
  _tot_3htracks_t1 += t1._num_htracks_3pt;

  _tot_digits_t2 += t2._num_digits;
  _tot_clusters_t2 += t2._num_clusters;
  _tot_spoints_t2 += t2._num_spoints;
  _tot_pos_tracks_t2 += t2._num_pos_tracks;
  _tot_neg_tracks_t2 += t2._num_neg_tracks;
  _tot_5stracks_t2 += t2._num_stracks_5pt;
  _tot_4stracks_t2 += t2._num_stracks_4pt;
  _tot_3stracks_t2 += t2._num_stracks_3pt;
  _tot_5htracks_t2 += t2._num_htracks_5pt;
  _tot_4htracks_t2 += t2._num_htracks_4pt;
  _tot_3htracks_t2 += t2._num_htracks_3pt;

  _p_t1->AddText("1");
  _p_t1->AddText(chr(t1._spill_num));
  _p_t1->AddText(chr(t1._num_events));
  _p_t1->AddText(chr(t1._num_digits));
  _p_t1->AddText(chr(t1._num_clusters));
  _p_t1->AddText(chr(t1._num_spoints));
  _p_t1->AddText(chr(t1._num_pos_tracks));
  _p_t1->AddText(chr(t1._num_neg_tracks));
  _p_t1->AddText(chr(num_stracks_t1));
  _p_t1->AddText(chr(t1._num_htracks_4pt));
  _p_t1->AddText(chr(t1._num_htracks_5pt));
  _p_t1->AddText(chr(_tot_digits_t1));
  _p_t1->AddText(chr(_tot_clusters_t1));
  _p_t1->AddText(chr(_tot_spoints_t1));
  _p_t1->AddText(chr(_tot_pos_tracks_t1));
  _p_t1->AddText(chr(_tot_neg_tracks_t1));
  _p_t1->AddText(chr(_tot_5stracks_t1 + _tot_4stracks_t1 + _tot_3stracks_t1));
  _p_t1->AddText(chr(_tot_4htracks_t1));
  _p_t1->AddText(chr(_tot_5htracks_t1));

  _p_t2->AddText("2");
  _p_t2->AddText(chr(t2._spill_num));
  _p_t2->AddText(chr(t2._num_events));
  _p_t2->AddText(chr(t2._num_digits));
  _p_t2->AddText(chr(t2._num_clusters));
  _p_t2->AddText(chr(t2._num_spoints));
  _p_t2->AddText(chr(t1._num_pos_tracks));
  _p_t2->AddText(chr(t1._num_neg_tracks));
  _p_t2->AddText(chr(num_stracks_t2));
  _p_t2->AddText(chr(t2._num_htracks_4pt));
  _p_t2->AddText(chr(t2._num_htracks_5pt));
  _p_t2->AddText(chr(_tot_digits_t2));
  _p_t2->AddText(chr(_tot_clusters_t2));
  _p_t2->AddText(chr(_tot_spoints_t2));
  _p_t2->AddText(chr(_tot_pos_tracks_t2));
  _p_t2->AddText(chr(_tot_neg_tracks_t2));
  _p_t2->AddText(chr(_tot_5stracks_t2 + _tot_4stracks_t2 + _tot_3stracks_t2));
  _p_t2->AddText(chr(_tot_4htracks_t2));
  _p_t2->AddText(chr(_tot_5htracks_t2));

  lCanvas->cd();
  _p_label->Draw();
  _p_t1->Draw();
  _p_t2->Draw();
  _line1->Draw();
  _line2->Draw();
  lCanvas->Update();

  return lCanvas;
}

const char* TrackerDataPlotterInfoBox::chr(int i) {
  std::stringstream ss1;
  ss1 << i;
  return ss1.str().c_str();
}

} // ~namespace MAUS
