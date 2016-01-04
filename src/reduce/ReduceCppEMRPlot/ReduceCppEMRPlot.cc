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

  // CanvasWrapper objects will be deleted by the ImageData destructor;
  // they own the canvas that they wrap.
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
  _number_of_bars = configJSON["EMRnumberOfBars"].asInt();
  _refresh_rate = configJSON["reduce_plot_refresh_rate"].asInt();
  _charge_threshold = configJSON["EMRchargeThreshold"].asDouble();
  _density_cut = configJSON["EMRdensityCut"].asDouble();
  _chi2_cut = configJSON["EMRchi2Cut"].asDouble();

  // Set ROOT to batch mode by default unless specified otherwise
  int root_batch_mode = 1;
  if ( configJSON["root_batch_mode"].isInt() )
      root_batch_mode = configJSON["root_batch_mode"].asInt();
  if ( root_batch_mode )
    gROOT->SetBatch();

  // Define canvases
  TCanvas* canv_occ_xz = new TCanvas("canv_occ_xz", "EMR xz occupancy", 1000, 1200);
  TCanvas* canv_occ_yz = new TCanvas("canv_occ_yz", "EMR yz occupancy", 1000, 1200);
  TCanvas* canv_beam_profile = new TCanvas("canv_beam_profile", "EMR xy beam profile", 1200, 1200);
  TCanvas* canv_depth_profile = new TCanvas("canv_depth_profile", "EMR depth profile", 1600, 1200);

  TCanvas* canv_range = new TCanvas("canv_range", "EMR range", 1600, 1200);
  TCanvas* canv_total_charge = new TCanvas("canv_total_charge", "EMR total charge", 1600, 1200);
  TCanvas* canv_charge_ratio = new TCanvas("canv_charge_ratio", "EMR charge ratio", 1600, 1200);
  TCanvas* canv_chi2_density = new TCanvas("canv_chi2_density", "EMR chi2 vs density", 1600, 1200);

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

  _htotal_charge_ma = new TH1F("total_charge_ma", "EMR total charge", 100, 0, 5000);
  _htotal_charge_ma->GetXaxis()->SetTitle("Charge [ADC]");
  _htotal_charge_ma->SetLineWidth(2);
  _htotal_charge_sa = new TH1F("total_charge_sa", "EMR total charge", 100, 0, 5000);
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
  _canvs.push_back(canv_occ_xz);
  _canvs.push_back(canv_occ_yz);
  _canvs.push_back(canv_beam_profile);
  _canvs.push_back(canv_depth_profile);
  _canvs.push_back(canv_range);
  _canvs.push_back(canv_total_charge);
  _canvs.push_back(canv_charge_ratio);
  _canvs.push_back(canv_chi2_density);

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

  // Add grid to all canvases.
  for (auto &canv:_canvs)
    canv->SetGrid();

  // Initialize the canvas wrappers
  CanvasWrapper* cwrap_occ_xz =
	ReduceCppTools::get_canvas_emr_wrapper(canv_occ_xz,
                                               _hoccupancy_xz,
                                               "EMR_occupancy_xz",
                                               "EMR xz occupancy");

  CanvasWrapper* cwrap_occ_yz =
	ReduceCppTools::get_canvas_emr_wrapper(canv_occ_yz,
                                               _hoccupancy_yz,
                                               "EMR_occupancy_yz",
                                               "EMR yz occupancy");

  CanvasWrapper* cwrap_beam_profile =
	ReduceCppTools::get_canvas_wrapper(canv_beam_profile,
                                           _hbeam_profile,
                                           "EMR_profile_xy",
                                           "EMR xy beam profile",
					   "COLZ");

  CanvasWrapper* cwrap_depth_profile =
	ReduceCppTools::get_canvas_wrapper(canv_depth_profile,
                                           _hdepth_profile,
                                           "EMR_profile_z",
                                           "EMR z depth profile");

  std::vector<TObject*> range_objs;
  range_objs.push_back(_hrange_primary);
  range_objs.push_back(_hrange_secondary);
  range_objs.push_back(_leg_range);
  range_objs.push_back(_text_match_eff);
  CanvasWrapper* cwrap_range =
	ReduceCppTools::get_canvas_multi_wrapper(canv_range,
                                                 range_objs,
                                                 "EMR_range",
                                                 "EMR range");

  std::vector<TObject*> total_charge_objs;
  total_charge_objs.push_back(_htotal_charge_ma);
  total_charge_objs.push_back(_htotal_charge_sa);
  total_charge_objs.push_back(_leg_charge);
  CanvasWrapper* cwrap_total_charge =
	ReduceCppTools::get_canvas_multi_wrapper(canv_total_charge,
                                   	         total_charge_objs,
                                               	 "EMR_total_charge",
                                                 "EMR total charge");

  std::vector<TObject*> charge_ratio_objs;
  charge_ratio_objs.push_back(_hcharge_ratio_ma);
  charge_ratio_objs.push_back(_hcharge_ratio_sa);
  charge_ratio_objs.push_back(_leg_charge);
  CanvasWrapper* cwrap_charge_ratio =
	ReduceCppTools::get_canvas_multi_wrapper(canv_charge_ratio,
                                   	         charge_ratio_objs,
                                               	 "EMR_charge_ratio",
                                                 "EMR charge ratio");

  std::vector<TObject*> chi2_density_objs;
  chi2_density_objs.push_back(_hchi2_density);
  chi2_density_objs.push_back(_text_pid_stats);
  chi2_density_objs.push_back(_line_density);
  chi2_density_objs.push_back(_line_chi2);
  CanvasWrapper* cwrap_chi2_density =
	ReduceCppTools::get_canvas_multi_wrapper(canv_chi2_density,
                                   	         chi2_density_objs,
                                               	 "EMR_chi2_density",
                                                 "EMR chi2 vs density",
						 "COLZ");

  this->reset();
  _output->GetImage()->CanvasWrappersPushBack(cwrap_occ_xz);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_occ_yz);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_beam_profile);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_depth_profile);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_range);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_total_charge);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_charge_ratio);
  _output->GetImage()->CanvasWrappersPushBack(cwrap_chi2_density);
}

void ReduceCppEMRPlot::_death() {}

void ReduceCppEMRPlot::_process(MAUS::Data* data) {

  if ( !data )
    throw Exception(Exception::recoverable, "Data was NULL",
                    "ReduceCppEMRPlot::_process");

  if ( !data->GetSpill() )
    throw Exception(Exception::recoverable, "Spill was NULL",
                    "ReduceCppEMRPlot::_process");

  std::string evType = data->GetSpill()->GetDaqEventType();
  if ( evType != "physics_event" )
     return;

  if ( !data->GetSpill()->GetReconEvents() )
     throw Exception(Exception::recoverable, "ReconEvents were NULL",
                        "ReduceCppEMRPlot::_process");

  int xRun = data->GetSpill()->GetRunNumber();
  int xSpill = data->GetSpill()->GetSpillNumber();
  _output->SetEventType(evType);
  _output->GetImage()->SetRunNumber(xRun);
  _output->GetImage()->SetSpillNumber(xSpill);

  ReconEventPArray* recEvts = data->GetSpill()->GetReconEvents();

  for (size_t i = 0;  i < recEvts->size(); ++i) {

    MAUS::EMREvent* evt = recEvts->at(i)->GetEMREvent();
    if ( !evt )
	continue;

    this->fill_emr_plots(evt);
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
    if (_hchi2_density->GetXaxis()->GetBinCenter(binx) < _density_cut)
	break;
    for (int biny = 1; biny < _hchi2_density->GetNbinsX()+1; biny++) {
      if (_hchi2_density->GetYaxis()->GetBinCenter(biny) > log10(1+_chi2_cut))
	  break;
      n_mu += _hchi2_density->GetBinContent(binx, biny);
    }
  }
  if ( _hchi2_density->GetEntries() )
      mu_fraction = static_cast<double>(n_mu)/_hchi2_density->GetEntries();
  TString pid_stats_string = "Mother frac.: "+TString::Format("%.3f", mu_fraction);
  _text_pid_stats->SetText(.95*_hchi2_density->GetXaxis()->GetXmax(),
			   .95*_hchi2_density->GetYaxis()->GetXmax(),
			   pid_stats_string);

  // Update the canvases
  for (size_t i = 0; i < _canvs.size(); i++) {
    _canvs[i]->cd()->Modified();
    _canvs[i]->Update();
  }
}

void ReduceCppEMRPlot::fill_emr_plots(EMREvent* evt) {

  // Skip events without a mother particle in the volume
  EMREventTrack* evtTrackMother = evt->GetMotherPtr();
  if ( !evtTrackMother )
      return;

  // OCCUPANCY PLOTS //
  // Loop over all the hits in the EMR fiducial volume
  for (size_t iPlane = 0; iPlane < evtTrackMother->GetEMRPlaneHitArray().size(); iPlane++) {
    EMRPlaneHit *plane = evtTrackMother->GetEMRPlaneHitArray()[iPlane];
    int xPlane = plane->GetPlane();
    int xCharge = plane->GetCharge();
    int xOri = plane->GetOrientation();
    if ( xCharge > _charge_threshold)
	_hdepth_profile->Fill(xPlane);

    EMRBarHitArray barHits = plane->GetEMRBarHitArray();
    for (size_t iHit = 0; iHit < barHits.size(); iHit++) {
      EMRBarHit barHit = barHits.at(iHit);
      int xBar = barHit.GetChannel()%_number_of_bars;
      int xTot = barHit.GetTot();
      if ( !xTot )
	  continue;

      // Fill triangular bins in the xz and yz occupancy plots
      if ( !xOri ) {
        _hoccupancy_xz->Fill(xPlane, xBar);
      } else {
        _hoccupancy_yz->Fill(xPlane, xBar);
      }
    }
  }
  if ( !evtTrackMother->GetEMRSpacePointArraySize() )
      return;

  // Fill the beam profile using the origin track point
  ThreeVector xOrigin = evtTrackMother->GetEMRTrack().GetOrigin();
  _hbeam_profile->Fill(xOrigin.x(), xOrigin.y());

  // PID VARIABLES PLOTS //
  // Fill the range histograms
  _hrange_primary->Fill(evtTrackMother->GetEMRTrack().GetRange());
  if ( evt->GetDaughterPtr() )
    _hrange_secondary->Fill(evt->GetDaughterPtr()->GetEMRTrack().GetRange());

  // Fill the charge plots
  _htotal_charge_ma->Fill(evtTrackMother->GetTotalChargeMA());
  _htotal_charge_sa->Fill(evtTrackMother->GetTotalChargeSA());

  _hcharge_ratio_ma->Fill(evtTrackMother->GetChargeRatioMA());
  _hcharge_ratio_sa->Fill(evtTrackMother->GetChargeRatioSA());

  // Fill the PID TGraph, the muons are located in the bottom right corner
  _hchi2_density->Fill(evtTrackMother->GetPlaneDensityMA(),
		       log10(1+evtTrackMother->GetEMRTrack().GetChi2()));
}
} // MAUS
