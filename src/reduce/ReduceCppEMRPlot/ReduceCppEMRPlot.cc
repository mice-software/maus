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

#include "src/legacy/Interface/STLUtils.hh"
#include "src/legacy/Interface/Squeak.hh"

#include "src/reduce/ReduceCppEMRPlot/ReduceCppEMRPlot.hh"

namespace MAUS {

PyMODINIT_FUNC init_ReduceCppEMRPlot(void) {
    PyWrapReduceBase<ReduceCppEMRPlot>::PyWrapReduceBaseModInit(
                                  "ReduceCppEMRPlot", "", "", "", "");
}

ReduceCppEMRPlot::ReduceCppEMRPlot()
  : ReduceBase<Data, ImageData>("ReduceCppEMRPlot") {}

ReduceCppEMRPlot::~ReduceCppEMRPlot() {
  for (size_t i = 0; i < _objects.size(); i++)
    delete _objects[i];
  _objects.resize(0);

  for (size_t i = 0; i < _histos.size(); i++)
    delete _histos[i];
  _histos.resize(0);

  for (size_t i = 0; i < _canvs.size(); i++)
    delete _canvs[i];
  _canvs.resize(0);

  // CanvasWrapper objects will be deleted by the ImageData destructor.
}

void ReduceCppEMRPlot::_birth(const std::string& argJsonConfigDocument) {

  if (!_output) {
    throw MAUS::Exception(Exception::nonRecoverable,
                          "The output is disconnected.",
                          "ReduceCppEMRPlot::_birth");
  }

  if (!_output->GetImage())
    _output->SetImage(new MAUS::Image());

  //  JsonCpp setup
  Json::Value configJSON;
  configJSON = JsonWrapper::StringToJson(argJsonConfigDocument);

  // Fetch variables
  _refresh_rate = configJSON["reduce_plot_refresh_rate"].asInt();
  _density_cut = configJSON["EMRdensityCut"].asDouble();
  _chi2_cut = configJSON["EMRchi2Cut"].asDouble();

  // Set ROOT to batch mode by default unless specified otherwise
  int root_batch_mode = 1;
  if ( configJSON["root_batch_mode"].isInt() )
      root_batch_mode = configJSON["root_batch_mode"].asInt();
  if ( root_batch_mode )
    gROOT->SetBatch();

  // Define canvases
  _canv_occ_xz = new TCanvas("canv_occ_xz", "EMR xz occupancy", 1000, 1200);
  _canv_occ_yz = new TCanvas("canv_occ_yz", "EMR yz occupancy", 1000, 1200);
  _canv_beam_profile = new TCanvas("canv_beam_profile", "EMR xy beam profile", 1200, 1200);
  _canv_depth_profile = new TCanvas("canv_depth_profile", "EMR z depth profile", 1600, 1200);

  _canv_range = new TCanvas("canv_range", "EMR range", 1600, 1200);
  _canv_total_charge = new TCanvas("canv_total_charge", "EMR total charge", 1600, 1200);
  _canv_charge_ratio = new TCanvas("canv_charge_ratio", "EMR charge ratio", 1600, 1200);
  _canv_chi2_density = new TCanvas("canv_chi2_density", "EMR chi2 vs density", 1600, 1200);

  // Define occupancy histograms
  _hoccupancy_xz = new TH2EMR("occupancy_xz", "Occupancy in the xz plane");
  _hoccupancy_yz = new TH2EMR("occupancy_yz", "Occupancy in the yz plane");

  _hbeam_profile = new TH2F("beam_profile", "Beam profile in the xy plane",
			    59, -500, 500, 59, -500, 500);
  _hbeam_profile->GetXaxis()->SetTitle("x [mm]");
  _hbeam_profile->GetYaxis()->SetTitle("y [mm]");

  _hdepth_profile = new TH1F("depth_profile", "Beam profile along the Z axis", 48, 0, 48);
  _hdepth_profile->GetXaxis()->SetTitle("Plane ID");
  _hdepth_profile->SetLineWidth(2);

  // Define PID histograms
  _hrange_primary = new TH1F("range_primary", "EMR range", 100, 0, 1000);
  _hrange_primary->GetXaxis()->SetTitle("Range [mm]");
  _hrange_primary->SetLineWidth(2);
  _hrange_secondary = new TH1F("range_secondary", "EMR range", 100, 0, 1000);
  _hrange_secondary->SetLineWidth(2);
  _hrange_secondary->SetLineColor(2);

  _htotal_charge_ma = new TH1F("total_charge_ma", "EMR total charge", 100, 0, 10000);
  _htotal_charge_ma->GetXaxis()->SetTitle("Charge [ADC]");
  _htotal_charge_ma->SetLineWidth(2);
  _htotal_charge_sa = new TH1F("total_charge_sa", "EMR total charge", 100, 0, 10000);
  _htotal_charge_sa->SetLineWidth(2);
  _htotal_charge_sa->SetLineColor(2);

  _hcharge_ratio_ma = new TH1F("charge_ratio_ma", "EMR charge ratio", 50, 0, 2);
  _hcharge_ratio_ma->GetXaxis()->SetTitle("#bar{Q}_{4/5}/#bar{Q}_{1/5}");
  _hcharge_ratio_ma->SetLineWidth(2);
  _hcharge_ratio_sa = new TH1F("charge_ratio_sa", "EMR charge ratio", 50, 0, 2);
  _hcharge_ratio_sa->SetLineWidth(2);
  _hcharge_ratio_sa->SetLineColor(2);

  _hchi2_density = new TH2F("chi2_density",
			    "Normalised #chi^{2} against plane density #rho_{P}",
			    20, 0, 1.0001, 30, 0, 3);
  _hchi2_density->GetXaxis()->SetTitle("#rho_{P}");
  _hchi2_density->GetYaxis()->SetTitle("log_{10}(1+#hat{#chi}_{X}^{2}+#hat{#chi}_{Y}^{2})");

  // Define legends
  _leg_range = new TLegend(.65, .7, .88, .88);
  _leg_range->SetLineColor(0);
  _leg_range->SetFillColor(0);
  _leg_range->AddEntry(_hrange_primary, "Primary", "l");
  _leg_range->AddEntry(_hrange_secondary, "Secondary", "l");

  _leg_charge = new TLegend(.65, .7, .88, .88);
  _leg_charge->SetLineColor(0);
  _leg_charge->SetFillColor(0);
  _leg_charge->AddEntry(_htotal_charge_ma, "MAPMT", "l");
  _leg_charge->AddEntry(_htotal_charge_sa, "SAPMT", "l");

  // Define text comments
  _text_match_eff = new TText();
  _text_match_eff->SetTextAlign(32);
  _text_match_eff->SetTextSize(0.05);

  _text_pid_stats = new TText();
  _text_pid_stats->SetTextAlign(32);
  _text_pid_stats->SetTextSize(0.05);

  // Define lines
  _line_density = new TLine(_density_cut, 0, _density_cut, log10(1+_chi2_cut));
  _line_density->SetLineWidth(2);
  _line_chi2 = new TLine(_density_cut, log10(1+_chi2_cut), 1, log10(1+_chi2_cut));
  _line_chi2->SetLineWidth(2);

  // Fill the arrays
  _canvs.push_back(_canv_occ_xz);
  _canvs.push_back(_canv_occ_yz);
  _canvs.push_back(_canv_beam_profile);
  _canvs.push_back(_canv_depth_profile);
  _canvs.push_back(_canv_range);
  _canvs.push_back(_canv_total_charge);
  _canvs.push_back(_canv_charge_ratio);
  _canvs.push_back(_canv_chi2_density);

  _histos.push_back(_hoccupancy_xz->GetHistogram());
  _histos.push_back(_hoccupancy_yz->GetHistogram());
  _histos.push_back(_hbeam_profile);
  _histos.push_back(_hdepth_profile);
  _histos.push_back(_hrange_primary);
  _histos.push_back(_hrange_secondary);
  _histos.push_back(_htotal_charge_ma);
  _histos.push_back(_htotal_charge_sa);
  _histos.push_back(_hcharge_ratio_ma);
  _histos.push_back(_hcharge_ratio_sa);
  _histos.push_back(_hchi2_density);

  _objects.push_back(_leg_range);
  _objects.push_back(_leg_charge);
  _objects.push_back(_text_match_eff);
  _objects.push_back(_text_pid_stats);
  _objects.push_back(_line_density);
  _objects.push_back(_line_chi2);

  // Initialize the canvas wrappers
  _cwrap_occ_xz = ReduceCppTools::get_canvas_emr_wrapper(_canv_occ_xz,
                                                         _hoccupancy_xz,
                                                         "EMR_occupancy_xz",
                                                         "EMR xz occupancy");

  _cwrap_occ_yz = ReduceCppTools::get_canvas_emr_wrapper(_canv_occ_yz,
                                                         _hoccupancy_yz,
                                                         "EMR_occupancy_yz",
                                                         "EMR yz occupancy");

  _cwrap_beam_profile = ReduceCppTools::get_canvas_wrapper(_canv_beam_profile,
                                                   	   _hbeam_profile,
                                                    	   "EMR_profile_xy",
                                                   	   "EMR xy beam profile",
						    	   "COLZ");

  _cwrap_depth_profile = ReduceCppTools::get_canvas_wrapper(_canv_depth_profile,
                                                  	    _hdepth_profile,
                                                  	    "EMR_profile_z",
                                                  	    "EMR z depth profile");

  std::vector<TObject*> range;
  range.push_back(_hrange_primary);
  range.push_back(_hrange_secondary);
  range.push_back(_leg_range);
  range.push_back(_text_match_eff);
  _cwrap_range = ReduceCppTools::get_canvas_multi_wrapper(_canv_range,
                                                    	  range,
                                                    	  "EMR_range",
                                                    	  "EMR range");

  std::vector<TObject*> total_charge;
  total_charge.push_back(_htotal_charge_ma);
  total_charge.push_back(_htotal_charge_sa);
  total_charge.push_back(_leg_charge);
  _cwrap_total_charge = ReduceCppTools::get_canvas_multi_wrapper(_canv_total_charge,
                                   	                   	 total_charge,
                                               	           	 "EMR_total_charge",
                                                   	   	 "EMR total charge");

  std::vector<TObject*> charge_ratio;
  charge_ratio.push_back(_hcharge_ratio_ma);
  charge_ratio.push_back(_hcharge_ratio_sa);
  charge_ratio.push_back(_leg_charge);
  _cwrap_charge_ratio = ReduceCppTools::get_canvas_multi_wrapper(_canv_charge_ratio,
                                   	                   	 charge_ratio,
                                               	           	 "EMR_charge_ratio",
                                                   	   	 "EMR charge ratio");

  std::vector<TObject*> chi2_density;
  chi2_density.push_back(_hchi2_density);
  chi2_density.push_back(_text_pid_stats);
  chi2_density.push_back(_line_density);
  chi2_density.push_back(_line_chi2);
  _cwrap_chi2_density = ReduceCppTools::get_canvas_multi_wrapper(_canv_chi2_density,
                                   	                   	 chi2_density,
                                               	           	 "EMR_chi2_density",
                                                   	   	 "EMR chi2 vs density",
						    	  	 "COLZ");

  this->reset();
  _output->GetImage()->CanvasWrappersPushBack(_cwrap_occ_xz);
  _output->GetImage()->CanvasWrappersPushBack(_cwrap_occ_yz);
  _output->GetImage()->CanvasWrappersPushBack(_cwrap_beam_profile);
  _output->GetImage()->CanvasWrappersPushBack(_cwrap_depth_profile);
  _output->GetImage()->CanvasWrappersPushBack(_cwrap_range);
  _output->GetImage()->CanvasWrappersPushBack(_cwrap_total_charge);
  _output->GetImage()->CanvasWrappersPushBack(_cwrap_charge_ratio);
  _output->GetImage()->CanvasWrappersPushBack(_cwrap_chi2_density);
}

void ReduceCppEMRPlot::_death() {}

void ReduceCppEMRPlot::_process(MAUS::Data* data) {
  if (data == NULL)
    throw Exception(Exception::recoverable, "Data was NULL",
                    "ReduceCppEMRPlot::_process");

  if (data->GetSpill() == NULL)
    throw Exception(Exception::recoverable, "Spill was NULL",
                    "ReduceCppEMRPlot::_process");

  std::string ev_type = data->GetSpill()->GetDaqEventType();
  if (ev_type != "physics_event")
     return;

  if (data->GetSpill()->GetReconEvents() == NULL)
     throw Exception(Exception::recoverable, "ReconEvents were NULL",
                        "ReduceCppEMRPlot::_process");

  int xRun = data->GetSpill()->GetRunNumber();
  int xSpill = data->GetSpill()->GetSpillNumber();
  _output->SetEventType(ev_type);
  _output->GetImage()->SetRunNumber(xRun);
  _output->GetImage()->SetSpillNumber(xSpill);

  ReconEventPArray* recEvts = data->GetSpill()->GetReconEvents();

  for (size_t i = 0;  i < recEvts->size(); ++i) {

    MAUS::EMREvent* emr_event = recEvts->at(i)->GetEMREvent();

    if (emr_event == NULL) continue;
    this->fill_emr_plots(emr_event);
  }

  _process_count++;
  if ( !(_process_count % _refresh_rate) )
    this->update();
}

void ReduceCppEMRPlot::reset() {
  for (size_t i = 0; i < _histos.size(); i++)
    _histos[i]->Reset();

  _process_count = 0;
}

void ReduceCppEMRPlot::update() {

  // Efficiency of the track matching
  double match_eff(-1);
  if ( _hrange_primary->GetEntries() )
      match_eff = _hrange_secondary->GetEntries()/_hrange_primary->GetEntries();
  TString match_eff_string = "Match eff.: "+TString::Format("%.3f", match_eff);
  _text_match_eff->SetText(.95*_hrange_primary->GetXaxis()->GetXmax(),
			   .65*_hrange_primary->GetMaximum(),
			   match_eff_string);

  // PID statistics
  double mu_fraction(-1);
  int n_mu(0);
  for (int binx = _hchi2_density->GetNbinsX(); binx > 0; binx--) {
    if (_hchi2_density->GetXaxis()->GetBinCenter(binx) < _density_cut) break;
    for (int biny = 1; biny < _hchi2_density->GetNbinsX()+1; biny++) {
      if (_hchi2_density->GetYaxis()->GetBinCenter(biny) > log10(1+_chi2_cut)) break;
      n_mu += _hchi2_density->GetBinContent(binx, biny);
    }
  }
  if ( _hchi2_density->GetEntries() )
      mu_fraction = static_cast<double>(n_mu)/_hchi2_density->GetEntries();
  TString pid_stats_string = "Muon frac.: "+TString::Format("%.3f", mu_fraction);
  _text_pid_stats->SetText(.95*_hchi2_density->GetXaxis()->GetXmax(),
			   .95*_hchi2_density->GetYaxis()->GetXmax(),
			   pid_stats_string);

  // Update the canvases
  for (size_t i = 0; i < _canvs.size(); i++) {
    _canvs[i]->cd()->Modified();
    _canvs[i]->Update();
  }
}

void ReduceCppEMRPlot::fill_emr_plots(EMREvent* emr_event) {

  // OCCUPANCY PLOTS //
  // Loop over all the hits in the EMR fiducial volume
  EMRPlaneHitArray emr_planes = emr_event->GetEMRPlaneHitArray();
  double x0(-1000), y0(-1000), mTot(-1);
  for (size_t iPlane = 0; iPlane < emr_planes.size(); iPlane++) {
    EMRPlaneHit *emr_plane = emr_planes.at(iPlane);
    int xPlane = emr_plane->GetPlane();
    int xCharge = emr_plane->GetCharge();
    int xOri = emr_plane->GetOrientation();
    if (xCharge)
	_hdepth_profile->Fill(xPlane);

    EMRBarArray emr_bars = emr_plane->GetEMRBarArray();
    for (size_t iBar = 0; iBar < emr_bars.size(); iBar++) {
      EMRBar *emr_bar = emr_bars.at(iBar);
      int xBar = emr_bar->GetBar();

      EMRBarHitArray emr_hits = emr_bar->GetEMRBarHitArray();
      for (size_t iHit = 0; iHit < emr_hits.size(); iHit++) {
	EMRBarHit emr_hit = emr_hits.at(iHit);
	int xTot = emr_hit.GetTot();
        if ( !xTot ) continue;

        // Fill triangular bins in the xz and yz occupancy plots
        if ( !xOri ) {
          _hoccupancy_xz->Fill(xPlane, xBar);
	} else {
          _hoccupancy_yz->Fill(xPlane, xBar);
	}

	// Get the coordintes at the entrance of the EMR (most energetic hit)
	if (xPlane == 0 && xTot > mTot) x0 = (xBar-30)*17;
	if (xPlane == 1 && xTot > mTot) y0 = (xBar-30)*17;
      }
    }
  }
  // If a pair (x,y) was found, fill the histogram
  if (x0 > -1000 && y0 > -1000) _hbeam_profile->Fill(x0, y0);

  // PID VARIABLES PLOTS //
  // If there is no primary in the EMR, there is no PID...
  if ( !emr_event->GetHasPrimary() ) return;

  // Fill the range histograms
  _hrange_primary->Fill(emr_event->GetRangePrimary());
  if ( emr_event->GetHasSecondary() )
    _hrange_secondary->Fill(emr_event->GetRangeSecondary());

  // Fill the charge plots
  _htotal_charge_ma->Fill(emr_event->GetTotalChargeMA());
  _htotal_charge_sa->Fill(emr_event->GetTotalChargeSA());

  _hcharge_ratio_ma->Fill(emr_event->GetChargeRatioMA());
  _hcharge_ratio_sa->Fill(emr_event->GetChargeRatioSA());

  // Fill the PID TGraph, the muons are located in the bottom right corner
  _hchi2_density->Fill(emr_event->GetPlaneDensityMA(),
		       log10(1+emr_event->GetChi2()));
}
} // MAUS
