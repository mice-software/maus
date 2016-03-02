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
#include "src/reduce/ReduceCppKLPlot/ReduceCppKLPlot.hh"


namespace MAUS {

PyMODINIT_FUNC init_ReduceCppKLPlot(void) {
    PyWrapReduceBase<ReduceCppKLPlot>::PyWrapReduceBaseModInit(
                                  "ReduceCppKLPlot", "", "", "", "");
}

ReduceCppKLPlot::ReduceCppKLPlot()
: ReduceBase<Data, ImageData>("ReduceCppKLPlot"),
  _refresh_rate(60), _process_count(0) {}

ReduceCppKLPlot::~ReduceCppKLPlot() {
  // All objects will be deleted by the ImageData destructor.
}

void ReduceCppKLPlot::_birth(const std::string& str_config) {

  if (!_output) {
    throw MAUS::Exception(Exception::nonRecoverable,
                          "The output is disconnected.",
                          "ReduceCppKLPlot::_birth");
  }

  if (!_output->GetImage())
    _output->SetImage(new MAUS::Image());

  _process_count = 0;

  // define histograms
  hadc = new TH1F("h1", "ADC", 100, 0, 5000);
  hadc->GetXaxis()->SetTitle("ADC");

  hadc_product = new TH1F("h2", "ADC Product", 100, 0, 5000);
  hadc_product->GetXaxis()->SetTitle("ADC product");

  hprofile = new TH1F("h3", "Beam Y-profile", 21, -0.5, 20.5);
  hprofile->GetXaxis()->SetTitle("Cell");

  digitkl = new TH2F("h4", "Digits", 21, -0.5, 20.5, 2, -0.5, 1.5);
  digitkl->GetXaxis()->SetTitle("Cell");
  digitkl->GetYaxis()->SetTitle("PMT");
  digitkl->GetYaxis()->SetBinLabel(1, "0");
  digitkl->GetYaxis()->SetBinLabel(2, "1");

  _histos.push_back(hadc);
  _histos.push_back(hadc_product);
  _histos.push_back(hprofile);
  _histos.push_back(digitkl);

  // define canvases
  _canv_kl = new TCanvas("KLReduce", "kl");
  _canv_kl->cd();

  // Turn off stat box
  gStyle->SetOptStat(0);

  // Sensible color palette
  gStyle->SetPalette(1);

//   // XY grid on canvas
//   gStyle->SetPadGridX(1);
//   gStyle->SetPadGridY(1);

  _canv_kl->Divide(2, 2);

  _canv_kl->cd(1);
  hadc->Draw();

  _canv_kl->cd(2);
  hadc_product->Draw();

  _canv_kl->cd(3);
  hprofile->Draw();

  _canv_kl->cd(4);
  digitkl->Draw("text&&colz");

  this->reset();
  _cwrap_kl = new CanvasWrapper();
  _cwrap_kl->SetDescription("KL Plots");
  _cwrap_kl->SetFileTag("KL Plots");
  _cwrap_kl->SetCanvas(_canv_kl);

  _output->GetImage()->CanvasWrappersPushBack(_cwrap_kl);
}

void ReduceCppKLPlot::_death() {}

void ReduceCppKLPlot::_process(MAUS::Data* data) {
  if (data == NULL)
    throw Exception(Exception::recoverable, "Data was NULL",
                    "ReduceCppKLPlot::_process");

  if (data->GetSpill() == NULL)
    throw Exception(Exception::recoverable, "Spill was NULL",
                    "ReduceCppKLPlot::_process");

  std::string ev_type = data->GetSpill()->GetDaqEventType();
  if (ev_type != "physics_event")
     return;

  if (data->GetSpill()->GetReconEvents() == NULL)
     throw Exception(Exception::recoverable, "ReconEvents were NULL",
                        "ReduceCppKLPlot::_process");

  int xRun = data->GetSpill()->GetRunNumber();
  int xSpill = data->GetSpill()->GetSpillNumber();
  _output->SetEventType(ev_type);
  _output->GetImage()->SetRunNumber(xRun);
  _output->GetImage()->SetSpillNumber(xSpill);

  ReconEventPArray* recEvts = data->GetSpill()->GetReconEvents();

  for (size_t i = 0;  i < recEvts->size(); ++i) {

    MAUS::KLEvent* kl_evt = recEvts->at(i)->GetKLEvent();
    if ( !kl_evt )
      continue;

    this->fill_kl_plots(kl_evt);
  }

  if (!(_process_count%_refresh_rate))
    this->update_KL_plots();

  _process_count++;
}

void ReduceCppKLPlot::reset() {
  for (size_t i = 0; i < _histos.size(); i++)
    _histos[i]->Reset();

  _process_count = 0;
}

void ReduceCppKLPlot::fill_kl_plots(MAUS::KLEvent* evt) {

  MAUS::KLEventCellHit *cellhit     = evt->GetKLEventCellHitPtr();
  MAUS::KLCellHitArray kl_cellhits = cellhit->GetKLCellHitArray();

  for (auto & cells:kl_cellhits) {
    int prod = cells.GetChargeProduct();
    hadc_product->Fill(prod);
  }

  MAUS::KLEventDigit *kl_digit  = evt->GetKLEventDigitPtr();
  MAUS::KLDigitArray  kl_digits = kl_digit->GetKLDigitArray();

  for (auto & kd:kl_digits) {
//     if (!kd)
//       continue;

    int charge = kd.GetChargeMm();
    int cell   = kd.GetCell();
    int side   = kd.GetPmt();
    hadc->Fill(charge);
    hprofile->Fill(cell);
    digitkl->Fill(cell, side);
  }
}

void ReduceCppKLPlot::update_KL_plots() {
  _canv_kl->Update();
}
} // MAUS
