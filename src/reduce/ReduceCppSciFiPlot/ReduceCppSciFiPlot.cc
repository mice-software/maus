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

#include <Python.h>
#include <stdlib.h>

#include <vector>
#include <iostream>
#include <string>

#include "TCanvas.h"
#include "TH1D.h"
#include "TMinuit.h"
#include "TFitResult.h"
#include "TF1.h"
#include "TLatex.h"
#include "TPaveText.h"
#include "TPaveStats.h"

#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"

#include "src/common_cpp/DataStructure/ImageData/ImageData.hh"
#include "src/common_cpp/DataStructure/ImageData/Image.hh"
#include "src/common_cpp/DataStructure/ImageData/CanvasWrapper.hh"

#include "src/legacy/Interface/STLUtils.hh"
#include "Utils/Squeak.hh"

#include "src/reduce/ReduceCppSciFiPlot/ReduceCppSciFiPlot.hh"

namespace MAUS {

PyMODINIT_FUNC init_ReduceCppSciFiPlot(void) {
    PyWrapReduceBase<ReduceCppSciFiPlot>::PyWrapReduceBaseModInit(
                                  "ReduceCppSciFiPlot", "", "", "", "");
}

ReduceCppSciFiPlot::ReduceCppSciFiPlot()
  : ReduceBase<Data, ImageData>("ReduceCppSciFiPlot") {}

ReduceCppSciFiPlot::~ReduceCppSciFiPlot() {
  // Everything will be deleted by the ImageData destructor.
}

void ReduceCppSciFiPlot::_birth(const std::string& argJsonConfigDocument) {

  if (!_output) {
    throw MAUS::Exceptions::Exception(Exceptions::nonRecoverable,
                          "The output is disconnected.",
                          "ReduceCppSciFiPlot::_birth");
  }

  if (!_output->GetImage())
    _output->SetImage(new MAUS::Image());

  //  JsonCpp setup
  Json::Value configJSON;
  configJSON = JsonWrapper::StringToJson(argJsonConfigDocument);

  _refresh_rate = configJSON["reduce_plot_refresh_rate"].asInt();

  // Set ROOT to batch mode by default unless specified otherwise
  int root_batch_mode = 1;
  if ( configJSON["root_batch_mode"].isInt() )
      root_batch_mode = configJSON["root_batch_mode"].asInt();
  if ( root_batch_mode )
    gROOT->SetBatch();

  // setup vectors to hold histogram objects
  std::vector<TH1*> sp_types;
  std::vector<TH1*> tku_sptrips, tkd_sptrips;
  std::vector<TH1*> tku_spdoubs, tkd_spdoubs;
  std::vector<TH1*> dig_st_vec, sp_st_vec;
  std::vector<TH1*> kuno_vec;

  // reusable histogram name and title strings
  std::string h_name, h_title;

  // setup histograms
  _eff_plot = new TH1F("SP_Type", "Spacepoint Type Across All Stations; Station;", 5, -0.5, 4.5);

  _track1_2Clus = new TH1F("Tr1pStSp", "Spacepoint Doublets TKU; Station;", 5, 0.5, 5.5);

  _track2_2Clus = new TH1F("Tr2pStSp", "Spacepoint Doublets TKD; Station", 5, 0.5, 5.5);

  _track1_3Clus = new TH1F("Tr1pStSp", "Spacepoint Triplets TKU; Station;", 5, 0.5, 5.5);

  _track2_3Clus = new TH1F("Tr2pStSp", "Spacepoint Triplets TKD; Station;", 5, 0.5, 5.5);

  // push into a vector all histograms belonging to same canvas
  sp_types.push_back(_eff_plot);
  sp_types.push_back(_track1_3Clus);
  sp_types.push_back(_track2_3Clus);
  sp_types.push_back(_track1_2Clus);
  sp_types.push_back(_track2_2Clus);

  // setup histograms per tracker/station/plane
  for (int tr = 0; tr < 2; ++tr) {
      std::string trs;
      if (tr == 0)
            trs = "U";
      if (tr == 1)
            trs = "D";

      // digits per station, one for each tracker
      h_name = "Digits_TK" + trs;
      h_title = "Digits per Station TK" + trs;
      _dig_st_hist[tr] = new TH1F(h_name.c_str(), h_title.c_str(), 5, 0.5, 5.5);
      _dig_st_hist[tr]->GetXaxis()->SetTitle("Station");
      _histos.push_back(_dig_st_hist[tr]);
      dig_st_vec.push_back(_dig_st_hist[tr]);

      // spacepoints per station, one for each tracker
      h_name = "SP_TK" + trs;
      h_title = "Spacepoints per Station TK" + trs;
      _sp_st_hist[tr] = new TH1F(h_name.c_str(), h_title.c_str(), 5, 0.5, 5.5);
      _sp_st_hist[tr]->GetXaxis()->SetTitle("Station");
      _histos.push_back(_sp_st_hist[tr]);
      sp_st_vec.push_back(_sp_st_hist[tr]);

      // kuno sum, one for each tracker
      h_name = "Kuno_Sum_TK" + trs;
      h_title = "Kuno Sum TK" + trs;
      _kuno_hist[tr] = new TH1F(h_name.c_str(), h_title.c_str(), 670, -5, 642);
      _kuno_hist[tr]->GetXaxis()->SetTitle("Channel sum");
      _histos.push_back(_kuno_hist[tr]);
      kuno_vec.push_back(_kuno_hist[tr]);

      // go through all stations, /* NOTE station numbers run from 1->5 */
      for (int st = 1; st < 6; ++st) {
          // XY 2d of triplets
          h_name = "SP_Triplet TK" + trs + " Station " + std::to_string(st);
          h_title = "Space Point Triplets TK" + trs + " Station " + std::to_string(st);
          _spt_hist[tr][st] = new TH2F(h_name.c_str(), h_title.c_str(),
                                       200, -200, 200, 200, -200, 200);
          _spt_hist[tr][st]->GetXaxis()->SetTitle("X [mm]");
          _spt_hist[tr][st]->GetYaxis()->SetTitle("Y [mm]");

          // XY 2d of doublets
          h_name = "SP_Doublet TK" + trs + " Station " + std::to_string(st);
          h_title = "Space Point Doublets TK" + trs + " Station " + std::to_string(st);
          _spd_hist[tr][st] = new TH2F(h_name.c_str(), h_title.c_str(),
                                       200, -200, 200, 200, -200, 200);
          _spd_hist[tr][st]->GetXaxis()->SetTitle("X [mm]");
          _spd_hist[tr][st]->GetYaxis()->SetTitle("Y [mm]");

          if (tr == 0) {
              tku_sptrips.push_back(_spt_hist[tr][st]);
              tku_spdoubs.push_back(_spd_hist[tr][st]);
          }
          if (tr == 1) {
              tkd_sptrips.push_back(_spt_hist[tr][st]);
              tkd_spdoubs.push_back(_spd_hist[tr][st]);
          }

          for (int pl = 0; pl < 3; ++pl) {
              // digits per channel, for each station, each tracker
              h_name = "DC_Tk" + trs
                        + std::to_string(st) + std::to_string(pl);
              h_title = "Digits per Channel TK" + trs
                        + std::to_string(st) + std::to_string(pl);
              if (tr == 0) {
                  _dig_ch_hist_up[st][pl] = new TH1F(h_name.c_str(), h_title.c_str(),
                                                    215, 0, 215);
                  _dig_ch_hist_up[st][pl]->GetXaxis()->SetTitle("Channel");
                  tku_digs.push_back(_dig_ch_hist_up[st][pl]);
                  _histos.push_back(_dig_ch_hist_up[st][pl]);
              }
              if (tr == 1) {
                  _dig_ch_hist_dn[st][pl] = new TH1F(h_name.c_str(), h_title.c_str(),
                                                    215, 0, 215);
                  _dig_ch_hist_dn[st][pl]->GetXaxis()->SetTitle("Channel");
                  _histos.push_back(_dig_ch_hist_dn[st][pl]);
                  tkd_digs.push_back(_dig_ch_hist_dn[st][pl]);
              }
            }
        }
    }

  // now, setup the canvases to draw histograms on
  TCanvas* _canvas_SciFiSpacepoints = new TCanvas("SciFiReduce_SPtypes",
                                                  "Spacepoint_Types_in_Stations", 1200, 800);

  TCanvas* _canvas_SciFiDigitUS = new TCanvas("SciFiReduce_TKU_DigsPerChannel",
                                              "Digit_in_Channel_US", 1200, 800);

  TCanvas* _canvas_SciFiDigitDS = new TCanvas("SciFiReduce_TKD_DigsPerChannel",
                                              "Digit_in_Channel_DS", 1200, 800);

  TCanvas* _canvas_US_trip = new TCanvas("SciFiReduce_TKU_triplet_xy",
                                         "Spacepoint_Triplets_Up", 1200, 800);

  TCanvas* _canvas_DS_trip = new TCanvas("SciFiReduce_TKD_triplet_xy",
                                         "Spacepoint_Triplets_Down", 1200, 800);

  TCanvas* _canvas_US_doub = new TCanvas("SciFiReduce_TKU_doublet_xy",
                                         "Spacepoint_Doublets_Up", 1200, 800);

  TCanvas* _canvas_DS_doub = new TCanvas("SciFiReduce_TKD_doublet_xy",
                                         "Spacepoint_Doublets_Down", 1200, 800);

  TCanvas* _canvas_dig_st = new TCanvas("SciFiReduce_Digits",
                                        "Digits Per Station", 1200, 800);

  TCanvas* _canvas_sp_st = new TCanvas("SciFiReduce_Spacepoints",
                                       "Spacepoints Per Station", 1200, 800);

  TCanvas* _canvas_Kuno = new TCanvas("SciFiReduce_Kuno",
                                      "Kuno Sum", 1200, 800);

  // push canvas objects into a vector
  _canvs.push_back(_canvas_SciFiSpacepoints);
  _canvs.push_back(_canvas_SciFiDigitUS);
  _canvs.push_back(_canvas_SciFiDigitDS);
  _canvs.push_back(_canvas_US_trip);
  _canvs.push_back(_canvas_DS_trip);
  _canvs.push_back(_canvas_US_doub);
  _canvs.push_back(_canvas_DS_doub);
  _canvs.push_back(_canvas_dig_st);
  _canvs.push_back(_canvas_sp_st);
  _canvs.push_back(_canvas_Kuno);

  // push histos into vector
  // the histograms in this vec will get reset in the reset() method
  _histos.push_back(_eff_plot);
  _histos.push_back(_track1_2Clus);
  _histos.push_back(_track2_2Clus);
  _histos.push_back(_track1_3Clus);
  _histos.push_back(_track2_3Clus);
  _histos.insert(_histos.begin(), tku_digs.begin(), tku_digs.end());
  _histos.insert(_histos.begin(), tkd_digs.begin(), tkd_digs.end());
  _histos.insert(_histos.begin(), sp_types.begin(), sp_types.end());
  _histos.insert(_histos.begin(), tku_sptrips.begin(), tku_sptrips.end());
  _histos.insert(_histos.begin(), tkd_sptrips.begin(), tkd_sptrips.end());
  _histos.insert(_histos.begin(), tku_spdoubs.begin(), tku_spdoubs.end());
  _histos.insert(_histos.begin(), tkd_spdoubs.begin(), tkd_spdoubs.end());
  _histos.insert(_histos.begin(), dig_st_vec.begin(), dig_st_vec.end());
  _histos.insert(_histos.begin(), sp_st_vec.begin(), sp_st_vec.end());
  _histos.insert(_histos.begin(), kuno_vec.begin(), kuno_vec.end());

  // setup canvas wrappers to hold histograms+canvases
  CanvasWrapper *cwrap_eff = ReduceCppTools::get_canvas_divide_wrapper(_canvas_SciFiSpacepoints,
                                                                  3, 2, false,
                                                    		  sp_types,
  						    		  "TKSpacePointTypes");

  CanvasWrapper *cwrap_digUS = ReduceCppTools::get_canvas_divide_wrapper(_canvas_SciFiDigitUS,
                                                                  3, 5, false,
                                                    		  tku_digs,
  						    		  "TKUdigitsPerChannel");

  CanvasWrapper *cwrap_digDS = ReduceCppTools::get_canvas_divide_wrapper(_canvas_SciFiDigitDS,
                                                                  3, 5, false,
                                                    		  tkd_digs,
						    		  "TKDdigitsPerChannel");

  CanvasWrapper *cwrap_US_tripxy = ReduceCppTools::get_canvas_divide_wrapper(_canvas_US_trip,
                                                                  3, 2, false,
                                                    		  tku_sptrips,
						    		  "TKUtripletsXY",
						    		  "TKUtripletsXY",
						    		  "COLZ");

  CanvasWrapper *cwrap_DS_tripxy = ReduceCppTools::get_canvas_divide_wrapper(_canvas_DS_trip,
                                                                  3, 2, false,
                                                    		  tkd_sptrips,
						    		  "TKDtripletsXY",
						    		  "TKDtripletsXY",
						    		  "COLZ");
  CanvasWrapper *cwrap_US_doubxy = ReduceCppTools::get_canvas_divide_wrapper(_canvas_US_doub,
                                                                  3, 2, false,
                                                    		  tku_spdoubs,
						    		  "TKUdoubletsXY",
						    		  "TKUdoubletsXY",
						    		  "COLZ");

  CanvasWrapper *cwrap_DS_doubxy = ReduceCppTools::get_canvas_divide_wrapper(_canvas_DS_doub,
                                                                  3, 2, false,
                                                    		  tkd_spdoubs,
						    		  "TKDdoubletsXY",
						    		  "TKDdoubletsXY",
						    		  "COLZ");

  CanvasWrapper *cwrap_dig_st = ReduceCppTools::get_canvas_divide_wrapper(_canvas_dig_st,
                                                                  2, 1, false,
                                                    		  dig_st_vec,
						    		  "DigitsPerStation",
						    		  "DigitsPerStation");

  CanvasWrapper *cwrap_sp_st = ReduceCppTools::get_canvas_divide_wrapper(_canvas_sp_st,
                                                                  2, 1, false,
                                                    		  sp_st_vec,
						    		  "SpacepointsPerStation",
						    		  "SpacepointsPerStation");

  CanvasWrapper *cwrap_Kuno = ReduceCppTools::get_canvas_divide_wrapper(_canvas_Kuno,
                                                                  2, 1, false,
                                                    		  kuno_vec,
						    		  "TKKuno",
						    		  "TKKuno");

  // this->reset();
  // get images from canvaswrappers
  _output->GetImage()->CanvasWrappersPushBack(cwrap_eff);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_digUS);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_digDS);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_US_tripxy);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_DS_tripxy);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_US_doubxy);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_DS_doubxy);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_dig_st);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_sp_st);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_Kuno);
}

void ReduceCppSciFiPlot::_death() {}

void ReduceCppSciFiPlot::_process(MAUS::Data* data) {
  if (data == NULL)
    throw Exceptions::Exception(Exceptions::recoverable, "Data was NULL",
                    "ReduceCppSciFiPlot::_process");

  if (data->GetSpill() == NULL)
    throw Exceptions::Exception(Exceptions::recoverable, "Spill was NULL",
                    "ReduceCppSciFiPlot::_process");

  std::string ev_type = data->GetSpill()->GetDaqEventType();

  if (ev_type != "physics_event")
     return;

  if (data->GetSpill()->GetReconEvents() == NULL)
     throw Exceptions::Exception(Exceptions::recoverable, "ReconEvents were NULL",
                        "ReduceCppSciFiPlot::_process");

  int xRun = data->GetSpill()->GetRunNumber();
  int xSpill = data->GetSpill()->GetSpillNumber();
  _output->SetEventType(ev_type);
  _output->GetImage()->SetRunNumber(xRun);
  _output->GetImage()->SetSpillNumber(xSpill);

  ReconEventPArray* recon_events = data->GetSpill()->GetReconEvents();

  for (size_t i = 0;  i < recon_events->size(); ++i) {
    MAUS::SciFiEvent* scifi_event = recon_events->at(i)->GetSciFiEvent();
    if (scifi_event == NULL) continue;

    this->update_scifi_plots(scifi_event, xRun);
  }

  _process_count++;
  if ( !(_process_count % _refresh_rate) )
    this->update();
}

void ReduceCppSciFiPlot::reset() {
  for (unsigned int i = 0; i < _histos.size(); i++)
    _histos[i]->Reset();

  _process_count = 0;
}

void ReduceCppSciFiPlot::update() {
  for (unsigned int i = 0; i < _canvs.size(); i++) {
    _canvs[i]->cd()->Modified();
    _canvs[i]->Update();
  }
}

void ReduceCppSciFiPlot::update_scifi_plots(SciFiEvent* scifi_event, int runNum) {
  // get scifi digits
  MAUS::SciFiDigitPArray trk_digits = scifi_event->digits();
  // clusters
  MAUS::SciFiClusterPArray trk_clusters = scifi_event->clusters();
  // spacepoints
  MAUS::SciFiSpacePointPArray trk_spoints = scifi_event->spacepoints();

  std::string htitle;
  // go through digits
  if (trk_digits.size()) {
      for (size_t d = 0; d < trk_digits.size(); ++d) {
          MAUS::SciFiDigit* dig = trk_digits[d];
          int tr = dig->get_tracker();
          int st = dig->get_station();
          int pl = dig->get_plane();
          int ch = dig->get_channel();
          if (st < 0 || st > 5) continue;
          if (pl < 0 || pl > 2) continue;

          std::string trs;
          if (tr == 0)
            trs = "U";
          if (tr == 1)
            trs = "D";

          // Fill # digits per station
          _dig_st_hist[tr]->Fill(st);
          htitle = "Digits per Station TK" + trs + ", Run " + std::to_string(runNum);
          _dig_st_hist[tr]->SetTitle(htitle.c_str());

          htitle = "Digits per Channel TK" + trs
                   + " Station" + std::to_string(st)
                   + " Plane" + std::to_string(pl)
                   + ", Run " + std::to_string(runNum);
          if (tr == 0) {
              // Fill digits per channel
              _dig_ch_hist_up[st][pl]->Fill(ch);
              _dig_ch_hist_up[st][pl]->SetTitle(htitle.c_str());
          } else if (tr == 1) {
              // Fill digits per channel
              _dig_ch_hist_dn[st][pl]->Fill(ch);
              _dig_ch_hist_dn[st][pl]->SetTitle(htitle.c_str());
          }
      } // end loop over digits
  } // end check on if digits exist

  // now, go through spacepoints
  if (trk_spoints.size()) {
      int tkcnt[2], tkcnd[2];
      std::vector<double> chanlist[6];
      for (unsigned int s = 0; s < trk_spoints.size(); ++s) {
          MAUS::SciFiSpacePoint* sp = trk_spoints[s];
          int n_type = 0;
          double chansum = 0;
          int tr = sp->get_tracker();
          int st = sp->get_station();
          double x  = sp->get_position().x();
          double y  = sp->get_position().y();

          std::string trs;
          if (tr == 0)
            trs = "U";
          if (tr == 1)
            trs = "D";

          // Fill # spacepoints per station
          _sp_st_hist[tr]->Fill(st);
          htitle = "Spacepoints per Station TK" + trs + ", Run " + std::to_string(runNum);
          _sp_st_hist[tr]->SetTitle(htitle.c_str());

          std::string sptype = sp->get_type();
          // if triplet
          if (sptype == "triplet") {
              n_type = 3;
              tkcnt[tr] += 1;
              // Fill xy 2d for triplets for each station
              _spt_hist[tr][st]->Fill(x, y);
              htitle = "Spacepoint Triplets TK" + trs
                       + " Station" + std::to_string(st)
                       + ", Run " + std::to_string(runNum);
              _spt_hist[tr][st]->SetTitle(htitle.c_str());
              MAUS::SciFiClusterPArray tripclus = sp->get_channels_pointers();
              for (unsigned int cl = 0; cl < tripclus.size(); ++cl) {
                  MAUS::SciFiCluster *clust = tripclus[cl];
                  // compute Kuno sum for triplets
                  chansum += clust->get_channel();
              }
              // Fill kuno histogram
              _kuno_hist[tr]->Fill(chansum);
              htitle = "Kuno Sum TK" + trs + ", Run " + std::to_string(runNum);
              _kuno_hist[tr]->SetTitle(htitle.c_str());

              // fill # triplet spacepoints per station
              htitle = "Spacepoint Triplets TK" + trs + ", Run " + std::to_string(runNum);
              if (tr == 0) {
                  _track1_3Clus->Fill(st);
                  _track1_3Clus->SetTitle(htitle.c_str());
              } else  if (tr == 1) {
                  _track2_3Clus->Fill(st);
                  _track2_3Clus->SetTitle(htitle.c_str());
              }
          } else { // doublet
              n_type = 2;
              tkcnd[tr] += 1;
              // Fill xy 2d for doublets for each station
              _spd_hist[tr][st]->Fill(x, y);
              htitle = "Spacepoint Doublets TK" + trs
                       + " Station" + std::to_string(st)
                       + ", Run " + std::to_string(runNum);
              _spd_hist[tr][st]->SetTitle(htitle.c_str());
              // fill # doublet spacepoints per station
              if (tr == 0)
                  _track1_2Clus->Fill(st);
              if (tr == 1)
                  _track2_2Clus->Fill(st);
          }
          // histogram the spscepoint type across all stations in both trackers
          _eff_plot->Fill(n_type);
      } // end loop over spacepoints
  } // end check on spacepoint exists
}
} // MAUS
