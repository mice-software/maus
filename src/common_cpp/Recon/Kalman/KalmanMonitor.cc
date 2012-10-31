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

KalmanMonitor::KalmanMonitor() {
  pull_hist = new TH2F("hist","Kalman Monitor",30,0,29,70,-30,30);
  pull2_hist = new TH2F("hist2","Kalman Monitor2",30,0,29,30,-50,50);

  x_proj_h = new TH2F("h1","projection residual; x, mm;;",31,0,30,70,-150, 150);
  y_proj_h = new TH2F("h2","projection residual; x, mm;;",31,0,30,70,-150, 150);
  px_proj_h = new TH2F("h3","projection residual; x, mm;;",31,0,30,70,-50, 50);
  py_proj_h = new TH2F("h4","projection residual; x, mm;;",31,0,30,70,-50, 50);
  pz_proj_h = new TH2F("h5","projection residual; x, mm;;",31,0,30,70,-50, 50);

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
}

KalmanMonitor::~KalmanMonitor() {
  TFile f("kalman_histograms.root", "new");
  f.Write();
}

void KalmanMonitor::print_info(std::vector<KalmanSite> const &sites) {
  int numb_sites = sites.size();
  _alpha_meas.resize(numb_sites);
  _site.resize(numb_sites);
  _alpha_projected.resize(numb_sites);

  for ( int i = 0; i < numb_sites; ++i ) {
    KalmanSite site = sites[i];
    std::cerr << "SITE ID: " << site.get_id() << std::endl;
    std::cerr << "SITE Z: " << site.get_z() << std::endl;
    std::cerr << "SITE Direction: " << "(" << site.get_direction().x() << ", " <<
                                       site.get_direction().y() << ", " <<
                                       site.get_direction().z() << ")" << std::endl;

    std::cerr << "SITE residual (mm): " << site.get_residual_x() << ", "
              << site.get_residual_y() << std::endl;
    std::cerr << "SITE measured alpha: " << site.get_alpha() << std::endl;
    std::cerr << "SITE smoothed alpha: " << site.get_smoothed_alpha() << std::endl;
    std::cerr << "SITE projected alpha: " << site.get_projected_alpha() << std::endl;
    std::cerr << "================Projection================" << std::endl;
    site.get_projected_a().Print();
    site.get_projected_covariance_matrix().Print();
    std::cerr << "=================Filtered=================" << std::endl;
    site.get_a().Print();
    site.get_covariance_matrix().Print();
    std::cerr << "==========================================" << std::endl;
  }
}

void KalmanMonitor::fill(std::vector<KalmanSite> const &sites) {
  std::vector<double> _alpha_meas, _site, _alpha_projected;
  int numb_sites = sites.size();
  _alpha_meas.resize(numb_sites);
  _site.resize(numb_sites);
  _alpha_projected.resize(numb_sites);

  for ( int i = 0; i < numb_sites; ++i ) {
    KalmanSite site = sites[i];
    _alpha_projected.at(i) = site.get_projected_alpha();
    _site.at(i) = site.get_id();
    _alpha_meas.at(i) = site.get_alpha();

    double pull = _alpha_meas.at(i) - _alpha_projected.at(i);
    double alpha_smooth = site.get_smoothed_alpha();
    double pull2 = _alpha_meas.at(i) - alpha_smooth;

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

    std::ofstream out2("kalman_mc.txt", std::ios::out | std::ios::app);

    if ( id < 15 ) {
      mc_x  = -mc_x;
      mc_px = -mc_px;
      mc_pz = -mc_pz;
    }
    // assert(a_smooth(0, 0) ==  a_smooth(0, 0));
    // assert(a_proj(0, 0) == a_proj(0, 0));
/*
    out2 << a_proj(0, 0) << " " << a_proj(1, 0) << " " << a_proj(2, 0)
         << " " << a_proj(3, 0) << " " << a_proj(4, 0) << " "
         << a(0, 0) << " " << a(1, 0) << " " << a(2, 0) << " " << a(3, 0)
         << " " << a(4, 0) << " " << a_smooth(0, 0) << " " << a_smooth(1, 0) << " "
         << a_smooth(2, 0) << " " << a_smooth(3, 0) << " " << a_smooth(4, 0) << " "
         << mc_x << " " << mc_y << " " << mc_px << " " << mc_py << " " << mc_pz << " "
         << pull << " " << pull2 << " " << id     << "\n";
    out2.close();
*/
    double x_proj = a_proj(0, 0);
    double y_proj =a_proj(2, 0);
    double px_proj=a_proj(1, 0);
    double py_proj=a_proj(3, 0);
    double kappa_proj= a_proj(4, 0);

    double x_filt=a(0, 0);
    double y_filt=a(2, 0);
    double px_filt=a(1, 0);
    double py_filt=a(3, 0);
    double kappa_filt=a(4, 0);

    double x_smooth=a_smooth(0, 0);
    double y_smooth=a_smooth(2, 0);
    double px_smooth=a_smooth(1, 0);
    double py_smooth=a_smooth(3, 0);
    double kappa_smooth=a_smooth(4, 0);

    pull_hist->Fill(id, pull);
    pull2_hist->Fill(id, pull2);

    // Projections - TH1
    x_proj_h1->Fill(x_proj-mc_x);
    y_proj_h1->Fill(y_proj-mc_y);
    px_proj_h1->Fill(px_proj-mc_px/mc_pz);
    py_proj_h1->Fill(py_proj-mc_py/mc_pz);
    pz_proj_h1->Fill((1./kappa_proj)-mc_pz);
    // Projections per site
    x_proj_h->Fill(id, x_proj-mc_x);
    y_proj_h->Fill(id, y_proj-mc_y);
    px_proj_h->Fill(id, px_proj-mc_px/mc_pz);
    py_proj_h->Fill(id, py_proj-mc_py/mc_pz);
    pz_proj_h->Fill(id, (1./kappa_proj)-mc_pz);
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
  }
}

} // ~namespace MAUS

