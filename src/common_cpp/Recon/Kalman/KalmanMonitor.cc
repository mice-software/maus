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

#include "src/common_cpp/Recon/Kalman/KalmanMonitor.hh"
#include <TCanvas.h>
#include <TImage.h>
#include <TLegend.h>
#include <TMultiGraph.h>

namespace MAUS {

KalmanMonitor::KalmanMonitor(): file(0),
                                chi2_tracker0(0), chi2_tracker1(0),
                                pvalue_tracker0(0), pvalue_tracker1(0),
                                station1_x(0), station2_x(0), station3_x(0),
                                station4_x(0), station5_x(0),
                                station6_x(0), station7_x(0), station8_x(0),
                                station9_x(0), station10_x(0),
                                pull_hist(0), residual_hist(0), smooth_residual_hist(0),
                                x_proj_h(0), y_proj_h(0),
                                px_proj_h(0), py_proj_h(0), pz_proj_h(0),
                                pull_site_3(0), residual_site_3(0), smoothed_residual_site_3(0),
                                pull_site_4(0), residual_site_4(0), smoothed_residual_site_4(0),
                                pull_site_5(0), residual_site_5(0), smoothed_residual_site_5(0),
                                pull_site_9(0), residual_site_9(0), smoothed_residual_site_9(0),
                                excl_plane11(0) {
  _counter = 0;
  file = new TFile("kalman_histograms.root", "UPDATE");

  pull_hist  = reinterpret_cast<TH2F*> (file->Get("pull"));

  if ( pull_hist ) {
    residual_hist = reinterpret_cast<TH2F*> (file->Get("residual"));
    smooth_residual_hist= reinterpret_cast<TH2F*> (file->Get("smooth_residual"));
    x_proj_h   = reinterpret_cast<TH2F*> (file->Get("x_proj_h"));
    y_proj_h   = reinterpret_cast<TH2F*> (file->Get("y_proj_h"));
    px_proj_h  = reinterpret_cast<TH2F*> (file->Get("px_proj_h"));
    py_proj_h  = reinterpret_cast<TH2F*> (file->Get("py_proj_h"));
    pz_proj_h  = reinterpret_cast<TH2F*> (file->Get("pz_proj_h"));
    chi2_tracker0 = reinterpret_cast<TH1F*> (file->Get("chi2_tracker0"));
    chi2_tracker1 = reinterpret_cast<TH1F*> (file->Get("chi2_tracker1"));
    pvalue_tracker0 = reinterpret_cast<TH1F*> (file->Get("pvalue_tracker0"));
    pvalue_tracker1 = reinterpret_cast<TH1F*> (file->Get("pvalue_tracker1"));
    pull_site_3   = reinterpret_cast<TH1F*> (file->Get("pull_site_3"));
    residual_site_3   = reinterpret_cast<TH1F*> (file->Get("residual_site_3"));
    smoothed_residual_site_3   = reinterpret_cast<TH1F*> (file->Get("smoothed_residual_site_3"));
    pull_site_4   = reinterpret_cast<TH1F*> (file->Get("pull_site_4"));
    residual_site_4   = reinterpret_cast<TH1F*> (file->Get("residual_site_4"));
    smoothed_residual_site_4   = reinterpret_cast<TH1F*> (file->Get("smoothed_residual_site_4"));
    pull_site_5   = reinterpret_cast<TH1F*> (file->Get("pull_site_5"));
    residual_site_5   = reinterpret_cast<TH1F*> (file->Get("residual_site_5"));
    smoothed_residual_site_5   = reinterpret_cast<TH1F*> (file->Get("smoothed_residual_site_5"));
    pull_site_9   = reinterpret_cast<TH1F*> (file->Get("pull_site_9"));
    residual_site_9   = reinterpret_cast<TH1F*> (file->Get("residual_site_9"));
    smoothed_residual_site_9   = reinterpret_cast<TH1F*> (file->Get("smoothed_residual_site_9"));
    station1_x      = reinterpret_cast<TGraph*> (file->Get("xd_station1"));
    station2_x      = reinterpret_cast<TGraph*> (file->Get("xd_station2"));
    station3_x      = reinterpret_cast<TGraph*> (file->Get("xd_station3"));
    station4_x            = reinterpret_cast<TGraph*> (file->Get("xd_station4"));
    station5_x            = reinterpret_cast<TGraph*> (file->Get("xd_station5"));
    station6_x            = reinterpret_cast<TGraph*> (file->Get("xd_station6"));
    station7_x            = reinterpret_cast<TGraph*> (file->Get("xd_station7"));
    station8_x            = reinterpret_cast<TGraph*> (file->Get("xd_station8"));
    station9_x            = reinterpret_cast<TGraph*> (file->Get("xd_station9"));
    station10_x           = reinterpret_cast<TGraph*> (file->Get("xd_station10"));
    station1_y      = reinterpret_cast<TGraph*> (file->Get("yd_station1"));
    station2_y      = reinterpret_cast<TGraph*> (file->Get("yd_station2"));
    station3_y      = reinterpret_cast<TGraph*> (file->Get("yd_station3"));
    station4_y            = reinterpret_cast<TGraph*> (file->Get("yd_station4"));
    station5_y            = reinterpret_cast<TGraph*> (file->Get("yd_station5"));
    station6_y            = reinterpret_cast<TGraph*> (file->Get("yd_station6"));
    station7_y            = reinterpret_cast<TGraph*> (file->Get("yd_station7"));
    station8_y            = reinterpret_cast<TGraph*> (file->Get("yd_station8"));
    station9_y            = reinterpret_cast<TGraph*> (file->Get("yd_station9"));
    station10_y           = reinterpret_cast<TGraph*> (file->Get("yd_station10"));
    excl_plane11         = reinterpret_cast<TH1F*> (file->Get("excl_plane11"));
  } else {
    pull_hist = new TH2F("pull", "Kalman", 30, 0, 29, 70, -30, 30);
    residual_hist = new TH2F("residual", "Kalman", 30, 0, 29, 70, -30, 30);
    smooth_residual_hist = new TH2F("smooth_residual", "Kalman", 30, 0, 29, 70, -30, 30);
    x_proj_h = new TH2F("x_proj_h", "projection residual; x, mm;", 31, 0, 30, 70, -50, 50);
    y_proj_h = new TH2F("y_proj_h", "projection residual; x, mm;", 31, 0, 30, 70, -50, 50);
    px_proj_h = new TH2F("px_proj_h", "projection residual; x, mm;", 31, 0, 30, 70, -30, 30);
    py_proj_h = new TH2F("py_proj_h", "projection residual; x, mm;", 31, 0, 30, 70, -30, 30);
    pz_proj_h = new TH2F("pz_proj_h", "projection residual; x, mm;", 31, 0, 30, 70, -30, 30);
    chi2_tracker0 = new TH1F("chi2_tracker0", "chi2_tracker0", 70, 0, 10);
    chi2_tracker1 = new TH1F("chi2_tracker1", "chi2_tracker1", 70, 0, 10);
    pvalue_tracker0 = new TH1F("pvalue_tracker0", "pvalue_tracker0", 70, 0, 1);
    pvalue_tracker1 = new TH1F("pvalue_tracker1", "pvalue_tracker1", 70, 0, 1);
    pull_site_3   = new TH1F("pull_site_3", "pull_site_3", 50, -10, 10);
    residual_site_3   = new TH1F("residual_site_3", "residual_site_3", 50, -10, 10);
    smoothed_residual_site_3   = new TH1F("smoothed_residual_site_3",
                                          "smoothed_residual_site_3", 50, -10, 10);

    pull_site_4   = new TH1F("pull_site_4", "pull_site_4", 50, -10, 10);
    residual_site_4   = new TH1F("residual_site_4", "residual_site_4", 50, -10, 10);
    smoothed_residual_site_4   = new TH1F("smoothed_residual_site_4",
                                          "smoothed_residual_site_4", 50, -10, 10);

    pull_site_5   = new TH1F("pull_site_5", "pull_site_5", 50, -10, 10);
    residual_site_5   = new TH1F("residual_site_5", "residual_site_5", 50, -10, 10);
    smoothed_residual_site_5   = new TH1F("smoothed_residual_site_5",
                                          "smoothed_residual_site_5", 50, -10, 10);

    pull_site_9   = new TH1F("pull_site_9", "pull_site_9", 50, -5, 5);
    residual_site_9   = new TH1F("residual_site_9", "residual_site_9", 50, -10, 10);
    smoothed_residual_site_9   = new TH1F("smoothed_residual_site_9",
                                          "smoothed_residual_site_9", 70, -4, 4);

    station1_x = new TGraph();
    station1_x->SetName("xd_station1");
    station2_x = new TGraph();
    station2_x->SetName("xd_station2");
    station3_x = new TGraph();
    station3_x->SetName("xd_station3");
    station4_x = new TGraph();
    station4_x->SetName("xd_station4");
    station5_x = new TGraph();
    station5_x->SetName("xd_station5");
    station6_x = new TGraph();
    station6_x->SetName("xd_station6");
    station7_x = new TGraph();
    station7_x->SetName("xd_station7");
    station8_x = new TGraph();
    station8_x->SetName("xd_station8");
    station9_x = new TGraph();
    station9_x->SetName("xd_station9");
    station10_x = new TGraph();
    station10_x->SetName("xd_station10");
    station1_y = new TGraph();
    station1_y->SetName("yd_station1");
    station2_y = new TGraph();
    station2_y->SetName("yd_station2");
    station3_y = new TGraph();
    station3_y->SetName("yd_station3");
    station4_y = new TGraph();
    station4_y->SetName("yd_station4");
    station5_y = new TGraph();
    station5_y->SetName("yd_station5");
    station6_y = new TGraph();
    station6_y->SetName("yd_station6");
    station7_y = new TGraph();
    station7_y->SetName("yd_station7");
    station8_y = new TGraph();
    station8_y->SetName("yd_station8");
    station9_y = new TGraph();
    station9_y->SetName("yd_station9");
    station10_y = new TGraph();
    station10_y->SetName("yd_station10");
    excl_plane11   = new TH1F("excl_plane11", "excl_plane11", 50, -10, 10);
  }
}

KalmanMonitor::~KalmanMonitor() {
  save();
}

void KalmanMonitor::print_info(std::vector<KalmanSite> const &sites) {
  size_t numb_sites = sites.size();

  for ( size_t i = 0; i < numb_sites; ++i ) {
    KalmanSite site = sites[i];
    std::cerr << "SITE ID: " << site.get_id() << std::endl;
    std::cerr << "Momentum: " << site.get_true_momentum() << std::endl;
    // std::cerr << "SITE Z: " << site.get_z() << std::endl;
    // std::cerr << "SITE Direction: " << "(" << site.get_direction().x() << ", " <<
    //                                   site.get_direction().y() << ", " <<
    //                                   site.get_direction().z() << ")" << std::endl;
    std::cerr << "Measurement: " << (site.get_measurement())(0, 0) << std::endl;
    // std::cerr << "Shift: " <<  "(" << (site.get_input_shift_A())(0, 0) << ", " <<
    //                                  (site.get_input_shift_A())(1, 0) << ", " <<
    //                                  (site.get_input_shift_A())(2, 0) << ")" << std::endl;
    std::cerr << "================Residuals================" << std::endl;
    std::cerr << (site.get_residual(KalmanSite::Projected))(0, 0) << std::endl;
    std::cerr << (site.get_residual(KalmanSite::Filtered))(0, 0) << std::endl;
    std::cerr << (site.get_residual(KalmanSite::Smoothed))(0, 0) << std::endl;
    std::cerr << "================Projection================" << std::endl;
    // site.get_a(KalmanSite::Projected).Print();
    // site.get_a(KalmanSite::Filtered).Print();
    // site.get_a(KalmanSite::Smoothed).Print();
    // site.get_a().Print();
    // site.get_smoothed_a().Print();
    // site.get_projected_covariance_matrix().Print();
    // std::cerr << "=================Filtered=================" << std::endl;
    // site.get_a().Print();
    // site.get_covariance_matrix().Print();
    std::cerr << "==========================================" << std::endl;
  }
}

void KalmanMonitor::fill(std::vector<KalmanSite> const &sites) {
  double chi2 = 0;
  int tracker = 1;

  int number_parameters = 4;
  size_t numb_sites = sites.size();
  int ndf = numb_sites - number_parameters;

  for ( size_t i = 0; i < numb_sites; ++i ) {
    KalmanSite site = sites[i];

    TMatrixD a(5, 1);
    a = site.get_a(KalmanSite::Filtered);

    TMatrixD a_smooth(5, 1);
    a_smooth = site.get_a(KalmanSite::Smoothed);

    // MC position and momentum.
    double mc_x  = site.get_true_position().x();
    double mc_y  = site.get_true_position().y();
    double mc_px = site.get_true_momentum().x();
    double mc_py = site.get_true_momentum().y();
    double mc_pz = site.get_true_momentum().z();

    TMatrixD a_proj(5, 1);
    a_proj = site.get_a(KalmanSite::Projected);

    int id = site.get_id();
    if ( id < 15 ) {
      tracker = 0;
      mc_x  = -mc_x;
      mc_px = -mc_px;
      // mc_pz = -mc_pz;
    }

    chi2 += site.get_chi2(KalmanSite::Filtered);
    // assert(a_smooth(0, 0) ==  a_smooth(0, 0) && "Sanity check - smoothing");
    // assert(a_proj(0, 0)   == a_proj(0, 0)    && "Sanity check - projection.");
/*
    std::ofstream out2("kalman_mc.txt", std::ios::out | std::ios::app);
    out2 << a_proj(0, 0) << " " << a_proj(1, 0) << " " << a_proj(2, 0)
         << " " << a_proj(3, 0) << " " << a_proj(4, 0) << " "
         << a(0, 0) << " " << a(1, 0) << " " << a(2, 0) << " " << a(3, 0)
         << " " << a(4, 0) << " " << a_smooth(0, 0) << " " << a_smooth(1, 0) << " "
         << a_smooth(2, 0) << " " << a_smooth(3, 0) << " " << a_smooth(4, 0) << " "
         << mc_x << " " << mc_y << " " << mc_px << " " << mc_py << " " << mc_pz << " "
         << pull << " " << pull2 << " " << id     << "\n";
    out2.close();
*/
    double x_proj    = a_proj(0, 0);
    double px_proj   = a_proj(1, 0);
    double y_proj    = a_proj(2, 0);
    double py_proj   = a_proj(3, 0);
    double kappa_proj= a_proj(4, 0);

  if ( id == 3 ) {
    Double_t pull = site.get_residual(KalmanSite::Projected)(0, 0);
    pull_site_3->Fill(pull);

    Double_t residual = site.get_residual(KalmanSite::Filtered)(0, 0);
    residual_site_3->Fill(residual);

    Double_t s_residual = site.get_residual(KalmanSite::Smoothed)(0, 0);
    smoothed_residual_site_3->Fill(s_residual);
  }
  if ( id == 4 ) {
    Double_t pull = site.get_residual(KalmanSite::Projected)(0, 0);
    pull_site_4->Fill(pull);

    Double_t residual = site.get_residual(KalmanSite::Filtered)(0, 0);
    residual_site_4->Fill(residual);

    Double_t s_residual = site.get_residual(KalmanSite::Smoothed)(0, 0);
    smoothed_residual_site_4->Fill(s_residual);
  }
  if ( id == 5 ) {
    Double_t pull = site.get_residual(KalmanSite::Projected)(0, 0);
    pull_site_5->Fill(pull);

    Double_t residual = site.get_residual(KalmanSite::Filtered)(0, 0);
    residual_site_5->Fill(residual);

    Double_t s_residual = site.get_residual(KalmanSite::Smoothed)(0, 0);
    smoothed_residual_site_5->Fill(s_residual);
  }

  if ( id == 9 ) {
    Double_t pull = site.get_residual(KalmanSite::Projected)(0, 0);
    pull_site_9->Fill(pull);

    Double_t residual = site.get_residual(KalmanSite::Filtered)(0, 0);
    residual_site_9->Fill(residual);

    Double_t s_residual = site.get_residual(KalmanSite::Smoothed)(0, 0);
    smoothed_residual_site_9->Fill(s_residual);
  }

  if ( id == 11 ) {
    Double_t excl_residual = site.get_residual(KalmanSite::Excluded)(0, 0);
    excl_plane11->Fill(excl_residual);
  }

    TMatrixD pull = site.get_residual(KalmanSite::Projected);
    pull_hist->Fill(id, pull(0, 0));
    TMatrixD residual = site.get_residual(KalmanSite::Filtered);
    residual_hist->Fill(id, residual(0, 0));
    TMatrixD smooth_residual = site.get_residual(KalmanSite::Smoothed);
    smooth_residual_hist->Fill(id, smooth_residual(0, 0));

    // Projections per site
    x_proj_h->Fill(id, x_proj-mc_x);
    y_proj_h->Fill(id, y_proj-mc_y);
    px_proj_h->Fill(id, px_proj/kappa_proj-mc_px);
    py_proj_h->Fill(id, py_proj/kappa_proj-mc_py);
    pz_proj_h->Fill(id, (1./kappa_proj)-mc_pz);

    // gr = new TGraph();
    // gr->SetTitle("difference");
    // TGraphErrors *gr2 = new TGraphErrors(...
    // TMultiGraph *mg = new TMultiGraph();
    // mg->Add(gr1,"lp");
    // mg->Add(gr2,"cp");
    // mg->Draw("a");
    // int station = ceil((id+1.)/3.);
    if ( id == 0 ) {
      double y = site.get_input_shift()(0, 0);
      double yd = site.get_input_shift()(1, 0);
      double x = station1_x->GetN();
      station1_x->SetPoint(static_cast<Int_t> (x), x, y);
      station1_y->SetPoint(static_cast<Int_t> (x), x, yd);
    }
    if ( id == 3 ) {
      double y = site.get_input_shift()(0, 0);
      double yd = site.get_input_shift()(1, 0);
      double x = station2_x->GetN();
      station2_x->SetPoint(static_cast<Int_t> (x), x, y);
      station2_y->SetPoint(static_cast<Int_t> (x), x, yd);
    }
    if ( id == 6 ) {
      double y = site.get_input_shift()(0, 0);
      double yd = site.get_input_shift()(1, 0);
      double x = station3_x->GetN();
      station3_x->SetPoint(static_cast<Int_t> (x), x, y);
      station3_y->SetPoint(static_cast<Int_t> (x), x, yd);
    }
    if ( id == 9 ) {
      double y = site.get_input_shift()(0, 0);
      double yd = site.get_input_shift()(1, 0);
      double x = station4_x->GetN();
      station4_x->SetPoint(static_cast<Int_t> (x), x, y);
      station4_y->SetPoint(static_cast<Int_t> (x), x, yd);
    }
    if ( id == 12 ) {
      double y = site.get_input_shift()(0, 0);
      double yd = site.get_input_shift()(1, 0);
      double x = station5_x->GetN();
      station5_x->SetPoint(static_cast<Int_t> (x), x, y);
      station5_y->SetPoint(static_cast<Int_t> (x), x, yd);
    }
    if ( id == 15 ) {
      double y = site.get_input_shift()(0, 0);
      double yd = site.get_input_shift()(1, 0);
      double x = station6_x->GetN();
      station6_x->SetPoint(static_cast<Int_t> (x), x, y);
      station6_y->SetPoint(static_cast<Int_t> (x), x, yd);
    }
    if ( id == 18 ) {
      double y = site.get_input_shift()(0, 0);
      double yd = site.get_input_shift()(1, 0);
      double x = station7_x->GetN();
      station7_x->SetPoint(static_cast<Int_t> (x), x, y);
      station7_y->SetPoint(static_cast<Int_t> (x), x, yd);
    }
    if ( id == 21 ) {
      double y = site.get_input_shift()(0, 0);
      double yd = site.get_input_shift()(1, 0);
      double x = station8_x->GetN();
      station8_x->SetPoint(static_cast<Int_t> (x), x, y);
      station8_y->SetPoint(static_cast<Int_t> (x), x, yd);
    }
    if ( id == 24 ) {
      double y = site.get_input_shift()(0, 0);
      double yd = site.get_input_shift()(1, 0);
      double x = station9_x->GetN();
      station9_x->SetPoint(static_cast<Int_t> (x), x, y);
      station9_y->SetPoint(static_cast<Int_t> (x), x, yd);
    }
    if ( id == 27 ) {
      double y = site.get_input_shift()(0, 0);
      double yd = site.get_input_shift()(1, 0);
      double x = station10_x->GetN();
      station10_x->SetPoint(static_cast<Int_t> (x), x, y);
      station10_y->SetPoint(static_cast<Int_t> (x), x, yd);
    }
  }
  double P_value = TMath::Prob(chi2, ndf);
  if ( tracker == 0 ) {
    chi2_tracker0->Fill(chi2/ndf);
    pvalue_tracker0->Fill(P_value);
  } else {
    chi2_tracker1->Fill(chi2/ndf);
    pvalue_tracker1->Fill(P_value);
  }

  _counter +=1;
  if ( !(_counter%10) ) {
    save();
  }

/*
  // Energy loss and scattering angles
  for ( int i = 1; i < numb_sites; ++i ) {
    KalmanSite site_i_1 = sites[i-1];
    KalmanSite site_i = sites[i];

    double muon_mass  = 105.65836668;
    double muon_mass2 = TMath::Power(muon_mass, 2.);

    double old_mc_x  = site_i_1.get_true_position().x();
    double old_mc_y  = site_i_1.get_true_position().y();
    double old_mc_px = site_i_1.get_true_momentum().x();
    double old_mc_py = site_i_1.get_true_momentum().y();
    double old_mc_pz = site_i_1.get_true_momentum().z();
    double old_p2 = TMath::Power(old_mc_px, 2.) +
                    TMath::Power(old_mc_py, 2.) +
                    TMath::Power(old_mc_pz, 2.);

    double old_p  = TMath::Sqrt(old_p2);
    double old_energy  = TMath::Sqrt(muon_mass2+old_p2);
    double old_thethax = atan2(old_mc_px, old_mc_pz);
    double old_thethay = atan2(old_mc_py, old_mc_pz);


    double new_mc_x  = site_i.get_true_position().x();
    double new_mc_y  = site_i.get_true_position().y();
    double new_mc_px = site_i.get_true_momentum().x();
    double new_mc_py = site_i.get_true_momentum().y();
    double new_mc_pz = site_i.get_true_momentum().z();
    double new_p2 = TMath::Power(new_mc_px, 2.) +
                    TMath::Power(new_mc_py, 2.) +
                    TMath::Power(new_mc_pz, 2.);

    double new_energy  = TMath::Sqrt(muon_mass2+new_p2);
    double new_thethax = atan2(new_mc_px, new_mc_pz);
    double new_thethay = atan2(new_mc_py, new_mc_pz);

    if ( site_i.get_id() == 1 ||
         site_i.get_id() == 2 ||
         site_i.get_id() == 4 ||
         site_i.get_id() == 5 ||
         site_i.get_id() == 7 ||
         site_i.get_id() == 8 ||
         site_i.get_id() == 10 ||
         site_i.get_id() == 11 ||
         site_i.get_id() == 13 ||
         site_i.get_id() == 14 ) {
      double fibre_eloss = old_energy - new_energy;
      double fibre_mcs_x = old_thethax - new_thethax;
      double fibre_mcs_y = old_thethay - new_thethay;
      std::ofstream out2("eloss_fibre.txt", std::ios::out | std::ios::app);
      out2 << old_p << " " << fibre_eloss << " " << fibre_mcs_x << " " << fibre_mcs_y << "\n";
      out2.close();
    } else if ( site_i.get_id() == 3 ||
         site_i.get_id() == 6 ||
         site_i.get_id() == 9 ||
         site_i.get_id() == 12 ) {
      double _eloss = old_energy - new_energy;
      double _mcs_x = old_thethax - new_thethax;
      double _mcs_y = old_thethay - new_thethay;
      std::ofstream out2("eloss_fibre_gas.txt", std::ios::out | std::ios::app);
      out2 << old_p << " " << _eloss << " " << _mcs_x << " " << _mcs_y << "\n";
      out2.close();
    }
  }
*/
}

void KalmanMonitor::save() {
  // hpx->Write("",TObject::kOverwrite)
  chi2_tracker0->Write("", TObject::kOverwrite);
  chi2_tracker1->Write("", TObject::kOverwrite);
  pvalue_tracker0->Write("", TObject::kOverwrite);
  pvalue_tracker1->Write("", TObject::kOverwrite);
  pull_hist->Write("", TObject::kOverwrite);
  residual_hist->Write("", TObject::kOverwrite);
  smooth_residual_hist->Write("", TObject::kOverwrite);

  x_proj_h->Write("", TObject::kOverwrite);
  y_proj_h->Write("", TObject::kOverwrite);
  px_proj_h->Write("", TObject::kOverwrite);
  py_proj_h->Write("", TObject::kOverwrite);
  pz_proj_h->Write("", TObject::kOverwrite);

  station1_x->Write("", TObject::kOverwrite);
  station2_x->Write("", TObject::kOverwrite);
  station3_x->Write("", TObject::kOverwrite);
  station4_x->Write("", TObject::kOverwrite);
  station5_x->Write("", TObject::kOverwrite);
  station6_x->Write("", TObject::kOverwrite);
  station7_x->Write("", TObject::kOverwrite);
  station8_x->Write("", TObject::kOverwrite);
  station9_x->Write("", TObject::kOverwrite);
  station10_x->Write("", TObject::kOverwrite);
  station1_y->Write("", TObject::kOverwrite);
  station2_y->Write("", TObject::kOverwrite);
  station3_y->Write("", TObject::kOverwrite);
  station4_y->Write("", TObject::kOverwrite);
  station5_y->Write("", TObject::kOverwrite);
  station6_y->Write("", TObject::kOverwrite);
  station7_y->Write("", TObject::kOverwrite);
  station8_y->Write("", TObject::kOverwrite);
  station9_y->Write("", TObject::kOverwrite);
  station10_y->Write("", TObject::kOverwrite);

  pull_site_3->Write("", TObject::kOverwrite);
  residual_site_3->Write("", TObject::kOverwrite);
  smoothed_residual_site_3->Write("", TObject::kOverwrite);
  pull_site_4->Write("", TObject::kOverwrite);
  residual_site_4->Write("", TObject::kOverwrite);
  smoothed_residual_site_4->Write("", TObject::kOverwrite);
  pull_site_5->Write("", TObject::kOverwrite);
  residual_site_5->Write("", TObject::kOverwrite);
  smoothed_residual_site_5->Write("", TObject::kOverwrite);
  pull_site_9->Write("", TObject::kOverwrite);
  residual_site_9->Write("", TObject::kOverwrite);
  smoothed_residual_site_9->Write("", TObject::kOverwrite);
  excl_plane11->Write("", TObject::kOverwrite);


  // TCanvas *c = new TCanvas("c","c",600,500);
  TMultiGraph *mg = new TMultiGraph("multigraph", "multigraph");
  mg->SetMaximum(1.);
  mg->SetMinimum(-3.);
  station2_x->SetLineColor(kBlack);
  station2_x->SetLineWidth(2);
  station3_x->SetLineColor(kBlue);
  station3_x->SetLineWidth(2);
  station4_x->SetLineColor(kRed);
  station4_x->SetLineWidth(2);
  station2_y->SetLineColor(kBlack);
  station2_y->SetLineWidth(2);
  station2_y->SetLineStyle(9);
  station3_y->SetLineColor(kBlue);
  station3_y->SetLineWidth(2);
  station3_y->SetLineStyle(9);
  station4_y->SetLineColor(kRed);
  station4_y->SetLineWidth(2);
  station4_y->SetLineStyle(9);
  mg->Add(station2_x);
  mg->Add(station3_x);
  mg->Add(station4_x);
  mg->Add(station2_y);
  mg->Add(station3_y);
  mg->Add(station4_y);

  // station4_x->Draw("ALP");
  // mg->Draw("LP");
  // c->BuildLegend();
/*
  TLegend *leg = new TLegend(0.1,0.7,0.4,0.9);
  leg->SetHeader("The Legend Title");
  leg->AddEntry("station2_x","Station 2, #Delta x","l");
  leg->AddEntry("station2_y","Station 2, #Delta y","l");
  leg->AddEntry("station3_x","Station 3, #Delta x","l");
  leg->AddEntry("station3_y","Station 3, #Delta y","l");
  leg->AddEntry("station4_x","Station 4, #Delta x","l");
  leg->AddEntry("station4_y","Station 4, #Delta y","l");
  leg->Draw();
*/
  mg->SetMaximum(1.);
  mg->SetMinimum(-3.);
  mg->Write("", TObject::kOverwrite);
/*
  TImage *img = TImage::Create();
  img->FromPad(c);
  img->WriteImage("canvas.png");
  delete c;
  delete leg;
*/
  file->Close();
}
} // ~namespace MAUS
