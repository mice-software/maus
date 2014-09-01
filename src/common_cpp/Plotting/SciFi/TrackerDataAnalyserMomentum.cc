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
#include <map>
#include <sstream>

// ROOT headers
#include "TAxis.h"
#include "TVirtualPad.h"
#include "TStyle.h"
#include "TF1.h"
#include "TH1.h"
#include "TH1D.h"
#include "TMath.h"
#include "Rtypes.h"
#include "TRefArray.h"
#include "TRef.h"

// MAUS headers
#include "src/common_cpp/Plotting/SciFi/TrackerDataAnalyserMomentum.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/DataStructure/MCEvent.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
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
    _n_bad_tracks(0),
    _mc_track_id(0),
    _mc_pid(0),
    _n_matched(0),
    _n_mismatched(0),
    _n_missed(0),
    _pt_rec(0.0),
    _pz_rec(0.0),
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
    _resol_histos_t1(NULL),
    _resol_histos_t2(NULL),
    _cResiduals(NULL),
    _cGraphs(NULL),
    _cResolutions(NULL),
    _n_bins(200),
    _n_pz_points(9),
    _pz_fit_min(-50),
    _pz_fit_max(50),
    _pz_lower_bound(0.0),
    _pz_upper_bound(90.0),
    _cutPzRec(0.0) {
  // Do nothing
}

TrackerDataAnalyserMomentum::~TrackerDataAnalyserMomentum() {
  if (_out_file) delete _out_file;
  if (_tree) delete _tree;
  if (_t1_pt_res) delete _t1_pt_res;
  if (_t1_pz_res) delete _t1_pz_res;
  if (_t1_pz_res_log) delete _t1_pz_res_log;
  if (_t2_pt_res) delete _t2_pt_res;
  if (_t2_pz_res) delete _t2_pz_res;
  if (_t2_pz_res_log) delete _t2_pz_res_log;
  if (_t1_pt_res_pt) delete _t1_pz_res_pt;
  if (_t1_pz_res_pt) delete _t1_pt_res_pt;
  if (_t2_pt_res_pt) delete _t2_pz_res_pt;
  if (_t2_pz_res_pt) delete _t2_pt_res_pt;
  if (_t1_pz_resol) delete _t1_pz_resol;
  if (_t2_pz_resol) delete _t2_pz_resol;
  if (_resol_histos_t1) delete _resol_histos_t1;
  if (_resol_histos_t2) delete _resol_histos_t2;
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
  _tree->Branch("n_bad_tracks", &_n_bad_tracks, "n_bad_tracks/I");
  _tree->Branch("mc_track_id", &_mc_track_id, "mc_track_id/I");
  _tree->Branch("mc_pid", &_mc_pid, "mc_pid/I");
  _tree->Branch("n_matched", &_n_matched, "n_matched/I");
  _tree->Branch("n_mismatched", &_n_mismatched, "n_mismatched/I");
  _tree->Branch("n_missed", &_n_missed, "n_missed/I");
  _tree->Branch("pt_rec", &_pt_rec, "pt_rec/D");
  _tree->Branch("pz_rec", &_pz_rec, "pz_rec/D");
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

  _resol_histos_t1 = new TObjArray(0);
  _resol_histos_t2 = new TObjArray(0);
  _resol_histos_t1->SetOwner(kTRUE);
  _resol_histos_t2->SetOwner(kTRUE);

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

    // Loop over MC events in the spill
    for (size_t iMevt = 0; iMevt < spill->GetMCEvents()->size(); ++iMevt) {
      MCEvent *mc_evt = (*spill->GetMCEvents())[iMevt];
      SciFiEvent *evt = (*spill->GetReconEvents())[iMevt]->GetSciFiEvent();
      calc_pat_rec_efficiency(mc_evt, evt);
    }
  } else {
    std::cout << "Not a usable spill" << std::endl;
  }
}

void TrackerDataAnalyserMomentum::calc_pat_rec_efficiency(MCEvent *mc_evt, SciFiEvent* evt) {
  std::vector<SciFiHelicalPRTrack*> htrks = evt->helicalprtracks();

  // Create the lookup bridge between MC and Recon
  SciFiLookup lkup;
  lkup.make_hits_map(mc_evt);

  // Reset tracks counters
  _n_bad_tracks = 0;

  // Loop over helical pattern recognition tracks in event
  for (size_t iTrk = 0; iTrk < htrks.size(); ++iTrk) {
    SciFiHelicalPRTrack* trk = htrks[iTrk];
    _pt_rec = 0.0;
    _pz_rec = 0.0;
    if ((trk->get_R() != 0.0) & (trk->get_dsdz() != 0.0)) {
      _num_points = trk->get_num_points();
      std::vector< std::vector<int> > spoint_mc_tids;  // Vector of MC track ids for each spoint

      // Calc recon momentum
      _pt_rec = 1.2 * trk->get_R();
      _pz_rec = _pt_rec / trk->get_dsdz();

      // Calc MC momentum
      // Loop over seed spacepoints associated with the track
      std::vector<SciFiSpacePoint*> spnts = trk->get_spacepoints_pointers();
      for ( size_t k = 0; k < spnts.size(); ++k ) {
        std::map<int, int> track_id_counter;  // Map of track id to freq for each spoint
        std::vector<SciFiCluster*> clusters = spnts[k]->get_channels_pointers();

        // Loop over clusters
        for ( size_t l = 0; l < clusters.size(); ++l ) {
          std::vector<SciFiDigit*> digits = clusters[l]->get_digits_pointers();

          // Loop over digits
          for ( size_t m = 0; m < digits.size(); ++m ) {
            // Perform the digits to hits lookup
            std::vector<SciFiHit*> hits;
            if (!lkup.get_hits(digits[m], hits)) {
              std::cerr << "Lookup failed\n";
              continue;
            }

            // Loop over MC hits
            for ( size_t n = 0; n < hits.size(); ++n ) {
              int track_id = hits[n]->GetTrackId();
              if (track_id_counter.count(track_id))
                track_id_counter[track_id] = track_id_counter[track_id] + 1;
              else
                track_id_counter[track_id] = 1;
            } // ~Loop over MC hits
          } // ~Loop over digits
        } // ~// Loop over clusters

        // Find the track_ids for this spacepoint
        std::map<int, int>::iterator mit1;
        std::vector<int> mc_tracks_ids;
        for ( mit1 = track_id_counter.begin(); mit1 != track_id_counter.end(); ++mit1 ) {
          mc_tracks_ids.push_back(mit1->first);
        }
        spoint_mc_tids.push_back(mc_tracks_ids);
      } // ~Loop over seed spacepoints

      // Is there a track id associated with 3 or more spoints?
      int track_id = 0;
      int counter = 0;
      bool success = find_mc_tid(spoint_mc_tids, track_id, counter);
      // If we have not found a common track id, abort for this track
      if (!success) {
        std::cerr << "Malformed track, skipping\n";
        ++_n_bad_tracks;
        break;
      }
      // If we have found a common track id amoung the spoints, fill the tree
      _mc_track_id = track_id;
      _n_matched = counter;
      _n_mismatched = spoint_mc_tids.size() - counter;
      _n_missed = 5 - counter; // TODO: improve

      // Calc the MC track momentum using hits only with this track id
      _pt_mc = 0.0;
      _pz_mc = 0.0;
      int counter2 = 0;
      for ( size_t k = 0; k < spnts.size(); ++k ) {
        ThreeVector mom_mc;
        bool success = find_mc_spoint_momentum(track_id, spnts[k], lkup, mom_mc);
        if (!success) continue;
        _pt_mc += sqrt(mom_mc.x()*mom_mc.x() + mom_mc.y()*mom_mc.y());
        _pz_mc += mom_mc.z();
        ++counter2;
      }
      _pt_mc /= counter2;
      _pz_mc /= counter2;

      // Fill the vectors and tree with the extracted recon and MC momenta
      if (trk->get_tracker() == 0) {
        _t1_pt_res->Fill(_pt_mc - _pt_rec);
        _t1_pz_res->Fill(_pz_mc + trk->get_charge()*_pz_rec);
        _t1_pz_res_log->Fill(_pz_mc + trk->get_charge()*_pz_rec);
        _vec_t1_pt_mc.push_back(_pt_mc);
        _vec_t1_pt_res.push_back(_pt_mc - _pt_rec);
        _vec_t1_pz.push_back(_pz_rec);
        _vec_t1_pz_res.push_back(_pz_mc + trk->get_charge()*_pz_rec);
      } else if (trk->get_tracker() == 1) {
        _t2_pt_res->Fill(_pt_mc - _pt_rec);
        _t2_pz_res->Fill(_pz_mc - trk->get_charge()*_pz_rec);
        _t2_pz_res_log->Fill(_pz_mc - trk->get_charge()*_pz_rec);
        _vec_t2_ptMc.push_back(_pt_mc);
        _vec_t2_pt_res.push_back(_pt_mc - _pt_rec);
        _vec_t2_pz.push_back(_pz_rec);
        _vec_t2_pz_res.push_back(_pz_mc - trk->get_charge()*_pz_rec);
      }
      _tracker_num = trk->get_tracker();
      _charge = trk->get_charge();
      _tree->Fill();
    } else {
      std::cout << "Bad track, skipping" << std::endl;
    }
  }
}

bool TrackerDataAnalyserMomentum::find_mc_spoint_momentum(const int track_id,
                const SciFiSpacePoint* sp, SciFiLookup &lkup, ThreeVector &mom) {

  std::vector<SciFiCluster*> clusters = sp->get_channels_pointers();
  std::vector<SciFiCluster*>::iterator clus_it;
  for (clus_it = clusters.begin(); clus_it != clusters.end(); ++clus_it) {
    std::vector<SciFiDigit*> digits = (*clus_it)->get_digits_pointers();
    std::vector<SciFiDigit*>::iterator dig_it;
    for (dig_it = digits.begin(); dig_it != digits.end(); ++dig_it) {
      std::vector<SciFiHit*> hits;
      if (!lkup.get_hits((*dig_it), hits)) {
        std::cerr << "Lookup failed\n";
        continue;
      }
      std::vector<SciFiHit*>::iterator hit_it;
      for (hit_it = hits.begin(); hit_it != hits.end(); ++hit_it) {
        if (track_id == (*hit_it)->GetTrackId()) {
          mom = (*hit_it)->GetMomentum();
          return true;
        }
      }
    }
  }
  return false;
};

bool TrackerDataAnalyserMomentum::find_mc_tid(const std::vector< std::vector<int> > &spoint_mc_tids,
                                              int &tid, int &counter) {

  std::map<int, int> track_id_counter;  // Map of track id to freq for each spoint

  for ( size_t i = 0; i < spoint_mc_tids.size(); ++i ) {
    for ( size_t j = 0; j < spoint_mc_tids[i].size(); ++j ) {
      int current_tid = spoint_mc_tids[i][j];
      if (track_id_counter.count(current_tid))
        track_id_counter[current_tid] = track_id_counter[current_tid] + 1;
      else
        track_id_counter[current_tid] = 1;
    }
  }

  std::map<int, int>::iterator mit1;
  tid = 0;
  for ( mit1 = track_id_counter.begin(); mit1 != track_id_counter.end(); ++mit1 ) {
    if ( mit1->second > 2 && tid == 0 ) {
      tid = mit1->first;
      counter = mit1->second;
    } else if ( mit1->second > 2 && tid != 0 ) {
      tid = -1;  // A malformed track, cannot asscoiate with an mc track id
      counter = 0;
      return false;
    }
  }
  return true;
};

void TrackerDataAnalyserMomentum::make_all() {
  make_residual_histograms();
  make_residual_graphs();
  make_pz_resolutions();
  make_resolution_graphs();
}

void TrackerDataAnalyserMomentum::make_pz_resolutions() {
  double pz_range = _pz_upper_bound - _pz_lower_bound;  // The range of the pz resolution graph
  double resolution_error =  pz_range / ( _n_pz_points * 2 );  // Error in pt_mc of each data point
  // The mid pt_mc value of the first data point
  double first_resolution_point = _pz_lower_bound + resolution_error;
  std::vector<TCut> vCuts(_n_pz_points);          // The cuts defining the pt_mc intervals
  std::vector<double> vPtMc(_n_pz_points);        // The centre of the pt_mc intervals
  std::vector<double> vPtMcErr(_n_pz_points);     // The width of the intervals
  std::vector<double> vPzRes_t1(_n_pz_points);    // The resulatant pz resolutions
  std::vector<double> vPzResErr_t1(_n_pz_points); // The errors assocaited with the pz res
  std::vector<double> vPzRes_t2(_n_pz_points);    // The resulatant pz resolutions
  std::vector<double> vPzResErr_t2(_n_pz_points); // The errors assocaited with the pz res

  // Cuts in pt_mc
  std::string s1 = "pt_mc>=";
  std::string s2 = "&&pt_mc<";
  for (int i = 0; i < _n_pz_points; ++i) {
    std::stringstream ss1;
    double point_lower_bound = _pz_lower_bound + (resolution_error * 2 * i);
    double point_upper_bound = point_lower_bound + (resolution_error * 2);
    ss1 << s1 << point_lower_bound << s2 << point_upper_bound;
    vCuts[i] = ss1.str().c_str();
    std::cerr << "vCuts[" << i << "] = " << vCuts[i] << std::endl;
  }

  // The central MC momentum & error associated with the MC momentum (just the interval half width)
  for (int i = 0; i < _n_pz_points; ++i) {
    vPtMc[i] = first_resolution_point + (resolution_error * 2 * i);
    vPtMcErr[i] = resolution_error;
    std::cerr << "vPtMc[" << i << "] = " << vPtMc[i] << std::endl;
  }

  // Cuts for to select each tracker
  TCut cutT1 = "tracker_num==0";
  TCut cutT2 = "tracker_num==1";

  // Form the reconstructed pz TCut
  TCut tcut_pzrec = formTCut("TMath::Abs(pz_rec)", "<", _cutPzRec);

  // Loop over the mometum intervals and calculate the resolution for each
  for (int i = 0; i < _n_pz_points; ++i) {
    TCut input_cut = vCuts[i]&&cutT1&&tcut_pzrec;
    calc_pz_resolution(0, input_cut, vPzRes_t1[i], vPzResErr_t1[i]);
  }
  for (int i = 0; i < _n_pz_points; ++i) {
    TCut input_cut = vCuts[i]&&cutT2&&tcut_pzrec;
    calc_pz_resolution(1, input_cut, vPzRes_t2[i], vPzResErr_t2[i]);
  }

  // Create the resultant resolution plots
  _t1_pz_resol = new TGraphErrors(_n_pz_points, &vPtMc[0], &vPzRes_t1[0],
                                  &vPtMcErr[0], &vPzResErr_t1[0]);
  _t2_pz_resol = new TGraphErrors(_n_pz_points, &vPtMc[0], &vPzRes_t2[0],
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
  _t1_pt_res_pt = new TGraph(_vec_t1_pt_mc.size(), &_vec_t1_pt_mc[0], &_vec_t1_pt_res[0]);
  _t1_pt_res_pt->SetName("t1_pt_res_pt");
  _t1_pt_res_pt->SetTitle("T1 p_{t} res vs. p_{t}^{MC}");
  _t1_pt_res_pt->GetXaxis()->SetTitle("p_{t}^{MC} (MeV/c)");
  _t1_pt_res_pt->GetYaxis()->SetTitle("p_{t}^{MC} - p_{t} (MeV/c)");
  _t1_pt_res_pt->GetYaxis()->SetTitleOffset(1.4);
  _t1_pt_res_pt->Draw("AP");

  _cGraphs->cd(2);
  _t1_pz_res_pt = new TGraph(_vec_t1_pt_mc.size(), &_vec_t1_pt_mc[0], &_vec_t1_pz_res[0]);
  _t1_pz_res_pt->SetName("t1_pz_res_pt");
  _t1_pz_res_pt->SetTitle("T1 p_{z} res vs. p_{t}^{MC}");
  _t1_pz_res_pt->GetXaxis()->SetTitle("p_{t}^{MC} (MeV/c)");
  _t1_pz_res_pt->GetYaxis()->SetTitle("p_{z}^{MC} - p_{z} (MeV/c)");
  _t1_pz_res_pt->GetYaxis()->SetTitleOffset(1.4);
  _t1_pz_res_pt->GetYaxis()->SetRangeUser(-200, 200);
  _t1_pz_res_pt->Draw("AP");

  _cGraphs->cd(3);
  _t2_pt_res_pt = new TGraph(_vec_t2_ptMc.size(), &_vec_t2_ptMc[0], &_vec_t2_pt_res[0]);
  _t2_pt_res_pt->SetName("t2_pt_res_pt");
  _t2_pt_res_pt->SetTitle("T2 p_{t} res vs. p_{t}^{MC}");
  _t2_pt_res_pt->GetXaxis()->SetTitle("p_{t}^{MC} (MeV/c)");
  _t2_pt_res_pt->GetYaxis()->SetTitle("p_{t}^{MC} - p_{t} (MeV/c)");
  _t2_pt_res_pt->GetYaxis()->SetTitleOffset(1.4);
  _t2_pt_res_pt->Draw("AP");

  _cGraphs->cd(4);
  _t2_pz_res_pt = new TGraph(_vec_t2_ptMc.size(), &_vec_t2_ptMc[0], &_vec_t2_pz_res[0]);
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
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  _t1_pz_resol->SetName("t1_pz_resol");
  _t1_pz_resol->SetTitle("T1 p_{z} Resolution");
  _t1_pz_resol->GetXaxis()->SetTitle("p_{t}^{MC} (MeV/c)");
  _t1_pz_resol->GetYaxis()->SetTitle("p_{z} Resolution (MeV/c)");
  _t1_pz_resol->Draw("AP");

  _cResolutions->cd(2);
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  _t2_pz_resol->SetName("t2_pz_resol");
  _t2_pz_resol->SetTitle("T2 p_{z} Resolution");
  _t2_pz_resol->GetXaxis()->SetTitle("p_{t}^{MC} (MeV/c)");
  _t2_pz_resol->GetYaxis()->SetTitle("p_{z} Resolution (MeV/c)");
  _t2_pz_resol->Draw("AP");
}

void TrackerDataAnalyserMomentum::calc_pz_resolution(const int trker, const TCut cut,
                                                     double &res, double &res_err) {
  if (_tree) {
    _out_file->cd();
    TCanvas lCanvas;
    // Create a histogram of the pz residual for the pt_mc interval defined by the input cut
    if (trker == 0) {
      _tree->Draw("pz_mc+charge*pz_rec>>h1", cut);
    } else if (trker == 1) {
      _tree->Draw("pz_mc-charge*pz_rec>>h1", cut);
    }
    // Pull the histogram from the current pad
    TH1D* h1 = reinterpret_cast<TH1D*>(gPad->GetPrimitive("h1"));
    double xmin = h1->GetXaxis()->GetXmin();
    double xmax = h1->GetXaxis()->GetXmax();
    // If set to 0, set the number of bins in the histos equal to the auto value from the ROOT tree
    if ( _n_bins == 0 ) _n_bins = h1->GetNbinsX();
    TH1D* h2 = new TH1D("h2", h1->GetTitle(), _n_bins, xmin, xmax);
    if (trker == 0) {
      _tree->Draw("pz_mc+charge*pz_rec>>h2", cut);
    } else if (trker == 1) {
      _tree->Draw("pz_mc-charge*pz_rec>>h2", cut);
    }
    // TH1D* h2 = reinterpret_cast<TH1D*>(h1->Clone("h2"));
    // Fit a gaussian to the histogram
    h2->Fit("gaus", "", "", _pz_fit_min, _pz_fit_max);
    TF1 *fit1 = h2->GetFunction("gaus");
    // Extract the sigma of the gaussian fit (equivalent to the pz resolution for this interval)
    res = fit1->GetParameter(2);
    res_err = fit1->GetParError(2);

    if (trker == 0) {
      _resol_histos_t1->Add(h2);
    } else if (trker == 1) {
      _resol_histos_t2->Add(h2);
    }

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

    if (_resol_histos_t1) _resol_histos_t1->Write();
    if (_resol_histos_t2) _resol_histos_t2->Write();

    if (_cResiduals) _cResiduals->Write();
    if (_cGraphs) _cGraphs->Write();
    if (_cResolutions) _cResolutions->Write();

    _out_file->Write();
    _out_file->Close();
  } else {
    std::cerr << "Invalid ROOT file pointer provided" << std::endl;
  }
}

TCut TrackerDataAnalyserMomentum::formTCut(const std::string &var, const std::string &op,
                                           double value) {
  TCut cut = "";
  std::stringstream ss1;
  TString s1;
  ss1 << var << op << value;
  ss1 >> s1;
  cut = s1;
  return cut;
}

} // ~namespace MAUS

