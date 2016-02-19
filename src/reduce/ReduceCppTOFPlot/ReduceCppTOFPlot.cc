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

  _hspslabx_0 = new TH1F("spx0", "SpacePoints X-plane;SlabX;", 10, -0.5, 9.5);
  _hspslabx_0->SetLineColor(1);
  _hspslabx_1 = new TH1F("spx1", "SpacePoints X-plane;SlabX;", 10, -0.5, 9.5);
  _hspslabx_1->SetLineColor(2);
  _hspslabx_2 = new TH1F("spx2", "SpacePoints X-plane;SlabX;", 10, -0.5, 9.5);
  _hspslabx_2->SetLineColor(4);

  _hspslaby_0 = new TH1F("spy0", "SpacePoints Y-plane;SlabY;", 10, -0.5, 9.5);
  _hspslaby_0->SetLineColor(1);
  _hspslaby_1 = new TH1F("spy1", "SpacePoints Y-plane;SlabY;", 10, -0.5, 9.5);
  _hspslaby_1->SetLineColor(2);
  _hspslaby_2 = new TH1F("spy2", "SpacePoints Y-plane;SlabY;", 10, -0.5, 9.5);
  _hspslaby_2->SetLineColor(4);

  _hnsp_0 = new TH1F("nsp0", "#SpacePoints;;", 10, -0.5, 9.5);
  _hnsp_0->SetLineColor(1);
  _hnsp_1 = new TH1F("nsp1", "#SpacePoints;;", 10, -0.5, 9.5);
  _hnsp_1->SetLineColor(2);
  _hnsp_2 = new TH1F("nsp2", "#SpacePoints;;", 10, -0.5, 9.5);
  _hnsp_2->SetLineColor(4);

  for (int s=0; s<3; ++s) {
    //std::stringstream tistr;
    //std::stringstream namestr;
    //namestr << "hspxy_" << s;
    //tistr << "tof%d: space points;SlabY;SlabX" % (i);
    int nbins = 10, edgelo = -0.5, edgehi = 9.5;
    char tistr[30], namestr[30];
    sprintf(tistr, "tof%d: space points;SlabY;SlabX",s);
    sprintf(namestr, "hspxy_%d",s);
    _hspxy[s] = new TH2F(namestr, tistr, nbins, edgelo, edgehi, nbins, edgelo, edgehi);
  }
  _histos.push_back(_h_tof01);
  _histos.push_back(_h_tof12);
  _histos.push_back(_h_tof02);
  _histos.push_back(_hspslabx_0);
  _histos.push_back(_hspslabx_1);
  _histos.push_back(_hspslabx_2);
  _histos.push_back(_hspslaby_0);
  _histos.push_back(_hspslaby_1);
  _histos.push_back(_hspslaby_2);
  _histos.push_back(_hnsp_0);
  _histos.push_back(_hnsp_1);
  _histos.push_back(_hnsp_2);
//  for (int s=0; s<3; ++s)
//      _histos.push_back(_hspxy[s]);

  // define canvases
  _canv_tof01 = new TCanvas("canv_tof01", "TOF0->1", 1600, 1200);
  _canv_tof12 = new TCanvas("canv_tof12", "TOF1->2", 1600, 1200);
  _canv_tof02 = new TCanvas("canv_tof02", "TOF0->2", 1600, 1200);
  _canv_tof_spslabx = new TCanvas("canv_tof_spslabx", "TOF Spacepoint X", 1600, 1200);
  _canv_tof_spslaby = new TCanvas("canv_tof_spslaby", "TOF Spacepoint Y", 1600, 1200);
  _canv_tof0_spxy = new TCanvas("canv_tof0_spxy", "TOF0 Spacepoint Y:X", 1600, 1200);
  _canv_tof1_spxy = new TCanvas("canv_tof1_spxy", "TOF1 Spacepoint Y:X", 1600, 1200);
  _canv_tof2_spxy = new TCanvas("canv_tof2_spxy", "TOF2 Spacepoint Y:X", 1600, 1200);
  _canv_tof_nsp = new TCanvas("canv_tof_nsp", "TOF #SpacePoints", 1600, 1200);
  _canvs.push_back(_canv_tof01);
  _canvs.push_back(_canv_tof12);
  _canvs.push_back(_canv_tof02);
  _canvs.push_back(_canv_tof_spslabx);
  _canvs.push_back(_canv_tof_spslaby);
  _canvs.push_back(_canv_tof0_spxy);
  _canvs.push_back(_canv_tof1_spxy);
  _canvs.push_back(_canv_tof2_spxy);
  _canvs.push_back(_canv_tof_nsp);

  // Add grid to all canvases.
  for (auto &canv:_canvs)
    canv->SetGridx();

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
                                                               "TOF0SpXY",
                                                               "colz&&text"); // imageTOF<name>
   CanvasWrapper *cwrap_tof1_spxy = ReduceCppTools::get_canvas_wrapper(_canv_tof1_spxy,
                                                               _hspxy[1],
                                                               "TOF1SpXY",
                                                               "TOF1SpXY",
                                                               "colz&&text"); // imageTOF<name>
   CanvasWrapper *cwrap_tof2_spxy = ReduceCppTools::get_canvas_wrapper(_canv_tof2_spxy,
                                                               _hspxy[2],
                                                               "TOF2SpXY",
                                                               "TOF2SpXY",
                                                               "colz&&text"); // imageTOF<name>
  std::vector<TObject*> spslabx_objs;
  spslabx_objs.push_back(_hspslabx_0);
  spslabx_objs.push_back(_hspslabx_1);
  spslabx_objs.push_back(_hspslabx_2);
  CanvasWrapper *cwrap_tof_spslabx = ReduceCppTools::get_canvas_multi_wrapper(_canv_tof_spslabx,
                                                    spslabx_objs,
                                                    "TOF0SpX",
                                                    "TOF0SpX"); // imageTOF<name> 

  std::vector<TObject*> spslaby_objs;
  spslaby_objs.push_back(_hspslaby_0);
  spslaby_objs.push_back(_hspslaby_1);
  spslaby_objs.push_back(_hspslaby_2);
  CanvasWrapper *cwrap_tof_spslaby = ReduceCppTools::get_canvas_multi_wrapper(_canv_tof_spslaby,
                                                    spslaby_objs,
                                                    "TOF0SpY",
                                                    "TOF0SpY"); // imageTOF<name> 
  std::vector<TObject*> nsp_objs;
  nsp_objs.push_back(_hnsp_0);
  nsp_objs.push_back(_hnsp_1);
  nsp_objs.push_back(_hnsp_2);
  CanvasWrapper *cwrap_tof_nsp = ReduceCppTools::get_canvas_multi_wrapper(_canv_tof_nsp,
                                                    nsp_objs,
                                                    "TOFnSp",
                                                    "TOFnSp"); // imageTOF<name> 
  this->reset();
  _output->GetImage()->CanvasWrappersPushBack(cwrap_tof01);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_tof12);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_tof02);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_tof_spslabx);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_tof_spslaby);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_tof0_spxy);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_tof1_spxy);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_tof2_spxy);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_tof_nsp);
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
  for (unsigned int i = 0; i < _canvs.size(); i++)
    _canvs[i]->Update();
}

void ReduceCppTOFPlot::update_tof_plots(TOFEvent* tof_event) {
  MAUS::TOFEventSpacePoint *space_points = tof_event->GetTOFEventSpacePointPtr();

  TOF0SpacePointArray *sp_tof0 = space_points->GetTOF0SpacePointArrayPtr();
  TOF1SpacePointArray *sp_tof1 = space_points->GetTOF1SpacePointArrayPtr();
  TOF2SpacePointArray *sp_tof2 = space_points->GetTOF2SpacePointArrayPtr();

  float t0(0), t1(0), t2(0);
  float spnt_x(0), spnt_y(0);
  if (sp_tof0->size()) {
    t0 = sp_tof0->at(0).GetTime();
    _hnsp_0->Fill(sp_tof0->size());
    for (int s=0; s < sp_tof0->size(); ++s) {
        TOFSpacePoint tsp = sp_tof0->at(s);
        spnt_x = tsp.GetSlabx();
        spnt_y = tsp.GetSlaby();
        _hspxy[0]->Fill(spnt_y, spnt_x);
        std::cerr << spnt_y << " " << spnt_x << std::endl;
        _hspslabx_0->Fill(spnt_x);
        _hspslaby_0->Fill(spnt_y);
    }
  }

  if (sp_tof1->size()) {
    t1 = sp_tof1->at(0).GetTime();
    _hnsp_1->Fill(sp_tof1->size());
    for (int s=0; s < sp_tof1->size(); ++s) {
        TOFSpacePoint tsp = sp_tof1->at(s);
        spnt_x = tsp.GetSlabx();
        spnt_y = tsp.GetSlaby();
        _hspxy[1]->Fill(spnt_y, spnt_x);
        _hspslabx_1->Fill(spnt_x);
        _hspslaby_1->Fill(spnt_y);
    }
  }

  if (sp_tof2->size()) {
    t2 = sp_tof2->at(0).GetTime();
    _hnsp_2->Fill(sp_tof2->size());
    for (int s=0; s < sp_tof2->size(); ++s) {
        TOFSpacePoint tsp = sp_tof2->at(s);
        spnt_x = tsp.GetSlabx();
        spnt_y = tsp.GetSlaby();
        _hspxy[2]->Fill(spnt_y, spnt_x);
        _hspslabx_1->Fill(spnt_x);
        _hspslaby_1->Fill(spnt_y);
    }
  }

  if (sp_tof0->size() == 1 && sp_tof1->size() == 1)
    _h_tof01->Fill(t1-t0);

  if (sp_tof0->size() == 1 && sp_tof2->size() == 1)
    _h_tof02->Fill(t2-t0);

  if (sp_tof1->size() == 1 && sp_tof2->size() == 1)
    _h_tof12->Fill(t2-t1);
}
} // MAUS
