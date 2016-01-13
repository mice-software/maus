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
  _histos.push_back(_h_tof01);
  _histos.push_back(_h_tof12);
  _histos.push_back(_h_tof02);

  // define canvases
  _canv_tof01 = new TCanvas("canv_tof01", "TOF0->1", 1600, 1200);
  _canv_tof12 = new TCanvas("canv_tof12", "TOF1->2", 1600, 1200);
  _canv_tof02 = new TCanvas("canv_tof02", "TOF0->2", 1600, 1200);
  _canvs.push_back(_canv_tof01);
  _canvs.push_back(_canv_tof12);
  _canvs.push_back(_canv_tof02);

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

  this->reset();
  _output->GetImage()->CanvasWrappersPushBack(cwrap_tof01);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_tof12);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_tof02);
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
  if (sp_tof0->size())
    t0 = sp_tof0->at(0).GetTime();

  if (sp_tof1->size())
    t1 = sp_tof1->at(0).GetTime();

  if (sp_tof2->size())
    t2 = sp_tof2->at(0).GetTime();

  if (sp_tof0->size() == 1 && sp_tof1->size() == 1)
    _h_tof01->Fill(t1-t0);

  if (sp_tof0->size() == 1 && sp_tof2->size() == 1)
    _h_tof02->Fill(t2-t0);

  if (sp_tof1->size() == 1 && sp_tof2->size() == 1)
    _h_tof12->Fill(t2-t1);
}
} // MAUS
