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
  TGraph *gr = new TGraph(30, &(_site[0]), &(_alpha_extrap[0]));
  gr->SetName("extrapolation");
  TGraph *gr2 = new TGraph(30, &(_site[0]), &(_alpha_meas[0]));
  gr2->SetName("measurement");
  TMultiGraph *mgr = new TMultiGraph();
  mgr->Add(gr, "p");
  mgr->Add(gr2, "p");
  mgr->Write();
}

void KalmanMonitor::save(std::vector<KalmanSite> &sites) {
  int numb_sites = sites.size();
  _alpha_meas.resize(numb_sites);
  _site.resize(numb_sites);
  _alpha_extrap.resize(numb_sites);

  for ( int i = 0; i < numb_sites; ++i ) {
    KalmanSite site = sites[i];
    _alpha_extrap.push_back(site.get_extrapolated_alpha());
    _site.push_back(site.get_id());
    _alpha_meas.push_back(site.get_alpha());
    double pull = _alpha_meas[i] - _alpha_extrap[i];

    TMatrixD a(5, 1);
    a = site.get_state_vector();
    TMatrixD C(5, 5);
    C = site.get_covariance_matrix();
    // double observed = pow(pow(a(0, 0), 2.)+pow(a(1, 0), 2.), 0.5);
    // double expected = pow(pow(a_filt(0, 0), 2.)+pow(a_filt(1, 0), 2.), 0.5);
    // double chi2 = pow(observed-expected, 2.)/expected;
    std::ofstream out2("kalman.txt", std::ios::out | std::ios::app);
    int id = site.get_id();
    out2 << a(0, 0)    << " " << C(0, 0) << " "
         << a(1, 0)    << " " << C(1, 1) << " "
         << pull << " " << id     << "\n";
    out2.close();
  }
}
