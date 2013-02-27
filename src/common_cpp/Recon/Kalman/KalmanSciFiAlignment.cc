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

// TODO: Find a proper way to save
//       misalignments and respective errors.
//

#include "src/common_cpp/Recon/Kalman/KalmanSciFiAlignment.hh"

namespace MAUS {

KalmanSciFiAlignment::KalmanSciFiAlignment() {
  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
  file = "SciFiMisalignments";
  fname = std::string(pMAUS_ROOT_DIR)+"/src/map/MapCppTrackerRecon/"+file;

  for ( int i = 0; i < 30; ++i ) {
    shifts_array[i].     ResizeTo(3, 1);
    covariance_shifts[i].ResizeTo(3, 3);
    shifts_array[i].     Zero();
    covariance_shifts[i].Zero();
  }
}

KalmanSciFiAlignment::~KalmanSciFiAlignment() {}

bool KalmanSciFiAlignment::load_misaligments() {
  std::ifstream inf(fname.c_str());
  if (!inf) {
    throw(Squeal(Squeal::recoverable,
          "Could not load misalignments.",
          "KalmanSciFiAlignment::load_misaligments"));
  }

  std::string line;
  // Titles line.
  double station, xd, yd, zd;
  double s_xx, s_xy, s_xz;
  double s_yx, s_yy, s_yz;
  double s_zx, s_zy, s_zz;
  int size = 11;

  getline(inf, line);
  for ( int line_i = 1; line_i < size; line_i++ ) {
    getline(inf, line);
    std::istringstream ist1(line.c_str());
    ist1 >> station
         >> xd >> s_xx >> s_xy >> s_xz
         >> yd >> s_yx >> s_yy >> s_yz
         >> zd >> s_zx >> s_zy >> s_zz;

    assert(line_i == station && "SciFiMisalignments (Shifts) set up as expected.");
    int site_id = 3*(station-1); // 0, 3, 6, 9, 12

    // Shifts.
    TMatrixD shifts(3, 1);
    shifts(0, 0) = xd;
    shifts(1, 0) = yd;
    shifts(2, 0) = zd;
    shifts_array[site_id]   = shifts;
    shifts_array[site_id+1] = shifts;
    shifts_array[site_id+2] = shifts;
    // Their covariance.
    TMatrixD cov_s(3, 3);
    cov_s(0, 0) = s_xx;
    cov_s(0, 1) = s_xy;
    cov_s(0, 2) = s_xz;
    cov_s(1, 0) = s_yx;
    cov_s(1, 1) = s_yy;
    cov_s(1, 2) = s_yz;
    cov_s(2, 0) = s_zx;
    cov_s(2, 1) = s_zy;
    cov_s(2, 2) = s_zz;
    covariance_shifts[site_id]   = cov_s;
    covariance_shifts[site_id+1] = cov_s;
    covariance_shifts[site_id+2] = cov_s;
  }

  inf.close();
  return true;
}

void KalmanSciFiAlignment::update(KalmanSite site) {
  int id = site.get_id();
  set_shifts(site.get_shift(), id);
  set_cov_shifts(site.get_shift_covariance(), id);
}

void KalmanSciFiAlignment::save() {
  std::ofstream file_out(fname.c_str());
  // Write shifts.
  file_out << "# station" << "\t" << "xd" << "\t"
           << "s_xd_xd" << "\t" << "s_xd_yd" << "\t"
           << "s_xd_zd" << "\t" << "yd" << "\t"
           << "s_yd_xd" << "\t" << "s_yd_yd" << "\t"
           << "s_yd_zd" << "\t" << "zd" << "\t"
           << "s_zd_xd" << "\t" << "s_zd_yd" << "\t"
           << "s_zd_zd \n";
      file_out << 1 << "\t"
      << 0. << "\t"
      << 0.1 << "\t" << 0. << "\t" << 0. << "\t"
      << 0. << "\t"
      << 0. << "\t" << 0.1 << "\t" << 0. << "\t"
      << 0. << "\t"
      << 0. << "\t" << 0. << "\t" << 0.1 << "\n";

  // sites 2 to 29; increment 3
  for ( int station = 2; station < 5; station++ ) {
      int site_i = 3*(station)-1; // j==5 || j==8 || j==11
      file_out << station << "\t"
      << shifts_array[site_i](0, 0) << "\t"
      << covariance_shifts[site_i](0, 0) << "\t"
      << covariance_shifts[site_i](0, 1) << "\t"
      << covariance_shifts[site_i](0, 2) << "\t"
      << shifts_array[site_i](1, 0) << "\t"
      << covariance_shifts[site_i](1, 0) << "\t"
      << covariance_shifts[site_i](1, 1) << "\t"
      << covariance_shifts[site_i](1, 2) << "\t"
      << shifts_array[site_i](2, 0) << "\t"
      << covariance_shifts[site_i](2, 0) << "\t"
      << covariance_shifts[site_i](2, 1) << "\t"
      << covariance_shifts[site_i](2, 2) << "\n";
  }
      file_out << 5 << "\t"
      << 0. << "\t"
      << 0.1 << "\t" << 0. << "\t" << 0. << "\t"
      << 0. << "\t"
      << 0. << "\t" << 0.1 << "\t" << 0. << "\t"
      << 0. << "\t"
      << 0. << "\t" << 0. << "\t" << 0.1 << "\n";

      file_out << 6 << "\t"
      << 0. << "\t"
      << 0.1 << "\t" << 0. << "\t" << 0. << "\t"
      << 0. << "\t"
      << 0. << "\t" << 0.1 << "\t" << 0. << "\t"
      << 0. << "\t"
      << 0. << "\t" << 0. << "\t" << 0.1 << "\n";

  for ( int station = 7; station < 10; station++ ) {
      int site_i = 3*(station)-1;
      file_out << station << "\t"
      << shifts_array[site_i](0, 0) << "\t"
      << covariance_shifts[site_i](0, 0) << "\t"
      << covariance_shifts[site_i](0, 1) << "\t"
      << covariance_shifts[site_i](0, 2) << "\t"
      << shifts_array[site_i](1, 0) << "\t"
      << covariance_shifts[site_i](1, 0) << "\t"
      << covariance_shifts[site_i](1, 1) << "\t"
      << covariance_shifts[site_i](1, 2) << "\t"
      << shifts_array[site_i](2, 0) << "\t"
      << covariance_shifts[site_i](2, 0) << "\t"
      << covariance_shifts[site_i](2, 1) << "\t"
      << covariance_shifts[site_i](2, 2) << "\n";
  }
      file_out << 10 << "\t"
      << 0. << "\t"
      << 0.1 << "\t" << 0. << "\t" << 0. << "\t"
      << 0. << "\t"
      << 0. << "\t" << 0.1 << "\t" << 0. << "\t"
      << 0. << "\t"
      << 0. << "\t" << 0. << "\t" << 0.1 << "\n";

  file_out.close();
}
} // ~namespace MAUS
