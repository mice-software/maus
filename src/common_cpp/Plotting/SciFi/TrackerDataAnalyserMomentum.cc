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
#include <cmath>

// ROOT headers
#include "TAxis.h"
#include "TVirtualPad.h"
#include "TStyle.h"
#include "TCut.h"
#include "TF1.h"
#include "TH1.h"
#include "TMath.h"
#include "Rtypes.h"

// MAUS headers
#include "src/common_cpp/Plotting/SciFi/TrackerDataAnalyserMomentum.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiStraightPRTrack.hh"
#include "src/common_cpp/DataStructure/SciFiHelicalPRTrack.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"
#include "src/common_cpp/Recon/SciFi/SimpleLine.hh"
#include "src/common_cpp/Recon/SciFi/SimpleCircle.hh"

namespace MAUS {

TrackerDataAnalyserMomentum::TrackerDataAnalyserMomentum()
  : _spill_num(0),
    _tracker_num(0),
    _charge(0),
    _num_points(0),
    _pt(0.0),
    _pz(0.0),
    _pt_mc(0.0),
    _pz_mc(0.0),
    _out_file(NULL),
    _tree(NULL),
    _t1_pt_res(NULL),
    _t1_pz_res(NULL),
    _t1_pz_res_log(NULL),
    _t2_pt_res(NULL),
    _t2_pz_res(NULL),
    _t2_pz_res_log(NULL),
    _t1_pt_res_pt(NULL),
    _t1_pz_res_pt(NULL),
    _t2_pt_res_pt(NULL),
    _t2_pz_res_pt(NULL),
    _t1_pz_resol(NULL),
    _t2_pz_resol(NULL),
    _cResiduals(NULL),
    _cGraphs(NULL),
    _cResolutions(NULL) {
  // Do nothing
}

TrackerDataAnalyserMomentum::~TrackerDataAnalyserMomentum() {
  if (_out_file) delete _out_file;
  if (_tree) delete _tree;
  if (_t1_pt_res) delete _t1_pt_res;
  if (_t1_pz_res) delete _t1_pz_res;
  if (_t1_pz_res_log) delete _t1_pz_res_log;
  if (_t2_pt_res) delete _t1_pt_res;
  if (_t2_pz_res) delete _t1_pz_res;
  if (_t2_pz_res_log) delete _t1_pz_res_log;
  if (_t1_pt_res_pt) delete _t1_pz_res_pt;
  if (_t1_pz_res_pt) delete _t1_pt_res_pt;
  if (_t2_pt_res_pt) delete _t2_pz_res_pt;
  if (_t2_pz_res_pt) delete _t2_pt_res_pt;
  if (_t1_pz_resol) delete _t1_pz_resol;
  if (_t2_pz_resol) delete _t2_pz_resol;
  if (_cResiduals) delete _cResiduals;
  if (_cGraphs) delete _cGraphs;
  if (_cResolutions) delete _cResolutions;
}

void TrackerDataAnalyserMomentum::setUp() {
  // Set up the output ROOT file
  _out_file = new TFile("momentum_analysis_output.root", "recreate");

  // Set up the output tree
  _tree = new TTree("tree", "Momentum Residual Data");
  _tree->Branch("spill_num", &_spill_num, "spill_num/I");
  _tree->Branch("tracker_num", &_tracker_num, "tracker_num/I");
  _tree->Branch("charge", &_charge, "charge/I");
  _tree->Branch("num_points", &_num_points, "num_points/I");
  _tree->Branch("pt", &_pt, "pt/D");
  _tree->Branch("pz", &_pz, "pz/D");
  _tree->Branch("pt_mc", &_pt_mc, "pt_mc/D");
  _tree->Branch("pz_mc", &_pz_mc, "pz_mc/D");

  // Set up the residual histograms
  int res_n_bins = 100;

  _t1_pt_res = new TH1D("t1_pt_res", "T1 p_{t} Residual", res_n_bins, -5, 5);
  _t1_pt_res->GetXaxis()->SetTitle("p_{T}^{MC} - p_{T} (MeV/c)");

  _t1_pz_res = new TH1D("t1_pz_res", "T1 p_{z} Residual", res_n_bins, -30, 30);
  _t1_pz_res->GetXaxis()->SetTitle("p_{z}^{MC} - p_{z} (MeV/c)");

  _t1_pz_res_log = new TH1D("t1_pz_res_log", "T1 p_{z} Residual", res_n_bins, -500, 500);
  _t1_pz_res_log->GetXaxis()->SetTitle("p_{z}^{MC} - p_{z} (MeV/c)");

  _t2_pt_res = new TH1D("t2_pt_res", "T2 p_{t} Residual", res_n_bins, -5, 5);
  _t2_pt_res->GetXaxis()->SetTitle("p_{T}^{MC} - p_{T} (MeV/c)");

  _t2_pz_res = new TH1D("t2_pz_res", "T2 p_{z} Residual", res_n_bins, -30, 30);
  _t2_pz_res->GetXaxis()->SetTitle("p_{z}^{MC} - p_{z} (MeV/c)");

  _t2_pz_res_log = new TH1D("t2_pz_res_log", "T2 p_{z} Residual", res_n_bins, -500, 500);
  _t2_pz_res_log->GetXaxis()->SetTitle("p_{z}^{MC} - p_{z} (MeV/c)");

  // Set up the residual graphs
  _t1_pz_res_pt = new TGraph();
  _t2_pz_res_pt = new TGraph();

  // Set global styles
  gStyle->SetOptStat(111111);
  gStyle->SetLabelSize(0.05, "XYZ");
  gStyle->SetTitleOffset(1.15, "X");
  gStyle->SetStatW(0.3);
  gStyle->SetStatH(0.2);
}

void TrackerDataAnalyserMomentum::accumulate(Spill* spill) {
  if (spill != NULL && spill->GetDaqEventType() == "physics_event") {
    _spill_num = spill->GetSpillNumber();
    // Loop over recon events in the spill
    for (size_t iRevt = 0; iRevt < spill->GetReconEvents()->size(); ++iRevt) {
      // MCEvent *mc_evt = (*spill->GetMCEvents())[iRevt];
      SciFiEvent *evt = (*spill->GetReconEvents())[iRevt]->GetSciFiEvent();
      std::vector<SciFiHelicalPRTrack*> htrks = evt->helicalprtracks();
      // Loop over helical pattern recognition tracks in event
      for (size_t iTrk = 0; iTrk < htrks.size(); ++iTrk) {
        SciFiHelicalPRTrack* trk = htrks[iTrk];
        if ((trk->get_R() != 0.0) & (trk->get_dsdz() != 0.0)) {
          _num_points = trk->get_num_points();
          _pt = 1.2 * trk->get_R();
          _pz = _pt / trk->get_dsdz();
          _pt_mc = 0.0;
          _pz_mc = 0.0;
          // Loop over seed spacepoints associated with the track,
          // and the clusters forming that,
          // to calculate  average MC truth momentum
          int num_spoint_skipped = 0;
          for (size_t iSp = 0; iSp < trk->get_spacepoints().size(); ++iSp) {
            SciFiSpacePoint* seed = trk->get_spacepoints()[iSp];
            double seed_px_mc = 0.0;
            double seed_py_mc = 0.0;
            double seed_pz_mc = 0.0;
            int num_clus_skipped = 0;
            for (size_t iCl = 0; iCl < seed->get_channels().size(); ++iCl) {
              ThreeVector p_mc = seed->get_channels()[iCl]->get_true_momentum();
              if ( fabs(p_mc.z()) > _pz_mc_cut ) { // Cut to remove bad clusters from the analysis
                seed_px_mc += p_mc.x();
                seed_py_mc += p_mc.y();
                seed_pz_mc += p_mc.z();
              } else {
                std::cerr << "Low cluster MC pz of " << p_mc.z() << ", skipping cluster\n";
                ++num_clus_skipped;
              }
            }
            if ((seed->get_channels().size() - num_clus_skipped) > 0) {
              seed_px_mc /= (seed->get_channels().size() - num_clus_skipped);
              seed_py_mc /= (seed->get_channels().size() - num_clus_skipped);
              seed_pz_mc /= (seed->get_channels().size() - num_clus_skipped);
              _pt_mc += sqrt(seed_px_mc*seed_px_mc + seed_py_mc*seed_py_mc);
              _pz_mc += seed_pz_mc;
            } else {
              std::cerr << "Spoint with " << num_clus_skipped << " bad MC cluster momenta found,";
              std::cerr << " skipping spoint\n";
              ++num_spoint_skipped;
              continue;
            }
          }
          _pt_mc /= (trk->get_spacepoints().size() - num_spoint_skipped);
          _pz_mc /= (trk->get_spacepoints().size() - num_spoint_skipped);
          if ( TMath::IsNaN(_pt_mc) || TMath::IsNaN(_pz_mc) ) {
            std::cerr << "Track with bad MC momenta found, skipping track\n";
            continue;
          }
          // Fill the vectors and tree with the extracted recon and MC momenta
          if (trk->get_tracker() == 0) {
            _t1_pt_res->Fill(_pt_mc - _pt);
            _t1_pz_res->Fill(_pz_mc + trk->get_charge()*_pz);
            _t1_pz_res_log->Fill(_pz_mc + trk->get_charge()*_pz);
            _t1_vPtMc.push_back(_pt_mc);
            _t1_vPt_res.push_back(_pt_mc - _pt);
            _t1_vPz.push_back(_pz);
            _t1_vPz_res.push_back(_pz_mc + trk->get_charge()*_pz);
          } else if (trk->get_tracker() == 1) {
            _t2_pt_res->Fill(_pt_mc - _pt);
            _t2_pz_res->Fill(_pz_mc - trk->get_charge()*_pz);
            _t2_pz_res_log->Fill(_pz_mc - trk->get_charge()*_pz);
            _t2_vPtMc.push_back(_pt_mc);
            _t2_vPt_res.push_back(_pt_mc - _pt);
            _t2_vPz.push_back(_pz);
            _t2_vPz_res.push_back(_pz_mc - trk->get_charge()*_pz);
          }
          _tracker_num = trk->get_tracker();
          _charge = trk->get_charge();
          _tree->Fill();
        } else {
          std::cout << "Bad track, skipping" << std::endl;
        }
      }
    }
  } else {
    std::cout << "Not a usable spill" << std::endl;
  }
}

void TrackerDataAnalyserMomentum::make_all() {
  make_residual_histograms();
  make_residual_graphs();
  make_pz_resolutions();
  make_resolution_graphs();
}

void TrackerDataAnalyserMomentum::make_pz_resolutions() {
  int nPoints = 10;                        // The number of MC momentum intervals used in the plots
  std::vector<TCut> vCuts(nPoints);        // The cuts defining the pt_mc intervals
  std::vector<double> vPtMc(nPoints);        // The centre of the pt_mc intervals
  std::vector<double> vPtMcErr(nPoints);     // The width of the intervals
  std::vector<double> vPzRes_t1(nPoints);    // The resulatant pz resolutions
  std::vector<double> vPzResErr_t1(nPoints); // The errors assocaited with the pz res
  std::vector<double> vPzRes_t2(nPoints);    // The resulatant pz resolutions
  std::vector<double> vPzResErr_t2(nPoints); // The errors assocaited with the pz res

  // Cuts in pt_mc
  vCuts[0] = "pt_mc>=0&&pt_mc<10";
  vCuts[1] = "pt_mc>=10&&pt_mc<20";
  vCuts[2] = "pt_mc>=20&&pt_mc<30";
  vCuts[3] = "pt_mc>=30&&pt_mc<40";
  vCuts[4] = "pt_mc>=40&&pt_mc<50";
  vCuts[5] = "pt_mc>=50&&pt_mc<60";
  vCuts[6] = "pt_mc>=60&&pt_mc<70";
  vCuts[7] = "pt_mc>=70&&pt_mc<80";
  vCuts[8] = "pt_mc>=80&&pt_mc<90";
  vCuts[9] = "pt_mc>=90&&pt_mc<100";

  // The central MC momentum
  vPtMc[0] = 5.0;
  vPtMc[1] = 15.0;
  vPtMc[2] = 25.0;
  vPtMc[3] = 35.0;
  vPtMc[4] = 45.0;
  vPtMc[5] = 55.0;
  vPtMc[6] = 65.0;
  vPtMc[7] = 75.0;
  vPtMc[8] = 85.0;
  vPtMc[9] = 95.0;

  // The error associated with the MC momentum (just the interval half width)
  vPtMcErr[0] = 5.0;
  vPtMcErr[1] = 5.0;
  vPtMcErr[2] = 5.0;
  vPtMcErr[3] = 5.0;
  vPtMcErr[4] = 5.0;
  vPtMcErr[5] = 5.0;
  vPtMcErr[6] = 5.0;
  vPtMcErr[7] = 5.0;
  vPtMcErr[8] = 5.0;
  vPtMcErr[9] = 5.0;

  // Cuts for to select each tracker
  TCut cutT1 = "tracker_num==0";
  TCut cutT2 = "tracker_num==1";

  // Loop over the mometum intervals and calculate the resolution for each
  for (int i = 0; i < nPoints; ++i) {
    TCut input_cut = vCuts[i]&&cutT1;
    calc_pz_resolution(0, input_cut, vPzRes_t1[i], vPzResErr_t1[i]);
  }
  for (int i = 0; i < nPoints; ++i) {
    TCut input_cut = vCuts[i]&&cutT2;
    calc_pz_resolution(1, input_cut, vPzRes_t2[i], vPzResErr_t2[i]);
  }

  // Create the resultant resolution plots
  _t1_pz_resol = new TGraphErrors(nPoints, &vPtMc[0], &vPzRes_t1[0],
                                  &vPtMcErr[0], &vPzResErr_t1[0]);
  _t2_pz_resol = new TGraphErrors(nPoints, &vPtMc[0], &vPzRes_t2[0],
                                  &vPtMcErr[0], &vPzResErr_t2[0]);
}

void TrackerDataAnalyserMomentum::make_residual_histograms() {
  _cResiduals = new TCanvas("cResiduals", "Mometum Residuals");
  _cResiduals->Divide(3, 2);
  _cResiduals->cd(1);
  _t1_pt_res->UseCurrentStyle();
  _t1_pt_res->Draw();
  _cResiduals->cd(2);
  _t1_pz_res->UseCurrentStyle();
  _t1_pz_res->Draw();
  TVirtualPad* pad = _cResiduals->cd(3);
  pad->SetLogy();
  _t1_pz_res_log->UseCurrentStyle();
  _t1_pz_res_log->Draw();
  _cResiduals->cd(4);
  _t2_pt_res->UseCurrentStyle();
  _t2_pt_res->Draw();
  _cResiduals->cd(5);
  _t2_pz_res->UseCurrentStyle();
  _t2_pz_res->Draw();
  pad = _cResiduals->cd(6);
  pad->SetLogy();
  _t2_pz_res_log->UseCurrentStyle();
  _t2_pz_res_log->Draw();
}

void TrackerDataAnalyserMomentum::make_residual_graphs() {
  _cGraphs = new TCanvas("cGraphs", "Mometum Graphs");
  _cGraphs->Divide(2, 2);

  _cGraphs->cd(1);
  _t1_pt_res_pt = new TGraph(_t1_vPtMc.size(), &_t1_vPtMc[0], &_t1_vPt_res[0]);
  _t1_pt_res_pt->SetName("t1_pt_res_pt");
  _t1_pt_res_pt->SetTitle("T1 p_{t} res vs. p_{t}^{MC}");
  _t1_pt_res_pt->GetXaxis()->SetTitle("p_{t}^{MC} (MeV/c)");
  _t1_pt_res_pt->GetYaxis()->SetTitle("p_{t}^{MC} - p_{t} (MeV/c)");
  _t1_pt_res_pt->GetYaxis()->SetTitleOffset(1.4);
  _t1_pt_res_pt->Draw("AP");

  _cGraphs->cd(2);
  _t1_pz_res_pt = new TGraph(_t1_vPtMc.size(), &_t1_vPtMc[0], &_t1_vPz_res[0]);
  _t1_pz_res_pt->SetName("t1_pz_res_pt");
  _t1_pz_res_pt->SetTitle("T1 p_{z} res vs. p_{t}^{MC}");
  _t1_pz_res_pt->GetXaxis()->SetTitle("p_{t}^{MC} (MeV/c)");
  _t1_pz_res_pt->GetYaxis()->SetTitle("p_{z}^{MC} - p_{z} (MeV/c)");
  _t1_pz_res_pt->GetYaxis()->SetTitleOffset(1.4);
  _t1_pz_res_pt->GetYaxis()->SetRangeUser(-200, 200);
  _t1_pz_res_pt->Draw("AP");

  _cGraphs->cd(3);
  _t2_pt_res_pt = new TGraph(_t2_vPtMc.size(), &_t2_vPtMc[0], &_t2_vPt_res[0]);
  _t2_pt_res_pt->SetName("t2_pt_res_pt");
  _t2_pt_res_pt->SetTitle("T2 p_{t} res vs. p_{t}^{MC}");
  _t2_pt_res_pt->GetXaxis()->SetTitle("p_{t}^{MC} (MeV/c)");
  _t2_pt_res_pt->GetYaxis()->SetTitle("p_{t}^{MC} - p_{t} (MeV/c)");
  _t2_pt_res_pt->GetYaxis()->SetTitleOffset(1.4);
  _t2_pt_res_pt->Draw("AP");

  _cGraphs->cd(4);
  _t2_pz_res_pt = new TGraph(_t2_vPtMc.size(), &_t2_vPtMc[0], &_t2_vPz_res[0]);
  _t2_pz_res_pt->SetName("t2_pz_res_pt");
  _t2_pz_res_pt->SetTitle("T2 p_{z} res vs. p_{t}^{MC}");
  _t2_pz_res_pt->GetXaxis()->SetTitle("p_{t}^{MC} (MeV/c)");
  _t2_pz_res_pt->GetYaxis()->SetTitle("p_{z}^{MC} - p_{z} (MeV/c)");
  _t2_pz_res_pt->GetYaxis()->SetTitleOffset(1.4);
  _t2_pz_res_pt->GetYaxis()->SetRangeUser(-200, 200);
  _t2_pz_res_pt->Draw("AP");
}

void TrackerDataAnalyserMomentum::make_resolution_graphs() {
  _cResolutions = new TCanvas("cResolutions", "Mometum Resolution Graphs");
  _cResolutions->Divide(2);

  _cResolutions->cd(1);
  _t1_pz_resol->SetName("t1_pz_resol");
  _t1_pz_resol->SetTitle("T1 p_{z} Resolution");
  _t1_pz_resol->GetXaxis()->SetTitle("p_{t}^{MC} (MeV/c)");
  _t1_pz_resol->GetYaxis()->SetTitle("p_{z} Resolution (MeV/c)");
  _t1_pz_resol->Draw("AP");

  _cResolutions->cd(2);
  _t2_pz_resol->SetName("t2_pz_resol");
  _t2_pz_resol->SetTitle("T2 p_{z} Resolution");
  _t2_pz_resol->GetXaxis()->SetTitle("p_{t}^{MC} (MeV/c)");
  _t2_pz_resol->GetYaxis()->SetTitle("p_{z} Resolution (MeV/c)");
  _t2_pz_resol->Draw("AP");
}

void TrackerDataAnalyserMomentum::calc_pz_resolution(const int trker, const TCut cut,
                                                     double &res, double &res_err) {
  if (_tree) {
    TCanvas lCanvas;
    // Create a histogram of the pz residual for the pt_mc interval defined by the input cut
    if (trker == 0) {
      _tree->Draw("pz_mc-pz>>h1", cut);
    } else if (trker == 1) {
      _tree->Draw("pz_mc+pz>>h1", cut);
    }
    // Pull the histogram from the current pad
    TH1 *h1 = reinterpret_cast<TH1*>(gPad->GetPrimitive("h1"));
    // Fit a gaussian to the histogram
    h1->Fit("gaus");
    TF1 *fit1 = h1->GetFunction("gaus");
    // Extract the sigma of the gaussian fit (equivalent to the pz resolution for this interval)
    res = fit1->GetParameter(2);
    res_err = fit1->GetParError(2);
    // Scale gets messed up unless histogram is deleted each time
    delete h1;
  } else {
    std::cerr << "Tree pointer invalid" << std::endl;
  }
}

bool TrackerDataAnalyserMomentum::save_graphics(std::string save_type) {
  if ( (save_type == "eps") || (save_type == "pdf") || (save_type == "png") ) {
    if (_cResiduals) {
      std::string save_name = "mom_res_histos.";
      save_name += save_type;
      _cResiduals->Print(save_name.c_str());
    }
    if (_cGraphs) {
      std::string save_name = "mom_residual_graphs.";
      save_name += save_type;
      _cGraphs->Print(save_name.c_str());
    }
    if (_cResolutions) {
      std::string save_name = "mom_resolution_graphs.";
      save_name += save_type;
      _cResolutions->Print(save_name.c_str());
    }
  } else {
    std::cerr << "Invalid graphics output type given\n";
    return false;
  }
  return true;
}

void TrackerDataAnalyserMomentum::save_root() {
  if (_out_file) {
    _out_file->cd();
    if (_tree) _tree->Write();

    if (_t1_pt_res) _t1_pt_res->Write();
    if (_t1_pz_res) _t1_pz_res->Write();
    if (_t1_pz_res_log) _t1_pz_res_log->Write();
    if (_t1_pt_res_pt) _t1_pt_res_pt->Write();
    if (_t1_pz_res_pt) _t1_pz_res_pt->Write();

    if (_t2_pt_res) _t2_pt_res->Write();
    if (_t2_pz_res) _t2_pz_res->Write();
    if (_t2_pz_res_log) _t2_pz_res_log->Write();
    if (_t2_pt_res_pt) _t2_pt_res_pt->Write();
    if (_t2_pz_res_pt) _t2_pz_res_pt->Write();

    if (_t1_pz_resol) _t1_pz_resol->Write();
    if (_t2_pz_resol) _t2_pz_resol->Write();

    if (_cResiduals) _cResiduals->Write();
    if (_cGraphs) _cGraphs->Write();
    if (_cResolutions) _cResolutions->Write();

    _out_file->Write();
    _out_file->Close();
  } else {
    std::cerr << "Invalid ROOT file pointer provided" << std::endl;
  }
}

} // ~namespace MAUS
