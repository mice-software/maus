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
#include "src/common_cpp/DataStructure/TOFEvent.hh"
#include "src/common_cpp/DataStructure/TOFEventSpacePoint.hh"
#include "src/common_cpp/DataStructure/TOFSpacePoint.hh"

#include "src/common_cpp/DataStructure/ImageData/ImageData.hh"
#include "src/common_cpp/DataStructure/ImageData/Image.hh"
#include "src/common_cpp/DataStructure/ImageData/CanvasWrapper.hh"

#include "src/legacy/Interface/STLUtils.hh"
#include "src/legacy/Interface/Squeak.hh"

#include "src/reduce/ReduceCppTOFPlot/ReduceCppTOFPlot.hh"

namespace MAUS {

PyMODINIT_FUNC init_ReduceCppTOFPlot(void) {
    PyWrapReduceBase<ReduceCppTOFPlot>::PyWrapReduceBaseModInit(
                                  "ReduceCppTOFPlot", "", "", "", "");
}

ReduceCppTOFPlot::ReduceCppTOFPlot()
  : ReduceBase<Data, ImageData>("ReduceCppTOFPlot") {}

ReduceCppTOFPlot::~ReduceCppTOFPlot() {
  for (unsigned int i = 0; i < _histos.size(); i++)
    delete _histos[i];
  _histos.resize(0);

  // CanvasWrapper objects will be deleted by the ImageData destructor;
  // they own the canvas that they wrap.
}

void ReduceCppTOFPlot::_birth(const std::string& argJsonConfigDocument) {

  if (!_output) {
    throw MAUS::Exception(Exception::nonRecoverable,
                          "The output is disconnected.",
                          "ReduceCppTOFPlot::_birth");
  }

  if (!_output->GetImage())
    _output->SetImage(new MAUS::Image());

  //  JsonCpp setup
  Json::Value configJSON;
  configJSON = JsonWrapper::StringToJson(argJsonConfigDocument);

  _refresh_rate = configJSON["reduce_plot_refresh_rate"].asInt();
  _got_SOR = false;

  // Set ROOT to batch mode by default unless specified otherwise
  int root_batch_mode = 1;
  if ( configJSON["root_batch_mode"].isInt() )
      root_batch_mode = configJSON["root_batch_mode"].asInt();
  if ( root_batch_mode )
    gROOT->SetBatch();

  // define histograms
  _h_tof01 = new TH1F("ht01", "TOF0->1;Time (ns);;", 200, 20, 40);
  _h_tof12 = new TH1F("ht12", "TOF1->2;Time (ns);;", 200, 25, 45);
  _h_tof02 = new TH1F("ht02", "TOF0->2;Time (ns);;", 300, 50, 80);

  // Slab hits in X Y planes
  _hslabx_0 = new TH1F("slabx0", "Slab Hits X-view;SlabX;;", 10, -0.5, 9.5);
  _hslabx_0->SetLineColor(1);
  _hslabx_1 = new TH1F("slabx1", "Slab Hits X-view;SlabX;;", 10, -0.5, 9.5);
  _hslabx_1->SetLineColor(2);
  _hslabx_2 = new TH1F("slabx2", "Slab Hits X-view;SlabX;;", 10, -0.5, 9.5);
  _hslabx_2->SetLineColor(4);
  _hstack_slabx = new THStack("_hstack_slabx", "SlabHits X-view;SlabX;;");
  _hstack_slabx->Add(_hslabx_0);
  _hstack_slabx->Add(_hslabx_1);
  _hstack_slabx->Add(_hslabx_2);

  _hslaby_0 = new TH1F("slaby0", "Slab Hits Y-view;SlabY;;", 10, -0.5, 9.5);
  _hslaby_0->SetLineColor(1);
  _hslaby_1 = new TH1F("slaby1", "Slab Hits Y-view;SlabY;;", 10, -0.5, 9.5);
  _hslaby_1->SetLineColor(2);
  _hslaby_2 = new TH1F("slaby2", "Slab Hits Y-view;SlabY;;", 10, -0.5, 9.5);
  _hslaby_2->SetLineColor(4);
  _hstack_slaby = new THStack("_hstack_slaby", "SlabHits Y-view;SlabY;;");
  _hstack_slaby->Add(_hslaby_0);
  _hstack_slaby->Add(_hslaby_1);
  _hstack_slaby->Add(_hslaby_2);

  // Spacepoint X and Y
  _hspx_0 = new TH1F("spx0", "SpacePoints X-view;SlabX;;", 10, -0.5, 9.5);
  _hspx_0->SetLineColor(1);
  _hspx_1 = new TH1F("spx1", "SpacePoints X-view;SlabX;;", 10, -0.5, 9.5);
  _hspx_1->SetLineColor(2);
  _hspx_2 = new TH1F("spx2", "SpacePoints X-view;SlabX;;", 10, -0.5, 9.5);
  _hspx_2->SetLineColor(4);

  _hspy_0 = new TH1F("spy0", "SpacePoints Y-view;SlabY;;", 10, -0.5, 9.5);
  _hspy_0->SetLineColor(1);
  _hspy_1 = new TH1F("spy1", "SpacePoints Y-view;SlabY;;", 10, -0.5, 9.5);
  _hspy_1->SetLineColor(2);
  _hspy_2 = new TH1F("spy2", "SpacePoints Y-view;SlabY;;", 10, -0.5, 9.5);
  _hspy_2->SetLineColor(4);

  _hstack_spx = new THStack("_hstack_spx", "SpacePoint X;Slab;;");
  _hstack_spx->Add(_hspx_0);
  _hstack_spx->Add(_hspx_1);
  _hstack_spx->Add(_hspx_2);
  _hstack_spy = new THStack("_hstack_spy", "SpacePoint Y;Slab;;");
  _hstack_spy->Add(_hspy_0);
  _hstack_spy->Add(_hspy_1);
  _hstack_spy->Add(_hspy_2);

  // Number of spacepoints
  _hnsp_0 = new TH1F("nsp0", "#SpacePoints;#Spacepoints;;", 10, -0.5, 9.5);
  _hnsp_0->SetLineColor(1);
  _hnsp_1 = new TH1F("nsp1", "#SpacePoints;#Spacepoints;;", 10, -0.5, 9.5);
  _hnsp_1->SetLineColor(2);
  _hnsp_2 = new TH1F("nsp2", "#SpacePoints;#Spacepoints;;", 10, -0.5, 9.5);
  _hnsp_2->SetLineColor(4);
  _hstack_nsp = new THStack("_hstack_nsp", "# Spacepoints");
  _hstack_nsp->Add(_hnsp_0);
  _hstack_nsp->Add(_hnsp_1);
  _hstack_nsp->Add(_hnsp_2);

  // Spacepoint 2d Y vs X
  for (unsigned int s = 0; s < 3; ++s) {
    int nbins = 10, edgelo = -0.5, edgehi = 9.5;
    char tistr[30], namestr[30];
    snprintf(tistr, sizeof(tistr), "tof%d: space points;SlabY;SlabX", s);
    snprintf(namestr, sizeof(namestr), "hspxy_%d", s);
    _hspxy[s] = new TH2F(namestr, tistr, nbins, edgelo, edgehi, nbins, edgelo, edgehi);
  }

  // PMT hits for each PMT[0->1], Plane[0->1]
  for (int tof = 0; tof < 3; ++tof) {
    for (int pm = 0; pm < 2; ++pm) {
      for (int pl = 0; pl < 2; ++pl) {
          int nbins = 10, edgelo = -0.5, edgehi = 9.5;
          char tistr[30], namestr[30];
          snprintf(tistr, sizeof(tistr), "TOF: PMT %d Plane %d;Slab;;", pm, pl);
          snprintf(namestr, sizeof(namestr), "htof%d_pm%dpln%d", tof, pm, pl);
          _htof_pmt[tof][pm][pl] = new TH1F(namestr, tistr, nbins, edgelo, edgehi);
          _htof_pmt[tof][pm][pl]->SetLineColor(tof+1);
          _histos.push_back(_htof_pmt[tof][pm][pl]);
      }
    }
  }
  _hstack_pm0pln0 = new THStack("_hstack_pm0pln0", "TOF PMT0 Plane0");
  _hstack_pm0pln0->Add(_htof_pmt[0][0][0]);
  _hstack_pm0pln0->Add(_htof_pmt[1][0][0]);
  _hstack_pm0pln0->Add(_htof_pmt[2][0][0]);

  _hstack_pm0pln1 = new THStack("_hstack_pm0pln1", "TOF PMT0 Plane1");
  _hstack_pm0pln1->Add(_htof_pmt[0][0][1]);
  _hstack_pm0pln1->Add(_htof_pmt[1][0][1]);
  _hstack_pm0pln1->Add(_htof_pmt[2][0][1]);

  _hstack_pm1pln0 = new THStack("_hstack_pm1pln0", "TOF PMT1 Plane0");
  _hstack_pm1pln0->Add(_htof_pmt[0][1][0]);
  _hstack_pm1pln0->Add(_htof_pmt[1][1][0]);
  _hstack_pm1pln0->Add(_htof_pmt[2][1][0]);

  _hstack_pm1pln1 = new THStack("_hstack_pm1pln1", "TOF PMT1 Plane1");
  _hstack_pm1pln1->Add(_htof_pmt[0][1][1]);
  _hstack_pm1pln1->Add(_htof_pmt[1][1][1]);
  _hstack_pm1pln1->Add(_htof_pmt[2][1][1]);

  // Number of spacepoints vs spill
  _htof0_nspVspill = new TH1F("htof0nspVspill",
                              "TOF0 #spacepoints vs spill;Spill;# Spacepoints;",
                              5000, 1., 0.);
  _htof0_nspVspill->SetBit(TH1::kCanRebin);
  _htof1_nspVspill = new TH1F("htof1nspVspill",
                              "TOF0 #spacepoints vs spill;Spill;# Spacepoints;",
                              5000, 1., 0.);
  _htof1_nspVspill->SetBit(TH1::kCanRebin);
  _htof2_nspVspill = new TH1F("htof2nspVspill",
                              "TOF0 #spacepoints vs spill;Spill;# Spacepoints;",
                              5000, 1., 0.);
  _htof2_nspVspill->SetBit(TH1::kCanRebin);


  _histos.push_back(_h_tof01);
  _histos.push_back(_h_tof12);
  _histos.push_back(_h_tof02);
  _histos.push_back(_hspx_0);
  _histos.push_back(_hspx_1);
  _histos.push_back(_hspx_2);
  _histos.push_back(_hspy_0);
  _histos.push_back(_hspy_1);
  _histos.push_back(_hspy_2);
  _histos.push_back(_hnsp_0);
  _histos.push_back(_hnsp_1);
  _histos.push_back(_hnsp_2);
  _histos.push_back(_hslabx_0);
  _histos.push_back(_hslabx_1);
  _histos.push_back(_hslabx_2);
  _histos.push_back(_hslaby_0);
  _histos.push_back(_hslaby_1);
  _histos.push_back(_hslaby_2);
  _histos.push_back(_htof0_nspVspill);
  _histos.push_back(_htof1_nspVspill);
  _histos.push_back(_htof2_nspVspill);

  // define canvases
  _canv_tof01 = new TCanvas("canv_tof01", "TOF0->1", 1600, 1200);
  _canv_tof12 = new TCanvas("canv_tof12", "TOF1->2", 1600, 1200);
  _canv_tof02 = new TCanvas("canv_tof02", "TOF0->2", 1600, 1200);

  _canv_tof_spx = new TCanvas("canv_tof_spx", "TOF Spacepoint X", 1600, 1200);
  _canv_tof_spy = new TCanvas("canv_tof_spy", "TOF Spacepoint X", 1600, 1200);

  _canv_tof0_spxy = new TCanvas("canv_tof0_spxy", "TOF0 Spacepoint Y:X", 1600, 1200);
  _canv_tof1_spxy = new TCanvas("canv_tof1_spxy", "TOF1 Spacepoint Y:X", 1600, 1200);
  _canv_tof2_spxy = new TCanvas("canv_tof2_spxy", "TOF2 Spacepoint Y:X", 1600, 1200);

  _canv_tof_nsp = new TCanvas("canv_tof_nsp", "TOF #SpacePoints", 1600, 1200);

  _canv_tof0_nspVspill = new TCanvas("canv_tof0_nspVspill",
                                     "TOF0 #spacepoints vs spill", 1600, 1200);
  _canv_tof1_nspVspill = new TCanvas("canv_tof1_nspVspill",
                                     "TOF1 #spacepoints vs spill", 1600, 1200);
  _canv_tof2_nspVspill = new TCanvas("canv_tof2_nspVspill",
                                     "TOF2 #spacepoints vs spill", 1600, 1200);

  _canv_tof_slabx = new TCanvas("canv_tof_slabx", "TOF Horizontal(X) Slab", 1600, 1200);
  _canv_tof_slaby = new TCanvas("canv_tof_slaby", "TOF Vertical(Y) Slab", 1600, 1200);

  _canv_tof_pm0pln0 = new TCanvas("canv_tof_pm0pln0", "TOF Pmt0 Plane0", 1600, 1200);
  _canv_tof_pm0pln1 = new TCanvas("canv_tof_pm0pln1", "TOF Pmt0 Plane1", 1600, 1200);
  _canv_tof_pm1pln0 = new TCanvas("canv_tof_pm1pln0", "TOF Pmt1 Plane0", 1600, 1200);
  _canv_tof_pm1pln1 = new TCanvas("canv_tof_pm1pln1", "TOF Pmt1 Plane1", 1600, 1200);

  _canvs.push_back(_canv_tof01);
  _canvs.push_back(_canv_tof12);
  _canvs.push_back(_canv_tof02);
  _canvs.push_back(_canv_tof0_spxy);
  _canvs.push_back(_canv_tof1_spxy);
  _canvs.push_back(_canv_tof2_spxy);
  _canvs.push_back(_canv_tof_nsp);
  _canvs.push_back(_canv_tof_slabx);
  _canvs.push_back(_canv_tof_slaby);
  _canvs.push_back(_canv_tof_spx);
  _canvs.push_back(_canv_tof_spy);
  _canvs.push_back(_canv_tof_pm0pln0);
  _canvs.push_back(_canv_tof_pm0pln1);
  _canvs.push_back(_canv_tof_pm1pln0);
  _canvs.push_back(_canv_tof_pm1pln1);
  _canvs.push_back(_canv_tof0_nspVspill);
  _canvs.push_back(_canv_tof1_nspVspill);
  _canvs.push_back(_canv_tof2_nspVspill);

  TLegend* _leg_012 = new TLegend(.65, .7, .88, .88);
  _leg_012->SetLineColor(0);
  _leg_012->SetFillColor(0);
  _leg_012->AddEntry(_hspx_0, "TOF0", "l");
  _leg_012->AddEntry(_hspx_1, "TOF1", "l");
  _leg_012->AddEntry(_hspx_2, "TOF2", "l");
  // Add grid to all canvases.
  for (auto &canv:_canvs) {
    canv->SetGridx();
    canv->SetGridy();
  }

  CanvasWrapper *cwrap_tof01 = ReduceCppTools::get_canvas_wrapper(_canv_tof01,
                                                    		  _h_tof01,
						    		  "TOF01");

  CanvasWrapper *cwrap_tof12 = ReduceCppTools::get_canvas_wrapper(_canv_tof12,
                                                    		  _h_tof12,
						    		  "TOF12");

  CanvasWrapper *cwrap_tof02 = ReduceCppTools::get_canvas_wrapper(_canv_tof02,
                                                    		  _h_tof02,
						    		  "TOF02");

  CanvasWrapper *cwrap_tof0_spxy = ReduceCppTools::get_canvas_wrapper(_canv_tof0_spxy,
                                                               _hspxy[0],
                                                               "TOF0SpXY",
                                                               "TOF0SpXY", // imageTOF<name>
                                                               "colz&&text"); // draw option
  CanvasWrapper *cwrap_tof1_spxy = ReduceCppTools::get_canvas_wrapper(_canv_tof1_spxy,
                                                               _hspxy[1],
                                                               "TOF1SpXY",
                                                               "TOF1SpXY", // imageTOF<name>
                                                               "colz&&text"); // draw option
  CanvasWrapper *cwrap_tof2_spxy = ReduceCppTools::get_canvas_wrapper(_canv_tof2_spxy,
                                                               _hspxy[2],  // histograms
                                                               "TOF2SpXY", // canvas title
                                                               "TOF2SpXY", // imageTOF<name>
                                                               "colz&&text"); // draw option
  std::vector<TObject*> nsp_objs;
  nsp_objs.push_back(_hstack_nsp);
  nsp_objs.push_back(_leg_012);
  CanvasWrapper *cwrap_tof_nsp = ReduceCppTools::get_canvas_multi_wrapper(_canv_tof_nsp,
                                                    nsp_objs,
                                                    "TOFnSp",
                                                    "TOFnSp", // imageTOF<name>
                                                    "nostack"); // draw option
  std::vector<TObject*> slabx_objs;
  slabx_objs.push_back(_hstack_slabx);
  slabx_objs.push_back(_leg_012);
  CanvasWrapper *cwrap_tof_slabx = ReduceCppTools::get_canvas_multi_wrapper(_canv_tof_slabx,
                                                    slabx_objs,
                                                    "TOFSlabX",
                                                    "TOFSlabX", // imageTOF<name>
                                                    "nostack"); // draw option
  std::vector<TObject*> slaby_objs;
  slaby_objs.push_back(_hstack_slaby);
  slaby_objs.push_back(_leg_012);
  CanvasWrapper *cwrap_tof_slaby = ReduceCppTools::get_canvas_multi_wrapper(_canv_tof_slaby,
                                                    slaby_objs,
                                                    "TOFSlabY",
                                                    "TOFSlabY", // imageTOF<name>
                                                    "nostack"); // draw option

  std::vector<TObject*> pm0pln0_objs;
  pm0pln0_objs.push_back(_hstack_pm0pln0);
  pm0pln0_objs.push_back(_leg_012);
  CanvasWrapper *cwrap_tof_pm0pln0 = ReduceCppTools::get_canvas_multi_wrapper(_canv_tof_pm0pln0,
                                                    pm0pln0_objs,
                                                    "TOFPmt0Plane0",
                                                    "TOFPmt0Plane0", // imageTOF<name>
                                                    "nostack"); // draw option
  std::vector<TObject*> pm0pln1_objs;
  pm0pln1_objs.push_back(_hstack_pm0pln1);
  pm0pln1_objs.push_back(_leg_012);
  CanvasWrapper *cwrap_tof_pm0pln1 = ReduceCppTools::get_canvas_multi_wrapper(_canv_tof_pm0pln1,
                                                    pm0pln1_objs,
                                                    "TOFPmt0Plane1",
                                                    "TOFPmt0Plane1", // imageTOF<name>
                                                    "nostack"); // draw option
  std::vector<TObject*> pm1pln0_objs;
  pm1pln0_objs.push_back(_hstack_pm1pln0);
  pm1pln0_objs.push_back(_leg_012);
  CanvasWrapper *cwrap_tof_pm1pln0 = ReduceCppTools::get_canvas_multi_wrapper(_canv_tof_pm1pln0,
                                                    pm1pln0_objs,
                                                    "TOFPmt1Plane0",
                                                    "TOFPmt1Plane0", // imageTOF<name>
                                                    "nostack");
  std::vector<TObject*> pm1pln1_objs;
  pm1pln1_objs.push_back(_hstack_pm1pln1);
  pm1pln1_objs.push_back(_leg_012);
  CanvasWrapper *cwrap_tof_pm1pln1 = ReduceCppTools::get_canvas_multi_wrapper(_canv_tof_pm1pln1,
                                                    pm1pln1_objs,
                                                    "TOFPmt1Plane1",
                                                    "TOFPmt1Plane1", // imageTOF<name>
                                                    "nostack"); // draw option

  CanvasWrapper *cwrap_tof0_nspVspill = ReduceCppTools::get_canvas_wrapper(_canv_tof0_nspVspill,
                                                    		  _htof0_nspVspill,
						    		  "TOF0nSpVsSpill");

  CanvasWrapper *cwrap_tof1_nspVspill = ReduceCppTools::get_canvas_wrapper(_canv_tof1_nspVspill,
                                                    		  _htof1_nspVspill,
						    		  "TOF1nSpVsSpill");
  CanvasWrapper *cwrap_tof2_nspVspill = ReduceCppTools::get_canvas_wrapper(_canv_tof2_nspVspill,
                                                    		  _htof2_nspVspill,
						    		  "TOF2nSpVsSpill");
  std::vector<TObject*> spslabx;
  spslabx.push_back(_hstack_spx);
  spslabx.push_back(_leg_012);
  CanvasWrapper *cwrap_tof_spx = ReduceCppTools::get_canvas_multi_wrapper(_canv_tof_spx,
                                                    spslabx,
                                                    "TOFSpX",
                                                    "TOFSpX", // imageTOF<name>
                                                    "nostack"); // draw option
  std::vector<TObject*> spslaby;
  spslaby.push_back(_hstack_spy);
  spslaby.push_back(_leg_012);
  CanvasWrapper *cwrap_tof_spy = ReduceCppTools::get_canvas_multi_wrapper(_canv_tof_spy,
                                                    spslaby,
                                                    "TOFSpY",
                                                    "TOFSpY", // imageTOF<name>
                                                    "nostack"); // draw option

  this->reset();
  _output->GetImage()->CanvasWrappersPushBack(cwrap_tof01);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_tof12);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_tof02);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_tof0_spxy);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_tof1_spxy);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_tof2_spxy);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_tof_nsp);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_tof_slabx);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_tof_slaby);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_tof_pm0pln0);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_tof_pm0pln1);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_tof_pm1pln0);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_tof_pm1pln1);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_tof0_nspVspill);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_tof1_nspVspill);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_tof2_nspVspill);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_tof_spx);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_tof_spy);
}

void ReduceCppTOFPlot::_death() {}

void ReduceCppTOFPlot::_process(MAUS::Data* data) {
  if (data == NULL)
    throw Exception(Exception::recoverable, "Data was NULL",
                    "ReduceCppTOFPlot::_process");

  if (data->GetSpill() == NULL)
    throw Exception(Exception::recoverable, "Spill was NULL",
                    "ReduceCppTOFPlot::_process");

  std::string ev_type = data->GetSpill()->GetDaqEventType();
  int runNum = data->GetSpill()->GetRunNumber();
  if (!_got_SOR)
      setHistosTitle(runNum);

  if (ev_type != "physics_event")
     return;

  if (data->GetSpill()->GetReconEvents() == NULL)
     throw Exception(Exception::recoverable, "ReconEvents were NULL",
                        "ReduceCppTOFPlot::_process");

  int xRun = data->GetSpill()->GetRunNumber();
  int xSpill = data->GetSpill()->GetSpillNumber();
  _output->SetEventType(ev_type);
  _output->GetImage()->SetRunNumber(xRun);
  _output->GetImage()->SetSpillNumber(xSpill);

  ReconEventPArray* recon_events = data->GetSpill()->GetReconEvents();

  for (size_t i = 0;  i < recon_events->size(); ++i) {
    MAUS::TOFEvent* tof_event = recon_events->at(i)->GetTOFEvent();
    if (tof_event == NULL) continue;

    this->update_tof_plots(tof_event);
  }

  _process_count++;
  if ( !(_process_count % _refresh_rate) )
    this->update();
}

void ReduceCppTOFPlot::reset() {
  for (unsigned int i = 0; i < _histos.size(); i++)
    _histos[i]->Reset();

  _process_count = 0;
}

void ReduceCppTOFPlot::update() {
  for (unsigned int i = 0; i < _canvs.size(); i++) {
    _canvs[i]->cd()->Modified();
    _canvs[i]->Update();
  }
}

void ReduceCppTOFPlot::update_tof_plots(TOFEvent* tof_event) {
  MAUS::TOFEventDigit *digits = tof_event->GetTOFEventDigitPtr();
  MAUS::TOFEventSlabHit *slab_hits = tof_event->GetTOFEventSlabHitPtr();
  MAUS::TOFEventSpacePoint *space_points = tof_event->GetTOFEventSpacePointPtr();

  TOF0SpacePointArray *sp_tof0 = space_points->GetTOF0SpacePointArrayPtr();
  TOF1SpacePointArray *sp_tof1 = space_points->GetTOF1SpacePointArrayPtr();
  TOF2SpacePointArray *sp_tof2 = space_points->GetTOF2SpacePointArrayPtr();

  TOF0SlabHitArray *sh_tof0 = slab_hits->GetTOF0SlabHitArrayPtr();
  TOF1SlabHitArray *sh_tof1 = slab_hits->GetTOF1SlabHitArrayPtr();
  TOF2SlabHitArray *sh_tof2 = slab_hits->GetTOF2SlabHitArrayPtr();

  TOF0DigitArray *dig_tof0 = digits->GetTOF0DigitArrayPtr();
  TOF1DigitArray *dig_tof1 = digits->GetTOF1DigitArrayPtr();
  TOF2DigitArray *dig_tof2 = digits->GetTOF2DigitArrayPtr();

  float t0(0), t1(0), t2(0);
  float spnt_x(0), spnt_y(0);

  if (dig_tof0->size()) {
      for (unsigned int s = 0; s < dig_tof0->size(); ++s) {
          TOFDigit tdig = dig_tof0->at(s);
          int pmt = tdig.GetPmt();
          unsigned int slb = tdig.GetSlab();
          int pln = tdig.GetPlane();
          _htof_pmt[0][pmt][pln]->Fill(slb);
      }
  }
  if (dig_tof1->size()) {
      for (unsigned int s = 0; s < dig_tof1->size(); ++s) {
          TOFDigit tdig = dig_tof1->at(s);
          int pmt = tdig.GetPmt();
          unsigned int slb = tdig.GetSlab();
          int pln = tdig.GetPlane();
          _htof_pmt[1][pmt][pln]->Fill(slb);
      }
  }
  if (dig_tof2->size()) {
      for (unsigned int s = 0; s < dig_tof2->size(); ++s) {
          TOFDigit tdig = dig_tof2->at(s);
          int pmt = tdig.GetPmt();
          unsigned int slb = tdig.GetSlab();
          int pln = tdig.GetPlane();
          _htof_pmt[2][pmt][pln]->Fill(slb);
      }
  }
  if (sh_tof0->size()) {
    for (unsigned int s = 0; s < sh_tof0->size(); ++s) {
        TOFSlabHit tsh = sh_tof0->at(s);
        if (tsh.IsVertical())
            _hslaby_0->Fill(tsh.GetSlab());
        else if (tsh.IsHorizontal())
            _hslabx_0->Fill(tsh.GetSlab());
    }
  }
  if (sh_tof1->size()) {
    for (unsigned int s = 0; s < sh_tof1->size(); ++s) {
        TOFSlabHit tsh = sh_tof1->at(s);
        if (tsh.IsVertical())
            _hslaby_1->Fill(tsh.GetSlab());
        else if (tsh.IsHorizontal())
            _hslabx_1->Fill(tsh.GetSlab());
    }
  }
  if (sh_tof2->size()) {
    for (unsigned int s = 0; s < sh_tof2->size(); ++s) {
        TOFSlabHit tsh = sh_tof2->at(s);
        if (tsh.IsVertical())
            _hslaby_2->Fill(tsh.GetSlab());
        else if (tsh.IsHorizontal())
            _hslabx_2->Fill(tsh.GetSlab());
    }
  }
  if (sp_tof0->size()) {
    t0 = sp_tof0->at(0).GetTime();
    _hnsp_0->Fill(sp_tof0->size());
    int spillnum = sp_tof0->at(0).GetPhysEventNumber();
    _htof0_nspVspill->Fill(spillnum, sp_tof0->size());
    for (unsigned int s = 0; s < sp_tof0->size(); ++s) {
        TOFSpacePoint tsp = sp_tof0->at(s);
        spnt_x = tsp.GetSlabx();
        spnt_y = tsp.GetSlaby();
        _hspxy[0]->Fill(spnt_y, spnt_x);
        _hspx_0->Fill(spnt_x);
        _hspy_0->Fill(spnt_y);
    }
  }

  if (sp_tof1->size()) {
    t1 = sp_tof1->at(0).GetTime();
    _hnsp_1->Fill(sp_tof1->size());
    int spillnum = sp_tof1->at(0).GetPhysEventNumber();
    _htof1_nspVspill->Fill(spillnum, sp_tof1->size());
    for (unsigned int s = 0; s < sp_tof1->size(); ++s) {
        TOFSpacePoint tsp = sp_tof1->at(s);
        spnt_x = tsp.GetSlabx();
        spnt_y = tsp.GetSlaby();
        _hspxy[1]->Fill(spnt_y, spnt_x);
        _hspx_1->Fill(spnt_x);
        _hspy_1->Fill(spnt_y);
    }
  }

  if (sp_tof2->size()) {
    t2 = sp_tof2->at(0).GetTime();
    _hnsp_2->Fill(sp_tof2->size());
    int spillnum = sp_tof2->at(0).GetPhysEventNumber();
    _htof2_nspVspill->Fill(spillnum, sp_tof2->size());
    for (unsigned int s = 0; s < sp_tof2->size(); ++s) {
        TOFSpacePoint tsp = sp_tof2->at(s);
        spnt_x = tsp.GetSlabx();
        spnt_y = tsp.GetSlaby();
        _hspxy[2]->Fill(spnt_y, spnt_x);
        _hspx_1->Fill(spnt_x);
        _hspy_1->Fill(spnt_y);
    }
  }

  if (sp_tof0->size() == 1 && sp_tof1->size() == 1)
    _h_tof01->Fill(t1-t0);

  if (sp_tof0->size() == 1 && sp_tof2->size() == 1)
    _h_tof02->Fill(t2-t0);

  if (sp_tof1->size() == 1 && sp_tof2->size() == 1)
    _h_tof12->Fill(t2-t1);
}

void ReduceCppTOFPlot::setHistosTitle(int runNum) {
    std::cerr << ">>>>> SETTING TITLE " << runNum << std::endl;
    for (unsigned int h = 0; h < _histos.size(); ++h) {
        std::string orig_ti = std::string(_histos[h]->GetTitle(), strlen(_histos[h]->GetTitle()));
        char tistr[40];
        snprintf(tistr, sizeof(tistr), "Run %d: %s", runNum, orig_ti.c_str());
        _histos[h]->SetTitle(tistr);
        std::cerr << "h: " << h << " " << orig_ti << " " << tistr << std::endl;
    }
    for (unsigned int c = 0; c < _canvs.size(); ++c) {
        std::string orig_ti = std::string(_canvs[c]->GetTitle(), strlen(_canvs[c]->GetTitle()));
        char tistr[40];
        snprintf(tistr, sizeof(tistr), "Run %d: %s", runNum, orig_ti.c_str());
        _canvs[c]->SetTitle(tistr);
    }
    this->update();
    _got_SOR = true;
}
} // MAUS
