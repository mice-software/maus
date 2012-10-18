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

#include "src/common_cpp/Recon/SingleStation/KalmanSSTrack.hh"

namespace MAUS {

KalmanSSTrack::~KalmanSSTrack() {}

//
// ------- Prediction ------------
//
void KalmanSSTrack::update_propagator(KalmanSite *old_site, KalmanSite *new_site) {
  // Find dz between sites.
  double new_z = new_site->get_z();
  double old_z = old_site->get_z();
  double deltaZ = new_z-old_z;

  // Reset.
  _F.Zero();
  bool magnets_on = true;
  if ( magnets_on && ( (old_site->get_id() == 4 &&  new_site->get_id() == 5) ||
                       (old_site->get_id() == 5 &&  new_site->get_id() == 4) ) ) {
    magnet_drift();
  } else {
    straight_line(deltaZ);
  }
}

void KalmanSSTrack::magnet_drift() {
  TMatrixD M_Q7(5, 5);
  M_Q7.Zero();
  TMatrixD M_0(5, 5);
  M_0.Zero();
  TMatrixD M_Q8(5, 5);
  M_Q8.Zero();
  TMatrixD M_1(5, 5);
  M_1.Zero();
  TMatrixD M_Q9(5, 5);
  M_Q9.Zero();

  double q7_current = 141.6; // A
  double q8_current = 214.3; // A
  double q9_current = 182.9; // A
  double momentum = _momentum;

  std::cerr << "momentum: " << _momentum << ", mass = " << _mass << "\n";

  double kappa_q7 = compute_kappa(q7_current, momentum); // 1.0;  // m2
  double sqrt_kappa_q7 = pow(kappa_q7, 0.5)/1000.; // m-1 -> mm-1
  double kappa_q8 = compute_kappa(q8_current, momentum); // 1.52; // m2
  double sqrt_kappa_q8 = pow(kappa_q8, 0.5)/1000.; // m-1 -> mm-1
  double kappa_q9 = compute_kappa(q9_current, momentum); // 1.3;  // m2
  double sqrt_kappa_q9 = pow(kappa_q9, 0.5)/1000.; // m-1 -> mm-1

  double L   = 660.; // mm
  double D_0 = 500.; // mm
  double D_1 = 500.; // mm

  for ( int i = 0; i < 5; i++ ) {
    M_0(i, i) = 1.;
    M_1(i, i) = 1.;
  }
  M_0(0, 1) = D_0;
  M_0(2, 3) = D_0;
  M_1(0, 1) = D_1;
  M_1(2, 3) = D_1;

  // Q 7 focusses in x...
  double phi_q7 = sqrt_kappa_q7*L;
  M_Q7(0, 0) = cos(phi_q7);
  M_Q7(0, 1) = sin(phi_q7)/sqrt_kappa_q7;
  M_Q7(1, 0) =-sin(phi_q7)*sqrt_kappa_q7;
  M_Q7(1, 1) = cos(phi_q7);
  // Q 7 de-focusses in y...
  M_Q7(2, 2) = cosh(phi_q7);
  M_Q7(2, 3) = sinh(phi_q7)/sqrt_kappa_q7;
  M_Q7(3, 2) = sqrt_kappa_q7*sinh(phi_q7);
  M_Q7(3, 3) = cosh(phi_q7);
  M_Q7(4, 4) = 1.;

  // Q 8 focusses in y...
  double phi_q8 = sqrt_kappa_q8*L;
  M_Q8(2, 2) = cos(phi_q8);
  M_Q8(2, 3) = sin(phi_q8)/sqrt_kappa_q8;
  M_Q8(3, 2) = -sin(phi_q8)*sqrt_kappa_q8;
  M_Q8(3, 3) = cos(phi_q8);
  // Q 8 de-focusses in x...
  M_Q8(0, 0) = cosh(phi_q8);
  M_Q8(0, 1) = sinh(phi_q8)/sqrt_kappa_q8;
  M_Q8(1, 0) = sqrt_kappa_q7*sinh(phi_q8);
  M_Q8(1, 1) = cosh(phi_q8);
  M_Q8(4, 4) = 1.;

  // Q 9 focusses in x...
  double phi_q9 = sqrt_kappa_q9*L;
  M_Q9(0, 0) = cos(phi_q9);
  M_Q9(0, 1) = sin(phi_q9)/sqrt_kappa_q9;
  M_Q9(1, 0) = -sin(phi_q9)*sqrt_kappa_q9;
  M_Q9(1, 1) = cos(phi_q9);
  // Q 9 de-focusses in y...
  M_Q9(2, 2) = cosh(phi_q9);
  M_Q9(2, 3) = sinh(phi_q9)/sqrt_kappa_q9;
  M_Q9(3, 2) = sqrt_kappa_q9*sinh(phi_q9);
  M_Q9(3, 3) = cosh(phi_q9);
  M_Q9(4, 4) = 1.;

  TMatrixD temp1(5, 5);
  temp1 = TMatrixD(M_Q7, TMatrixD::kMult, M_0);
  TMatrixD temp2(5, 5);
  temp2 = TMatrixD(temp1, TMatrixD::kMult, M_Q8);
  TMatrixD temp3(5, 5);
  temp3 = TMatrixD(temp2, TMatrixD::kMult, M_1);
  // F = M_Q7 * M_0 * M_Q8 * M_1 * M_Q9
  _F = TMatrixD(temp3, TMatrixD::kMult, M_Q9);
/*
  M_Q7.Print();
  M_0.Print();
  M_Q8.Print();
  M_1.Print();
  M_Q9.Print();
*/
}

double KalmanSSTrack::compute_kappa(double current, double momentum) {
  double c = 0.2998;
  double momentum_in_GeV_c = momentum/1000.;

  double gradient = (current+2e-13)/174.09; // T/m
  double kappa = c*gradient/momentum_in_GeV_c;
  return kappa;
}

void KalmanSSTrack::straight_line(double deltaZ) {
  for ( int i = 0; i < 5; i++ ) {
    _F(i, i) = 1.;
  }
  _F(0, 1) = deltaZ;
  _F(2, 3) = deltaZ;
}

void KalmanSSTrack::compute_chi2(const std::vector<KalmanSite> &sites) {
  int number_of_sites = sites.size();
  int number_parameters = 5;
  int number_of_measurement_sites = 7;
  int id = sites[0].get_id();
  _tracker = 1;

  double sum = sites[6].get_smoothed_alpha() +
               sites[7].get_smoothed_alpha() +
               sites[8].get_smoothed_alpha();

  std::cerr << "Spacepoint sum: " << sum << std::endl;
  if (sum < -4 && sum > 4 ) {
    std::cerr << "Bad space point!" << std::endl;
    _chi2+=9999.;
  }
  // double alpha, model_alpha;
  for ( int i = 0; i < number_of_sites; ++i ) {
    KalmanSite site = sites[i];
    std::cerr << site.get_chi2() << std::endl;
    _chi2 += site.get_chi2();
  }
  _ndf = number_of_measurement_sites - number_parameters;
  std::ofstream output("chi2.txt", std::ios::out | std::ios::app);
  output << _tracker << " " << _chi2 << " " << _ndf << "\n";
  output.close();

  // CM35 analysis.
  assert(sites[4].get_id() == 4);
  TMatrixD triplet_face_0(5, 1);
  triplet_face_0 = sites[4].get_a();
  double x  = triplet_face_0(0, 0);
  double xp = triplet_face_0(1, 0);
  double y  = triplet_face_0(2, 0);
  double yp = triplet_face_0(3, 0);
  double kappa = triplet_face_0(4, 0);
  double px = xp/kappa;
  double py = yp/kappa;

  double r = pow(x*x+y*y, 0.5);
  double tranvs = pow(px*px+py*py, 0.5);

  std::ofstream output2("emittance_chi2.txt", std::ios::out | std::ios::app);
  output2 << r << " " << tranvs << " " << _chi2 << " " << _ndf  << "\n";
  output2.close();
}

} // namespace MAUS
