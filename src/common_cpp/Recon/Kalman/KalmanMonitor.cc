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

KalmanMonitor::KalmanMonitor() {
  // histo = new TH1F("h","monitor; site; alpha",30, 0, 30);
}

KalmanMonitor::~KalmanMonitor() {
  TFile f("hist.root", "update");
  // file->Write();
  TGraph *gr = new TGraph(30, &(_site[0]), &(_alpha_projected[0]));
  gr->SetName("extrapolation");
  TGraph *gr2 = new TGraph(30, &(_site[0]), &(_alpha_meas[0]));
  gr2->SetName("measurement");
  TMultiGraph *mgr = new TMultiGraph();
  mgr->Add(gr, "p");
  mgr->Add(gr2, "p");
  mgr->Write();
}

void KalmanMonitor::save(std::vector<KalmanSite> const &sites) {
  int numb_sites = sites.size();
  _alpha_meas.resize(numb_sites);
  _site.resize(numb_sites);
  _alpha_projected.resize(numb_sites);

  for ( int i = 0; i < numb_sites; ++i ) {
    KalmanSite site = sites[i];
    // std::cerr << "SITE ID: " << site.get_id() << std::endl;
    // std::cerr << "SITE extrap alpha: " << site.get_extrapolated_alpha() << std::endl;
    // std::cerr << "SITE measured alpha: " << site.get_alpha() << std::endl;

    _alpha_projected.at(i) = site.get_projected_alpha();
    _site.at(i) = site.get_id();
    _alpha_meas.at(i) = site.get_alpha();

    double pull = _alpha_meas.at(i) - _alpha_projected.at(i);
    double alpha_smooth = get_smoothed_measurement(site);
    double pull2 = _alpha_meas.at(i) - alpha_smooth;
    // std::cerr << "PULL: " << _alpha_meas.at(i) << " " << _alpha_extrap.at(i) << std::endl;
    TMatrixD a(5, 1);
    a = site.get_a();
    TMatrixD C(5, 5);
    C = site.get_covariance_matrix();

    TMatrixD a_smooth(5, 1);
    a_smooth = site.get_smoothed_a();
    double res_x = site.get_residual_x();
    double res_y = site.get_residual_y();
    // double observed = pow(pow(a(0, 0), 2.)+pow(a(1, 0), 2.), 0.5);
    // double expected = pow(pow(a_filt(0, 0), 2.)+pow(a_filt(1, 0), 2.), 0.5);
    // double chi2 = pow(observed-expected, 2.)/expected;
    std::ofstream out2("kalman.txt", std::ios::out | std::ios::app);
    int id = site.get_id();
    out2 << a(0, 0)    << " " << C(0, 0) << " "
         << a(1, 0)    << " " << C(1, 1) << " "
         << res_x      << " " << res_y << " "
         << a_smooth(0, 0) << " " << a_smooth(1, 0) << " "
         << pull << " " << pull2 << " " << id     << "\n";
    out2.close();
  }
}

void KalmanMonitor::print_info(std::vector<KalmanSite> const &sites) {
  int numb_sites = sites.size();
  _alpha_meas.resize(numb_sites);
  _site.resize(numb_sites);
  _alpha_projected.resize(numb_sites);

  for ( int i = 0; i < numb_sites; ++i ) {
    KalmanSite site = sites[i];
    std::cerr << "SITE ID: " << site.get_id() << std::endl;
    // std::cerr << "SITE Z: " << site.get_z() << std::endl;
    // std::cerr << "SITE Direction: " << "(" << site.get_direction().x() << ", " <<
    //                                   site.get_direction().y() << ", " <<
    //                                   site.get_direction().z() << ")" << std::endl;

    std::cerr << "SITE residual (mm): " << site.get_residual_x() << ", "
              << site.get_residual_y() << std::endl;
    std::cerr << "SITE measured alpha: " << site.get_alpha() << std::endl;
    std::cerr << "SITE projected alpha: " << site.get_projected_alpha() << std::endl;
    // site.get_projected_a().Print();
    // site.get_a().Print();
  }
}

void KalmanMonitor::save_mc(std::vector<KalmanSite> const &sites) {
  int numb_sites = sites.size();
  _alpha_meas.resize(numb_sites);
  _site.resize(numb_sites);
  _alpha_projected.resize(numb_sites);

  for ( int i = 0; i < numb_sites; ++i ) {
    KalmanSite site = sites[i];
    // std::cerr << "SITE ID: " << site.get_id() << std::endl;
    // std::cerr << "SITE extrap alpha: " << site.get_extrapolated_alpha() << std::endl;
    // std::cerr << "SITE measured alpha: " << site.get_alpha() << std::endl;

    _alpha_projected.at(i) = site.get_projected_alpha();
    _site.at(i) = site.get_id();
    _alpha_meas.at(i) = site.get_alpha();

    double pull = _alpha_meas.at(i) - _alpha_projected.at(i);
    double alpha_smooth = get_smoothed_measurement(site);
    double pull2 = _alpha_meas.at(i) - alpha_smooth;
    std::cerr << "MONITOR: " << _alpha_meas.at(i) << " " << alpha_smooth << std::endl;
    // std::cerr << "PULL: " << _alpha_meas.at(i) << " " << _alpha_extrap.at(i) << std::endl;
    TMatrixD a(5, 1);
    a = site.get_a();
    TMatrixD C(5, 5);

    TMatrixD a_smooth(5, 1);
    a_smooth = site.get_smoothed_a();
    C = site.get_covariance_matrix();
    double res_x = site.get_residual_x();
    double res_y = site.get_residual_y();
    // MC position and momentum.
    double mc_x  = site.get_true_position().x();
    double mc_y  = site.get_true_position().y();
    double mc_px = site.get_true_momentum().x();
    double mc_py = site.get_true_momentum().y();
    double mc_pz = site.get_true_momentum().z();
    // double observed = pow(pow(a(0, 0), 2.)+pow(a(1, 0), 2.), 0.5);
    // double expected = pow(pow(a_filt(0, 0), 2.)+pow(a_filt(1, 0), 2.), 0.5);
    // double chi2 = pow(observed-expected, 2.)/expected;

    TMatrixD a_projected(5, 1);
    a_projected = site.get_projected_a();
    double pr_x0 = a_projected(0, 0);
    double pr_y0 = a_projected(1, 0);
    double pr_mx = a_projected(2, 0);
    double pr_my = a_projected(3, 0);

    std::ofstream out2("kalman_mc.txt", std::ios::out | std::ios::app);
    int id = site.get_id();
    out2 << a(0, 0)    << " " << C(0, 0) << " "
         << a(1, 0)    << " " << C(1, 1) << " "
         << res_x      << " " << res_y << " "
         << a_smooth(0, 0) << " " << a_smooth(1, 0) << " "
         << pr_x0+pr_mx*site.get_z() << " " << pr_y0+pr_my*site.get_z() << " "
         << mc_x << " " << mc_y << " " << mc_px << " " << mc_py << " " << mc_pz << " "
         << pull << " " << pull2 << " " << id     << "\n";
    out2.close();
  }
}

void KalmanMonitor::save_global_track(std::vector<KalmanSite> const &sites) {
  int numb_sites = sites.size();
  _alpha_meas.resize(numb_sites);
  _site.resize(numb_sites);
  _alpha_projected.resize(numb_sites);

  for ( int i = 0; i < numb_sites; ++i ) {
    KalmanSite site = sites[i];
    // std::cerr << "SITE ID: " << site.get_id() << std::endl;
    // std::cerr << "SITE extrap alpha: " << site.get_extrapolated_alpha() << std::endl;
    // std::cerr << "SITE measured alpha: " << site.get_alpha() << std::endl;

    _alpha_projected.at(i) = site.get_projected_alpha();
    _site.at(i) = site.get_id();
    _alpha_meas.at(i) = site.get_alpha();

    double pull = _alpha_meas.at(i) - _alpha_projected.at(i);
    double alpha_smooth = get_smoothed_measurement(site);
    double pull2 = _alpha_meas.at(i) - alpha_smooth;
    // std::cerr << "PULL: " << _alpha_meas.at(i) << " " << _alpha_extrap.at(i) << std::endl;
    TMatrixD a(5, 1);
    a = site.get_a();
    TMatrixD C(5, 5);
    C = site.get_covariance_matrix();

    TMatrixD a_smooth(5, 1);
    a_smooth = site.get_smoothed_a();
    double res_x = site.get_residual_x();
    double res_y = site.get_residual_y();
    // double observed = pow(pow(a(0, 0), 2.)+pow(a(1, 0), 2.), 0.5);
    // double expected = pow(pow(a_filt(0, 0), 2.)+pow(a_filt(1, 0), 2.), 0.5);
    // double chi2 = pow(observed-expected, 2.)/expected;
    std::ofstream out2("kalman.txt", std::ios::out | std::ios::app);
    int id = site.get_id();
    out2 << a(0, 0)    << " " << C(0, 0) << " "
         << a(1, 0)    << " " << C(1, 1) << " "
         << res_x      << " " << res_y << " "
         << a_smooth(0, 0) << " " << a_smooth(1, 0) << " "
         << pull << " " << pull2 << " " << id     << "\n";
    out2.close();
  }
}
double KalmanMonitor::get_smoothed_measurement(KalmanSite &a_site) {
  Hep3Vector dir = a_site.get_direction();
  double dx = dir.x();
  double dy = dir.y();
  static const double A = 2./(7.*0.427);

  TMatrixD H(2, 5);
  H.Zero();
  H(0, 0) = - A*dy;
  H(0, 1) =  A*dx;

  TMatrixD a_smooth(5, 1);
  a_smooth = a_site.get_smoothed_a();
  TMatrixD ha(2, 1);
  ha = TMatrixD(H, TMatrixD::kMult, a_smooth);
  // Extrapolation converted to expected measurement.
  double alpha_smooth = ha(0, 0);
  return alpha_smooth;
}
/*
  //////////////////////////////////////////////////////////////////////////
  std::cout <<  "******************* STATE VECTOR *******************" << std::endl;
  std::cout << "predicted: ";
  a.Print();
  std::cout << "filtered ";
  a_filt.Print();
  std::cout <<  "******************* COVARIANCE MATRIX *******************" << std::endl;
 // std::cout << "predicted: ";
 // C.Invert().Print();
  std::cout << "updated ";
  C.Print();
  std::cout <<  "******************* OTHER MATRICES*******************" << std::endl;
  //std::cout << "H";
  //_H.Print();
  std::cout << "Measurement: ";
  m.Print();
  std::cout << "ha= aH";
  ha.Print();
  std::cout << "Pull: ";
  pull.Print();
  std::cout << "K: ";
  K.Print();
*/
