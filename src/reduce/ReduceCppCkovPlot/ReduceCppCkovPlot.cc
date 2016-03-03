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

#include <stdlib.h>

#include <vector>
#include <iostream>
#include <sstream>

#include "TPaveStats.h"
#include "TStyle.h"

#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/DataStructure/ImageData/ImageData.hh"
#include "src/common_cpp/DataStructure/ImageData/Image.hh"
#include "src/common_cpp/DataStructure/ImageData/CanvasWrapper.hh"
#include "src/legacy/Interface/Squeak.hh"
#include "src/reduce/ReduceCppCkovPlot/ReduceCppCkovPlot.hh"
#include "src/common_cpp/Utils/ReduceCppTools.hh"

namespace MAUS {

PyMODINIT_FUNC init_ReduceCppCkovPlot(void) {
    PyWrapReduceBase<ReduceCppCkovPlot>::PyWrapReduceBaseModInit(
                                  "ReduceCppCkovPlot", "", "", "", "");
}

ReduceCppCkovPlot::ReduceCppCkovPlot()
: ReduceBase<Data, ImageData>("ReduceCppCkovPlot"),
  _refresh_rate(60), _process_count(0) {}

ReduceCppCkovPlot::~ReduceCppCkovPlot() {
  // Everything will be deleted by the ImageData destructor.
}

void ReduceCppCkovPlot::_birth(const std::string& str_config) {

  if (!_output) {
    throw MAUS::Exception(Exception::nonRecoverable,
                          "The output is disconnected.",
                          "ReduceCppCkovPlot::_birth");
  }

  if (!_output->GetImage())
    _output->SetImage(new MAUS::Image());

  _process_count = 0;

  // Define histograms.
  _htof_A = new TH2F("tof_A", "tof_A", 100, 0, 50, 200, 15, 35);
  _htof_A->GetXaxis()->SetTitle("CkovA PE");
  _htof_A->GetYaxis()->SetTitle("t(TOF0 to TOF1) (ns)");

  _htof_B = new TH2F("tof_B", "tof_B", 100, 0, 50, 200, 15, 35);
  _htof_B->GetXaxis()->SetTitle("CkovB PE");
  _htof_B->GetYaxis()->SetTitle("t(TOF0 to TOF1) (ns)");

  _htof =  new TH1F("htof", "htof", 100, 10, 40);
  _htof->GetXaxis()->SetTitle("TOF(ns)");

  _hcharge.resize(8);
  int xPmt = 0;
  int nbins = 500, x_lo = -50, x_hi = 500;

  for (auto &h:_hcharge) {
    std::stringstream ss;
    ss << "PMT" << (++xPmt);
    const char* name = ss.str().c_str();
    h = new TH1F(name, name, nbins, x_lo, x_hi);
    h->GetXaxis()->SetTitle("Charge");
  }

  _htime.resize(8);
  xPmt = 0;
  nbins = 500;
  x_lo = -50;
  x_hi = 500;

  for (auto &h:_htime) {
    std::stringstream ss_n, ss_d;
    ss_n << "harr_time" << (++xPmt);
    ss_d << "Arrival Times PMT" << xPmt;

    const char *name  = ss_n.str().c_str();
    const char *descr = ss_d.str().c_str();
    h = new TH1F(name, descr, nbins, x_lo, x_hi);
    h->GetXaxis()->SetTitle("Uncalibrated Time (ns)");
  }

  // Define canvases
  TCanvas *canvas_tof_A  = new TCanvas("CkovReduce_ToF_A",       "tof_A",          600,  600);
  TCanvas *canvas_tof_B  = new TCanvas("CkovReduce_ToF_B",       "tof_B",          600,  600);
  TCanvas *canvas_tof    = new TCanvas("CkovReduce_ToF",         "tof",            600,  600);
  TCanvas *canvas_charge = new TCanvas("CkovReduce_charge",       "charge",        1200, 800);
  TCanvas *canvas_time   = new TCanvas("CkovReduce_ArrivalTimes", "Arrival times", 1200, 800);

  // Fill the arrays
  _canvs.push_back(canvas_tof_A);
  _canvs.push_back(canvas_tof_B);
  _canvs.push_back(canvas_tof);
  _canvs.push_back(canvas_charge);
  _canvs.push_back(canvas_time);

  _histos.push_back(_htof_A);
  _histos.push_back(_htof_B);
  _histos.push_back(_htof);
  _histos.insert(_histos.begin(), _hcharge.begin(), _hcharge.end());
  _histos.insert(_histos.begin(), _htime.begin(),   _htime.end());

  // Turn off stat box
  gStyle->SetOptStat(0);

  // Sensible color palette
  gStyle->SetPalette(1);

  CanvasWrapper *cwrap_tof_A = ReduceCppTools::get_canvas_wrapper(canvas_tof_A,
                                                                   _htof_A,
                                                                   "TOF_CkovA");

  CanvasWrapper *cwrap_tof_B = ReduceCppTools::get_canvas_wrapper(canvas_tof_B,
                                                                   _htof_B,
                                                                   "TOF_CkovB");

  CanvasWrapper *cwrap_tof = ReduceCppTools::get_canvas_wrapper(canvas_tof,
                                                                 _htof,
                                                                 "TOF");

  CanvasWrapper *cwrap_charge = ReduceCppTools::get_canvas_divide_wrapper(canvas_charge, 4, 2, true,
                                                                          _hcharge,
                                                                          "Charge_ckov");

  CanvasWrapper *cwrap_time = ReduceCppTools::get_canvas_divide_wrapper(canvas_time, 4, 2, true,
                                                                          _htime,
                                                                          "ArrivalTimes_ckov");

  _output->GetImage()->CanvasWrappersPushBack(cwrap_tof_A);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_tof_B);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_tof);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_charge);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_time);
}

void ReduceCppCkovPlot::_death() {}

void ReduceCppCkovPlot::_process(MAUS::Data* data) {
  if (data == NULL)
    throw Exception(Exception::recoverable, "Data was NULL",
                    "ReduceCppCkovPlot::_process");

  if (data->GetSpill() == NULL)
    throw Exception(Exception::recoverable, "Spill was NULL",
                    "ReduceCppCkovPlot::_process");

  std::string ev_type = data->GetSpill()->GetDaqEventType();
  if (ev_type != "physics_event")
     return;

  if (data->GetSpill()->GetReconEvents() == NULL)
     throw Exception(Exception::recoverable, "ReconEvents were NULL",
                        "ReduceCppCkovPlot::_process");

  int xRun = data->GetSpill()->GetRunNumber();
  int xSpill = data->GetSpill()->GetSpillNumber();
  _output->SetEventType(ev_type);
  _output->GetImage()->SetRunNumber(xRun);
  _output->GetImage()->SetSpillNumber(xSpill);

  ReconEventPArray* recEvts = data->GetSpill()->GetReconEvents();

  for (size_t i = 0;  i < recEvts->size(); ++i) {

    MAUS::CkovEvent *ckov_evt = recEvts->at(i)->GetCkovEvent();
    MAUS::TOFEvent  *tof_evt  = recEvts->at(i)->GetTOFEvent();
    if ( !ckov_evt || !tof_evt)
      continue;

    this->fill_Ckov_plots(ckov_evt, tof_evt);
  }

  if (!(_process_count%_refresh_rate))
    this->update_Ckov_plots();

  _process_count++;
}

void ReduceCppCkovPlot::reset() {
  for (size_t i = 0; i < _histos.size(); i++)
    _histos[i]->Reset();

  _process_count = 0;
}

void ReduceCppCkovPlot::fill_Ckov_plots(MAUS::CkovEvent* ckov_evt, MAUS::TOFEvent* tof_evt) {

  //  Get the tof event and tof0/tof1 spacepoints.
  TOFEventSpacePoint *tof_spoints = tof_evt->GetTOFEventSpacePointPtr();
  if (!tof_spoints)
    return;

  TOF0SpacePointArray *sp_tof0 = tof_spoints->GetTOF0SpacePointArrayPtr();
  TOF0SpacePointArray *sp_tof1 = tof_spoints->GetTOF1SpacePointArrayPtr();

  // Get the ckov digits
  CkovDigitArray *ckov_digs = ckov_evt->GetCkovDigitArrayPtr();

  // Number of photoelectrons in CKOVa and CKOVb.
  int PE_B = ckov_digs->at(0).GetCkovB().GetNumberOfPes();
  int PE_A = ckov_digs->at(0).GetCkovA().GetNumberOfPes();

  if (sp_tof0->size() == 1 && sp_tof1->size() == 1) {
    double t_0 = sp_tof0->at(0).GetTime();
    double t_1 = sp_tof1->at(0).GetTime();
    double TOF = t_1 - t_0;
    _htof->Fill(TOF);

    if (PE_B > 0)
      _htof_B->Fill(PE_B, TOF);

    if (PE_A > 0)
      _htof_A->Fill(PE_A, TOF);
  }

  double charge, time;

  charge = ckov_digs->at(0).GetCkovA().GetPulse0();
  time   = ckov_digs->at(0).GetCkovA().GetArrivalTime0();
  if (charge > -1000)
    _hcharge[0]->Fill(charge);
  if (time < 255)
    _htime[0]->Fill(time);

  charge = ckov_digs->at(0).GetCkovA().GetPulse1();
  time   = ckov_digs->at(0).GetCkovA().GetArrivalTime1();
  if (charge > -1000)
    _hcharge[1]->Fill(charge);
  if (time < 255)
    _htime[1]->Fill(time);

  charge = ckov_digs->at(0).GetCkovA().GetPulse2();
  time = ckov_digs->at(0).GetCkovA().GetArrivalTime2();
  if (charge > -1000)
    _hcharge[2]->Fill(charge);
  if (time < 255)
    _htime[2]->Fill(time);


  charge = ckov_digs->at(0).GetCkovA().GetPulse3();
  time = ckov_digs->at(0).GetCkovA().GetArrivalTime3();
  if (charge > -1000)
    _hcharge[3]->Fill(charge);
  if (time < 255)
    _htime[3]->Fill(time);

  charge = ckov_digs->at(0).GetCkovB().GetPulse4();
  time = ckov_digs->at(0).GetCkovB().GetArrivalTime4();
  if (charge > -1000)
    _hcharge[4]->Fill(charge);
  if (time < 255)
    _htime[4]->Fill(time);

  charge = ckov_digs->at(0).GetCkovB().GetPulse5();
  time = ckov_digs->at(0).GetCkovB().GetArrivalTime5();
  if (charge > -1000)
    _hcharge[5]->Fill(charge);
  if (time < 255)
    _htime[5]->Fill(time);

  charge = ckov_digs->at(0).GetCkovB().GetPulse6();
  time = ckov_digs->at(0).GetCkovB().GetArrivalTime6();
  if (charge > -1000)
    _hcharge[6]->Fill(charge);
  if (time < 255)
    _htime[6]->Fill(time);

  charge = ckov_digs->at(0).GetCkovB().GetPulse7();
  time = ckov_digs->at(0).GetCkovB().GetArrivalTime7();
  if (charge > -1000)
    _hcharge[7]->Fill(charge);
  if (time < 255)
    _htime[7]->Fill(time);
}

void ReduceCppCkovPlot::update_Ckov_plots() {
  for (auto &c:_canvs)
      c->Update();
}
} // MAUS
