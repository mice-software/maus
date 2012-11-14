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

KalmanMonitor::KalmanMonitor() {}

KalmanMonitor::~KalmanMonitor() {}

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

void KalmanMonitor::save(std::vector<KalmanSite> const &sites) {
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
    double alpha_smooth = site.get_smoothed_alpha(); // get_smoothed_measurement(site);
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
    assert(a_smooth(0, 0) ==  a_smooth(0, 0));
    assert(a_proj(0, 0) == a_proj(0, 0));

    out2 << a_proj(0, 0) << " " << a_proj(1, 0) << " " << a_proj(2, 0)
         << " " << a_proj(3, 0) << " " << a_proj(4, 0) << " "
         << a(0, 0) << " " << a(1, 0) << " " << a(2, 0) << " " << a(3, 0)
         << " " << a(4, 0) << " " << a_smooth(0, 0) << " " << a_smooth(1, 0) << " "
         << a_smooth(2, 0) << " " << a_smooth(3, 0) << " " << a_smooth(4, 0) << " "
         << mc_x << " " << mc_y << " " << mc_px << " " << mc_py << " " << mc_pz << " "
         << pull << " " << pull2 << " " << id     << "\n";
    out2.close();
  }
}

} // ~namespace MAUS

