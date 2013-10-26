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

// MAUS headers
#include "src/common_cpp/Recon/SciFi/TrackerDataAnalyserMomentum.hh"
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
  : _t1_pt_res(NULL),
    _t1_pz_res(NULL),
    _t1_pz_res_log(NULL),
    _t1_pz_res_pt(NULL),
    _t2_pt_res(NULL),
    _t2_pz_res(NULL),
    _t2_pz_res_log(NULL),
    _t2_pz_res_pt(NULL),
    _cResiduals(NULL),
    _cGraphs(NULL) {
  // Do nothing
}

TrackerDataAnalyserMomentum::~TrackerDataAnalyserMomentum() {
  if (_t1_pt_res) delete _t1_pt_res;
  if (_t1_pz_res) delete _t1_pz_res;
  if (_t1_pz_res_log) delete _t1_pz_res_log;
  if (_t1_pz_res_pt) delete _t1_pz_res_pt;
  if (_t2_pt_res) delete _t1_pt_res;
  if (_t2_pz_res) delete _t1_pz_res;
  if (_t2_pz_res_log) delete _t1_pz_res_log;
  if (_t2_pz_res_pt) delete _t2_pz_res_pt;
  if (_cResiduals) delete _cResiduals;
  if (_cGraphs) delete _cGraphs;
}

void TrackerDataAnalyserMomentum::setUp() {
  int res_n_bins = 100;
  _t1_pt_res = new TH1D("t1_pt_res", "T1 p_{t} Residual", res_n_bins, -5, 5);
  _t1_pz_res = new TH1D("t1_pz_res", "T1 p_{z} Residual", res_n_bins, -30, 30);
  _t1_pz_res_log = new TH1D("t1_pz_res_log", "T1 p_{z} Residual", res_n_bins, -500, 500);
  _t1_pz_res_pt = new TGraph();
  _t2_pt_res = new TH1D("t2_pt_res", "T2 p_{t} Residual", res_n_bins, -5, 5);
  _t2_pz_res = new TH1D("t2_pz_res", "T2 p_{z} Residual", res_n_bins, -30, 30);
  _t2_pz_res_log = new TH1D("t2_pz_res_log", "T2 p_{z} Residual", res_n_bins, -500, 500);
  _t2_pz_res_pt = new TGraph();

  _t1_pt_res->GetXaxis()->SetTitle("p_{T}^{MC} - p_{T} (MeV/c)");
  _t1_pz_res->GetXaxis()->SetTitle("p_{z}^{MC} - p_{z} (MeV/c)");
  _t1_pz_res_log->GetXaxis()->SetTitle("p_{z}^{MC} - p_{z} (MeV/c)");
  _t2_pt_res->GetXaxis()->SetTitle("p_{T}^{MC} - p_{T} (MeV/c)");
  _t2_pz_res->GetXaxis()->SetTitle("p_{z}^{MC} - p_{z} (MeV/c)");
  _t2_pz_res_log->GetXaxis()->SetTitle("p_{z}^{MC} - p_{z} (MeV/c)");
}

void TrackerDataAnalyserMomentum::accumulate(Spill* spill) {
  if (spill != NULL && spill->GetDaqEventType() == "physics_event") {
    for (size_t iRevt = 0; iRevt < spill->GetReconEvents()->size(); ++iRevt) {
      SciFiEvent *evt = (*spill->GetReconEvents())[iRevt]->GetSciFiEvent();
      std::vector<SciFiHelicalPRTrack*> htrks = evt->helicalprtracks();
      for (size_t iTrk = 0; iTrk < htrks.size(); ++iTrk) {
        SciFiHelicalPRTrack* trk = htrks[iTrk];
        if ((trk->get_R() != 0.0) & (trk->get_dsdz() != 0.0)) {
          double pt = 1.2 * trk->get_R();
          double pz = pt / trk->get_dsdz();
          double pt_mc = 0.0;
          double pz_mc = 0.0;
          for (size_t iSp = 0; iSp < trk->get_spacepoints().size(); ++iSp) {
            SciFiSpacePoint* seed = trk->get_spacepoints()[iSp];
            double seed_px_mc = 0.0;
            double seed_py_mc = 0.0;
            double seed_pz_mc = 0.0;
            for (size_t iCl = 0; iCl < seed->get_channels().size(); ++iCl) {
              ThreeVector p_mc = seed->get_channels()[iCl]->get_true_momentum();
              seed_px_mc += p_mc.x();
              seed_py_mc += p_mc.y();
              seed_pz_mc += p_mc.z();
            }
            seed_px_mc /= seed->get_channels().size();
            seed_py_mc /= seed->get_channels().size();
            seed_pz_mc /= seed->get_channels().size();
            pt_mc += sqrt(seed_px_mc*seed_px_mc + seed_py_mc*seed_py_mc);
            pz_mc += seed_pz_mc;
          }
          pt_mc /= trk->get_spacepoints().size();
          pz_mc /= trk->get_spacepoints().size();
          if (trk->get_tracker() == 0) {
            _t1_pt_res->Fill(pt_mc - pt);
            _t1_pz_res->Fill(pz_mc + trk->get_charge()*pz);
            _t1_pz_res_log->Fill(pz_mc + trk->get_charge()*pz);
            _t1_vPt.push_back(pt);
            _t1_vPt_res.push_back(pt_mc - pt);
            _t1_vPz.push_back(pz);
            _t1_vPz_res.push_back(pz_mc + trk->get_charge()*pz);
          } else if (trk->get_tracker() == 1) {
            _t2_pt_res->Fill(pt_mc - pt);
            _t2_pz_res->Fill(pz_mc - trk->get_charge()*pz);
            _t2_pz_res_log->Fill(pz_mc - trk->get_charge()*pz);
            _t2_vPt.push_back(pt);
            _t2_vPt_res.push_back(pt_mc - pt);
            _t2_vPz.push_back(pz);
            _t2_vPz_res.push_back(pz_mc - trk->get_charge()*pz);
          }
        } else {
          std::cout << "Bad track, skipping" << std::endl;
        }
      }
    }
  } else {
    std::cout << "Not a usable spill" << std::endl;
  }
}

void TrackerDataAnalyserMomentum::analyse(bool show) {
  // Draw the histos
  gStyle->SetOptStat(111111);
  _cResiduals = new TCanvas("cResiduals", "Mometum Residuals");
  _cResiduals->Divide(3, 2);
  _cResiduals->cd(1);
  _t1_pt_res->Draw();
  _cResiduals->cd(2);
  _t1_pz_res->Draw();
  TVirtualPad* pad = _cResiduals->cd(3);
  pad->SetLogy();
  _t1_pz_res_log->Draw();
  _cResiduals->cd(4);
  _t2_pt_res->Draw();
  _cResiduals->cd(5);
  _t2_pz_res->Draw();
  pad = _cResiduals->cd(6);
  pad->SetLogy();
  _t2_pz_res_log->Draw();

  // Draw the graphs
  _cGraphs = new TCanvas("cGraphs", "Mometum Graphs");
  _cGraphs->Divide(2);

  _cGraphs->cd(1);
  _t1_pz_res_pt = new TGraph(_t1_vPt.size(), &_t1_vPt[0], &_t1_vPz_res[0]);
  _t1_pz_res_pt->SetTitle("T1 p_{z} res vs. p_{t}");
  _t1_pz_res_pt->GetXaxis()->SetTitle("p_{t} (MeV/c)");
  _t1_pz_res_pt->GetYaxis()->SetTitle("p_{z}^{MC} - p_{z} (MeV/c)");
  _t1_pz_res_pt->GetYaxis()->SetTitleOffset(1.4);
  _t1_pz_res_pt->Draw("AP");

  _t2_pz_res_pt = new TGraph(_t2_vPt.size(), &_t2_vPt[0], &_t2_vPz_res[0]);
  _t2_pz_res_pt->SetTitle("T2 p_{z} res vs. p_{t}");
  _t2_pz_res_pt->GetXaxis()->SetTitle("p_{t} (MeV/c)");
  _t2_pz_res_pt->GetYaxis()->SetTitle("p_{z}^{MC} - p_{z} (MeV/c)");
  _t2_pz_res_pt->GetYaxis()->SetTitleOffset(1.4);
  _cGraphs->cd(2);
  _t2_pz_res_pt->Draw("AP");
}

bool TrackerDataAnalyserMomentum::save(std::string save_type) {
  if ( (save_type == "eps") || (save_type == "pdf") || (save_type == "png") ) {
    if (_cResiduals) {
      std::string save_name = "mom_res_histos.";
      save_name += save_type;
      _cResiduals->Print(save_name.c_str());
    }
    if (_cGraphs) {
      std::string save_name = "mom_res_graphs.";
      save_name += save_type;
      _cResiduals->Print(save_name.c_str());
    }
  } else {
    std::cerr << "Invalid graphics output type given\n";
    return false;
  }
  return true;
}

} // ~namespace MAUS
