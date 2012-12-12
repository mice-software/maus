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

namespace MAUS {

KalmanMonitor::KalmanMonitor(): file(0), chi2_tracker0(0), chi2_tracker1(0),
                                station1(0), station2(0), station3(0), station4(0), station5(0),
                                station6(0), station7(0), station8(0), station9(0), station10(0),
                                pull_hist(0), residual_hist(0), smooth_residual_hist(0),
                                x_proj_h(0), y_proj_h(0),
                                px_proj_h(0), py_proj_h(0), pz_proj_h(0),
                                pull_site_3(0), residual_site_3(0), smoothed_residual_site_3(0),
                                pull_site_4(0), residual_site_4(0), smoothed_residual_site_4(0),
                                pull_site_5(0), residual_site_5(0), smoothed_residual_site_5(0) {
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
    pull_site_3   = reinterpret_cast<TH1F*> (file->Get("pull_site_3"));
    residual_site_3   = reinterpret_cast<TH1F*> (file->Get("residual_site_3"));
    smoothed_residual_site_3   = reinterpret_cast<TH1F*> (file->Get("smoothed_residual_site_3"));
    pull_site_4   = reinterpret_cast<TH1F*> (file->Get("pull_site_4"));
    residual_site_4   = reinterpret_cast<TH1F*> (file->Get("residual_site_4"));
    smoothed_residual_site_4   = reinterpret_cast<TH1F*> (file->Get("smoothed_residual_site_4"));
    pull_site_5   = reinterpret_cast<TH1F*> (file->Get("pull_site_5"));
    residual_site_5   = reinterpret_cast<TH1F*> (file->Get("residual_site_5"));
    smoothed_residual_site_5   = reinterpret_cast<TH1F*> (file->Get("smoothed_residual_site_5"));
    station1      = reinterpret_cast<TGraph*> (file->Get("xd_station1"));
    station2      = reinterpret_cast<TGraph*> (file->Get("xd_station2"));
    station3      = reinterpret_cast<TGraph*> (file->Get("xd_station3"));
    station4            = reinterpret_cast<TGraph*> (file->Get("xd_station4"));
    station5            = reinterpret_cast<TGraph*> (file->Get("xd_station5"));
    station6            = reinterpret_cast<TGraph*> (file->Get("xd_station6"));
    station7            = reinterpret_cast<TGraph*> (file->Get("xd_station7"));
    station8            = reinterpret_cast<TGraph*> (file->Get("xd_station8"));
    station9            = reinterpret_cast<TGraph*> (file->Get("xd_station9"));
    station10           = reinterpret_cast<TGraph*> (file->Get("xd_station10"));
  } else {
    pull_hist = new TH2F("pull", "Kalman", 30, 0, 29, 70, -30, 30);
    residual_hist = new TH2F("residual", "Kalman", 30, 0, 29, 70, -30, 30);
    smooth_residual_hist = new TH2F("smooth_residual", "Kalman", 30, 0, 29, 70, -30, 30);
    x_proj_h = new TH2F("x_proj_h", "projection residual; x, mm;", 31, 0, 30, 70, -150, 150);
    y_proj_h = new TH2F("y_proj_h", "projection residual; x, mm;", 31, 0, 30, 70, -150, 150);
    px_proj_h = new TH2F("px_proj_h", "projection residual; x, mm;", 31, 0, 30, 70, -50, 50);
    py_proj_h = new TH2F("py_proj_h", "projection residual; x, mm;", 31, 0, 30, 70, -50, 50);
    pz_proj_h = new TH2F("pz_proj_h", "projection residual; x, mm;", 31, 0, 30, 70, -50, 50);
    chi2_tracker0 = new TH1F("chi2_tracker0", "chi2_tracker0", 50, 0, 10);
    chi2_tracker1 = new TH1F("chi2_tracker1", "chi2_tracker1", 50, 0, 10);

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

    station1 = new TGraph();
    station1->SetName("xd_station1");
    station2 = new TGraph();
    station2->SetName("xd_station2");
    station3 = new TGraph();
    station3->SetName("xd_station3");
    station4 = new TGraph();
    station4->SetName("xd_station4");
    station5 = new TGraph();
    station5->SetName("xd_station5");
    station6 = new TGraph();
    station6->SetName("xd_station6");
    station7 = new TGraph();
    station7->SetName("xd_station7");
    station8 = new TGraph();
    station8->SetName("xd_station8");
    station9 = new TGraph();
    station9->SetName("xd_station9");
    station10 = new TGraph();
    station10->SetName("xd_station10");
  }
/*
  x_filt_h  = new TH2F("h6","X coord of Hits",31,0,30,70,-150, 150);
  y_filt_h  = new TH2F("h7","Y coord of Hits",31,0,30,70,-150, 150);
  px_filt_h = new TH2F("h8","Y coord of Hits",31,0,30,70,-50, 50);
  py_filt_h = new TH2F("h9","Y coord of Hits",31,0,30,70,-50, 50);
  pz_filt_h = new TH2F("h10","Y coord of Hits",31,0,30,70,-50, 50);

  x_smooth_h  = new TH2F("h11","X coord of Hits",31,0,30,70,-150,150);
  y_smooth_h  = new TH2F("h12","Y coord of Hits",31,0,30,70,-150,150);
  px_smooth_h = new TH2F("h13","Y coord of Hits",31,0,30,70,-50, 50);
  py_smooth_h = new TH2F("h14","Y coord of Hits",31,0,30,70,-50, 50);
  pz_smooth_h = new TH2F("h15","Y coord of Hits",31,0,30,70,-50, 50);
  ///////////////
  x_proj_h1  = new TH1F("h16","X coord of Hits",70,-150,150);
  y_proj_h1  = new TH1F("h17","Y coord of Hits",70,-150,150);
  px_proj_h1 = new TH1F("h18","Y coord of Hits",70,-50, 50);
  py_proj_h1 = new TH1F("h19","Y coord of Hits",70,-50, 50);
  pz_proj_h1 = new TH1F("h20","Y coord of Hits",70,-50, 50);

  x_filt_h1  = new TH1F("h21","X coord of Hits",70,-150,150);
  y_filt_h1  = new TH1F("h22","Y coord of Hits",70,-150,150);
  px_filt_h1 = new TH1F("h23","Y coord of Hits",70,-50, 50);
  py_filt_h1 = new TH1F("h24","Y coord of Hits",70,-50, 50);
  pz_filt_h1 = new TH1F("h25","Y coord of Hits",70,-50, 50);

  x_smooth_h1  = new TH1F("h26","X coord of Hits",70,-150,150);
  y_smooth_h1  = new TH1F("h27","Y coord of Hits",70,-150,150);
  px_smooth_h1 = new TH1F("h28","Y coord of Hits",70,-50, 50);
  py_smooth_h1 = new TH1F("h29","Y coord of Hits",70,-50, 50);
  pz_smooth_h1 = new TH1F("h30","Y coord of Hits",70,-50, 50);
*/
}

KalmanMonitor::~KalmanMonitor() {
  save();
}

void KalmanMonitor::print_info(std::vector<KalmanSite> const &sites) {
  int numb_sites = sites.size();
  // _alpha_meas.resize(numb_sites);
  _site.resize(numb_sites);
  // _alpha_projected.resize(numb_sites);

  for ( int i = 0; i < numb_sites; ++i ) {
    KalmanSite site = sites[i];
    std::cerr << "SITE ID: " << site.get_id() << std::endl;
    std::cerr << "SITE Z: " << site.get_z() << std::endl;
    std::cerr << "SITE Direction: " << "(" << site.get_direction().x() << ", " <<
                                       site.get_direction().y() << ", " <<
                                       site.get_direction().z() << ")" << std::endl;
    std::cerr << "Measurement: " << (site.get_measurement())(0, 0) << std::endl;
    std::cerr << "Shift: " <<  "(" << (site.get_shifts())(0, 0) << ", " <<
                                      (site.get_shifts())(1, 0) << ", " <<
                                      (site.get_shifts())(2, 0) << ")" << std::endl;
    std::cerr << "================Residuals================" << std::endl;
    std::cerr << (site.get_pull())(0, 0) << std::endl;
    std::cerr << (site.get_residual())(0, 0) << std::endl;
    std::cerr << (site.get_smoothed_residual())(0, 0) << std::endl;
    std::cerr << "================Projection================" << std::endl;
    site.get_projected_a().Print();
    site.get_a().Print();
    site.get_smoothed_a().Print();
    // site.get_projected_covariance_matrix().Print();
    // std::cerr << "=================Filtered=================" << std::endl;
    // site.get_a().Print();
    // site.get_covariance_matrix().Print();
    std::cerr << "==========================================" << std::endl;
  }
}

void KalmanMonitor::fill(std::vector<KalmanSite> const &sites) {
  std::vector<double> _alpha_meas, _site, _alpha_projected;
  int numb_sites = sites.size();
  _alpha_meas.resize(numb_sites);
  _site.resize(numb_sites);
  _alpha_projected.resize(numb_sites);

  double chi2 = 0;
  int tracker = 1;

  int number_parameters = 5;
  int number_of_sites = sites.size();
  int ndf = number_of_sites - number_parameters;
  for ( int i = 0; i < numb_sites; ++i ) {
    KalmanSite site = sites[i];
    // _alpha_projected.at(i) = site.get_projected_alpha();
    _site.at(i) = site.get_id();
    // _alpha_meas.at(i) = site.get_alpha();

    // double pull = _alpha_meas.at(i) - _alpha_projected.at(i);
    // double alpha_smooth = site.get_smoothed_alpha();
    // double pull2 = _alpha_meas.at(i) - alpha_smooth;

    TMatrixD a(5, 1);
    a = site.get_a();

    TMatrixD a_smooth(5, 1);
    a_smooth = site.get_smoothed_a();

    // MC position and momentum.
    double mc_x  = site.get_true_position().x();
    double mc_y  = site.get_true_position().y();
    double mc_px = site.get_true_momentum().x();
    double mc_py = site.get_true_momentum().y();
    double mc_pz = site.get_true_momentum().z();

    TMatrixD a_proj(5, 1);
    a_proj = site.get_projected_a();

    int id = site.get_id();

    if ( id < 15 ) {
      tracker = 0;
      mc_x  = -mc_x;
      mc_px = -mc_px;
      mc_pz = -mc_pz;
    }

    chi2 += site.get_chi2();

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
    double y_proj    = a_proj(2, 0);
    double px_proj   = a_proj(1, 0);
    double py_proj   = a_proj(3, 0);
    double kappa_proj= a_proj(4, 0);

  if ( id == 3 ) {
    Double_t pull = site.get_pull()(0, 0);
    pull_site_3->Fill(pull);

    Double_t residual = site.get_residual()(0, 0);
    residual_site_3->Fill(residual);

    Double_t s_residual = site.get_smoothed_residual()(0, 0);
    smoothed_residual_site_3->Fill(s_residual);
  }
  if ( id == 4 ) {
    Double_t pull = site.get_pull()(0, 0);
    pull_site_4->Fill(pull);

    Double_t residual = site.get_residual()(0, 0);
    residual_site_4->Fill(residual);

    Double_t s_residual = site.get_smoothed_residual()(0, 0);
    smoothed_residual_site_4->Fill(s_residual);
  }
  if ( id == 5 ) {
    Double_t pull = site.get_pull()(0, 0);
    pull_site_5->Fill(pull);

    Double_t residual = site.get_residual()(0, 0);
    residual_site_5->Fill(residual);

    Double_t s_residual = site.get_smoothed_residual()(0, 0);
    smoothed_residual_site_5->Fill(s_residual);
  }
/*
    double x_filt    = a(0, 0);
    double y_filt    = a(2, 0);
    double px_filt   = a(1, 0);
    double py_filt   = a(3, 0);
    double kappa_filt= a(4, 0);

    double x_smooth  = a_smooth(0, 0);
    double y_smooth  = a_smooth(2, 0);
    double px_smooth = a_smooth(1, 0);
    double py_smooth = a_smooth(3, 0);
    double kappa_smooth = a_smooth(4, 0);
*/
    TMatrixD pull = site.get_pull();
    pull_hist->Fill(id, pull(0, 0));
    TMatrixD residual = site.get_residual();
    residual_hist->Fill(id, residual(0, 0));
    TMatrixD smooth_residual = site.get_smoothed_residual();
    smooth_residual_hist->Fill(id, smooth_residual(0, 0));
    // Projections - TH1
/*
    x_proj_h1->Fill(x_proj-mc_x);
    y_proj_h1->Fill(y_proj-mc_y);
    px_proj_h1->Fill(px_proj-mc_px/mc_pz);
    py_proj_h1->Fill(py_proj-mc_py/mc_pz);
    pz_proj_h1->Fill((1./kappa_proj)-mc_pz);
*/
    // Projections per site
    x_proj_h->Fill(id, x_proj-mc_x);
    y_proj_h->Fill(id, y_proj-mc_y);
    px_proj_h->Fill(id, px_proj-mc_px/mc_pz);
    py_proj_h->Fill(id, py_proj-mc_py/mc_pz);
    pz_proj_h->Fill(id, (1./kappa_proj)-mc_pz);
/*
    // Filtered - TH1
    x_filt_h1->Fill(x_filt-mc_x);
    y_filt_h1->Fill(y_filt-mc_y);
    px_filt_h1->Fill(px_filt-mc_px/mc_pz);
    py_filt_h1->Fill(py_filt-mc_py/mc_pz);
    pz_filt_h1->Fill((1./kappa_filt)-mc_pz);
    // Filtered state per site
    x_filt_h->Fill(id, x_filt-mc_x);
    y_filt_h->Fill(id, y_filt-mc_y);
    px_filt_h->Fill(id, px_filt-mc_px/mc_pz);
    py_filt_h->Fill(id, py_filt-mc_py/mc_pz);
    pz_filt_h->Fill(id, (1./kappa_filt)-mc_pz);

    // Smooth - TH1
    x_smooth_h1->Fill(x_smooth-mc_x);
    y_smooth_h1->Fill(y_smooth-mc_y);
    px_smooth_h1->Fill(px_smooth-mc_px/mc_pz);
    py_smooth_h1->Fill(py_smooth-mc_py/mc_pz);
    pz_smooth_h1->Fill((1./kappa_smooth)-mc_pz);
    // Smooth state per site
    x_smooth_h->Fill(id, x_smooth-mc_x);
    y_smooth_h->Fill(id, y_smooth-mc_y);
    px_smooth_h->Fill(id, px_smooth-mc_px/mc_pz);
    py_smooth_h->Fill(id, py_smooth-mc_py/mc_pz);
    pz_smooth_h->Fill(id, (1./kappa_smooth)-mc_pz);
*/
    // gr = new TGraph();
    // gr->SetTitle("difference");
    // TGraphErrors *gr2 = new TGraphErrors(...
    // TMultiGraph *mg = new TMultiGraph();
    // mg->Add(gr1,"lp");
    // mg->Add(gr2,"cp");
    // mg->Draw("a");
    // int station = ceil((id+1.)/3.);
    if ( id == 0 ) {
      double y = site.get_shifts()(0, 0);
      double x = station1->GetN();
      station1->SetPoint(static_cast<Int_t> (x), x, y);
    }
    if ( id == 3 ) {
      double y = site.get_shifts()(0, 0);
      double x = station2->GetN();
      station2->SetPoint(static_cast<Int_t> (x), x, y);
    }
    if ( id == 6 ) {
      double y = site.get_shifts()(0, 0);
      double x = station3->GetN();
      station3->SetPoint(static_cast<Int_t> (x), x, y);
    }
    if ( id == 9 ) {
      double y = site.get_shifts()(0, 0);
      double x = station4->GetN();
      station4->SetPoint(static_cast<Int_t> (x), x, y);
    }
    if ( id == 12 ) {
      double y = site.get_shifts()(0, 0);
      double x = station5->GetN();
      station5->SetPoint(static_cast<Int_t> (x), x, y);
    }
    if ( id == 15 ) {
      double y = site.get_shifts()(0, 0);
      double x = station6->GetN();
      station6->SetPoint(static_cast<Int_t> (x), x, y);
    }
    if ( id == 18 ) {
      double y = site.get_shifts()(0, 0);
      double x = station7->GetN();
      station7->SetPoint(static_cast<Int_t> (x), x, y);
    }
    if ( id == 21 ) {
      double y = site.get_shifts()(0, 0);
      double x = station8->GetN();
      station8->SetPoint(static_cast<Int_t> (x), x, y);
    }
    if ( id == 24 ) {
      double y = site.get_shifts()(0, 0);
      double x = station9->GetN();
      station9->SetPoint(static_cast<Int_t> (x), x, y);
    }
    if ( id == 27 ) {
      double y = site.get_shifts()(0, 0);
      double x = station10->GetN();
      station10->SetPoint(static_cast<Int_t> (x), x, y);
    }
  }

  if ( tracker == 0 ) {
    chi2_tracker0->Fill(chi2/ndf);
  } else {
    chi2_tracker1->Fill(chi2/ndf);
  }

  _counter +=1;
  if ( !(_counter%20) ) {
    save();
  }
}

void KalmanMonitor::save() {
  // hpx->Write("",TObject::kOverwrite)
  chi2_tracker0->Write("", TObject::kOverwrite);
  chi2_tracker1->Write("", TObject::kOverwrite);
  pull_hist->Write("", TObject::kOverwrite);
  residual_hist->Write("", TObject::kOverwrite);
  smooth_residual_hist->Write("", TObject::kOverwrite);

  x_proj_h->Write("", TObject::kOverwrite);
  y_proj_h->Write("", TObject::kOverwrite);
  px_proj_h->Write("", TObject::kOverwrite);
  py_proj_h->Write("", TObject::kOverwrite);
  pz_proj_h->Write("", TObject::kOverwrite);

  station1->Write("", TObject::kOverwrite);
  station2->Write("", TObject::kOverwrite);
  station3->Write("", TObject::kOverwrite);
  station4->Write("", TObject::kOverwrite);
  station5->Write("", TObject::kOverwrite);
  station6->Write("", TObject::kOverwrite);
  station7->Write("", TObject::kOverwrite);
  station8->Write("", TObject::kOverwrite);
  station9->Write("", TObject::kOverwrite);
  station10->Write("", TObject::kOverwrite);


  pull_site_3->Write("", TObject::kOverwrite);
  residual_site_3->Write("", TObject::kOverwrite);
  smoothed_residual_site_3->Write("", TObject::kOverwrite);
  pull_site_4->Write("", TObject::kOverwrite);
  residual_site_4->Write("", TObject::kOverwrite);
  smoothed_residual_site_4->Write("", TObject::kOverwrite);
  pull_site_5->Write("", TObject::kOverwrite);
  residual_site_5->Write("", TObject::kOverwrite);
  smoothed_residual_site_5->Write("", TObject::kOverwrite);
/*
  x_filt_h->Write();
  y_filt_h->Write();
  px_filt_h->Write();
  py_filt_h->Write();
  pz_filt_h->Write();

  x_smooth_h->Write();
  y_smooth_h->Write();
  px_smooth_h->Write();
  py_smooth_h->Write();
  pz_smooth_h->Write();
  ///////////////
  x_proj_h1->Write();
  y_proj_h1->Write();
  px_proj_h1->Write();
  py_proj_h1->Write();
  pz_proj_h1->Write();

  x_filt_h1->Write();
  y_filt_h1->Write();
  px_filt_h1->Write();
  py_filt_h1->Write();
  pz_filt_h1->Write();

  x_smooth_h1->Write();
  y_smooth_h1->Write();
  px_smooth_h1->Write();
  py_smooth_h1->Write();
  pz_smooth_h1->Write();
*/
  file->Close();
}

} // ~namespace MAUS

