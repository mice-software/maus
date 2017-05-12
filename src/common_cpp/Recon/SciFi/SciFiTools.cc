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

#include <CLHEP/Units/PhysicalConstants.h>
#include "src/common_cpp/Recon/SciFi/SciFiTools.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"

namespace SciFiTools {

void calc_straight_residual(const MAUS::SciFiSpacePoint *sp,
                                        const MAUS::SimpleLine &line_x,
                                        const MAUS::SimpleLine &line_y, double &dx, double &dy) {

    MAUS::ThreeVector pos = sp->get_position();
    dx = pos.x() - ( line_x.get_c() + ( pos.z() * line_x.get_m() ) );
    dy = pos.y() - ( line_y.get_c() + ( pos.z() * line_y.get_m() ) );
} // ~calc_straight_residual(...)

double calc_circle_residual(const MAUS::SciFiSpacePoint *sp, double xc, double yc, double r) {
    MAUS::ThreeVector pos = sp->get_position();
    double delta = sqrt(((pos.x()-xc) * (pos.x()-xc)) + ((pos.y()-yc) * (pos.y()-yc))) - r;
    return delta;
} // ~calc_circle_residual(...)

double calc_phi(double xpos, double ypos, const MAUS::SimpleCircle &circle) {
    // Note this function returns phi_i + phi_0, unless using x0, y0 in which case it returns phi_0
    double angle = atan2(ypos - circle.get_y0(), xpos - circle.get_x0());
    if ( angle < 0. ) angle += 2. * CLHEP::pi; // TODO is this ok if have different sign particles?
    // std::cerr << "calc_phi: x is " << xpos << ", y is " << ypos << ", R is " << circle.get_R();
    // std::cerr << ", X0 is " << circle.get_x0() << ", Y0 is " << circle.get_y0();
    // std::cerr << ", phi is " << angle << "\n";
    return angle;
} // ~calculate_phi(...)

bool calc_xy_pulls(MAUS::SciFiHelicalPRTrack* trk) {
  if (!trk)
    return NULL;
  for (auto sp : trk->get_spacepoints_pointers()) {
    double delta = calc_circle_residual(sp, trk->get_circle_x0(),
                                        trk->get_circle_y0(), trk->get_R());
    sp->set_prxy_pull(delta);
  }
  return true;
}

void draw_line(const MAUS::SciFiSpacePoint *sp1, const MAUS::SciFiSpacePoint *sp2,
                           MAUS::SimpleLine &line_x, MAUS::SimpleLine &line_y) {
  MAUS::ThreeVector pos_outer = sp1->get_position();
  MAUS::ThreeVector pos_inner = sp2->get_position();
  line_x.set_m(( pos_inner.x() - pos_outer.x()) / (pos_inner.z() - pos_outer.z() ));
  line_x.set_c(pos_outer.x() - ( pos_outer.z() * line_x.get_m()) );
  line_y.set_m(( pos_inner.y() - pos_outer.y()) / (pos_inner.z() - pos_outer.z() ));
  line_y.set_c(pos_outer.y() - ( pos_outer.z() *  line_y.get_m() ));
} // ~draw_line(...)

double det3by3(const TMatrixD &m) {
  if ( m.GetNrows() == 3 && m.GetNcols() == 3 ) {
    double det = m(0, 0)*(m(1, 1)*m(2, 2)-m(1, 2)*m(2, 1)) +
                 m(0, 1)*(m(1, 2)*m(2, 0)-m(1, 0)*m(2, 2)) +
                 m(0, 2)*(m(1, 0)*m(2, 1)-m(1, 1)*m(2, 0));
    return det;
  } else {
    std::cerr << "Error: SciFiTools::det3by3 says: Bad matrix given" << std::endl;
    return 0;
  }
} // ~det3by3(...)

MAUS::SimpleCircle make_3pt_circle(const MAUS::SciFiSpacePoint *sp1,
                                               const MAUS::SciFiSpacePoint *sp2,
                                               const MAUS::SciFiSpacePoint *sp3) {
  MAUS::SimpleCircle c;
  MAUS::ThreeVector pos1 = sp1->get_position();
  MAUS::ThreeVector pos2 = sp2->get_position();
  MAUS::ThreeVector pos3 = sp3->get_position();

  // Calculate alpha
  Double_t arr_a[] = {pos1.x(), pos1.y(), 1.0,
                      pos2.x(), pos2.y(), 1.0,
                      pos3.x(), pos3.y(), 1.0};
  TMatrixD mat_a(3, 3, arr_a);
  double alpha = det3by3(mat_a);
  c.set_alpha(alpha);

  // Calculate beta
  Double_t arr_b[] = {(pos1.x()*pos1.x())+(pos1.y()*pos1.y()), pos1.y(), 1.0,
                      (pos2.x()*pos2.x())+(pos2.y()*pos2.y()), pos2.y(), 1.0,
                      (pos3.x()*pos3.x())+(pos3.y()*pos3.y()), pos3.y(), 1.0};
  TMatrixD mat_b(3, 3, arr_b);
  double beta = - det3by3(mat_b);
  c.set_beta(beta);

  // Calculate gamma
  Double_t arr_g[] = {(pos1.x()*pos1.x())+(pos1.y()*pos1.y()), pos1.x(), 1.0,
                      (pos2.x()*pos2.x())+(pos2.y()*pos2.y()), pos2.x(), 1.0,
                      (pos3.x()*pos3.x())+(pos3.y()*pos3.y()), pos3.x(), 1.0};
  TMatrixD mat_g(3, 3, arr_g);
  double gamma = det3by3(mat_g);
  c.set_gamma(gamma);

  // Calculate kappa
  Double_t arr_k[] = {(pos1.x()*pos1.x())+(pos1.y()*pos1.y()), pos1.x(), pos1.y(),
                      (pos2.x()*pos2.x())+(pos2.y()*pos2.y()), pos2.x(), pos2.y(),
                      (pos3.x()*pos3.x())+(pos3.y()*pos3.y()), pos3.x(), pos3.y()};
  TMatrixD mat_k(3, 3, arr_k);
  double kappa = - det3by3(mat_k);
  c.set_kappa(kappa);

  // Calculate the dependent parameters
  double x0 = - beta / (2 * alpha);
  c.set_x0(x0);

  double y0 = - gamma / (2 * alpha);
  c.set_y0(y0);

  double R = sqrt(((beta*beta+gamma*gamma)/(4*alpha*alpha))-(kappa/alpha));
  c.set_R(R);

  return c;
} // ~make_3pt_circle

double my_mod(const double num, const double denom) {
  return num - (denom * floor(num / denom));
} // ~my_mod(...)

std::vector<double> phi_to_s(const double R, const std::vector<double> &phi) {
  std::vector<double> s_i;
  for ( int i = 0; i < static_cast<int>(phi.size()); ++i ) {
    s_i.push_back(phi[i] * R);
  }
  return s_i;
} // ~phi_to_s(...)

int num_stations_with_unused_spnts(const MAUS::SpacePoint2dPArray &spnts_by_station) {
  int stations_hit = 0;
  for ( int i = 0; i < static_cast<int>(spnts_by_station.size()); ++i ) {
    int unused_sp = 0;
    for ( int j = 0; j < static_cast<int>(spnts_by_station[i].size()); ++j ) {
      if ( !spnts_by_station[i][j]->get_used() ) {
        ++unused_sp;
      }
    }
    if ( unused_sp > 0 ) {
      ++stations_hit;
    }
  }
  return stations_hit;
} // ~num_stations_with_unused_spnts(...)

void print_spacepoint_xyz(const std::vector<MAUS::SciFiSpacePoint*> &spnts) {
  for ( size_t i = 0; i < spnts.size(); ++i ) {
    MAUS::ThreeVector pos = spnts[i]->get_position();
    std::cout << "( " << pos.x() << ", " << pos.y() << ", " << pos.z() << ")  ";
  }
  std::cout << std::endl;
} // print_spacepoint_xyz(...)

void sort_by_station(const std::vector<MAUS::SciFiSpacePoint*> &spnts,
                     MAUS::SpacePoint2dPArray &spnts_by_station) {
  for ( int st_num = 0; st_num < static_cast<int>(spnts_by_station.size()); ++st_num ) {
    for ( int i = 0; i < static_cast<int>(spnts.size()); ++i ) {
      if ( spnts[i]->get_station() == st_num + 1 ) {
        spnts_by_station[st_num].push_back(spnts[i]);
      }
    }
  }
} // ~sort_by_station(...)

MAUS::SpacePoint2dPArray sort_by_tracker(const MAUS::SciFiSpacePointPArray &spnts) {
  const int n_trackers = 2;
  MAUS::SpacePoint2dPArray spnts_by_tracker(n_trackers);
  for ( int trker_no = 0; trker_no < n_trackers; ++trker_no ) {
    for ( size_t i = 0; i < spnts.size(); ++i ) {
      if ( spnts[i]->get_tracker() == trker_no ) {
        spnts_by_tracker[trker_no].push_back(spnts[i]);
      }
    }
  }
  return spnts_by_tracker;
} // ~sort_by_tracker(...)

void stations_with_unused_spnts(const MAUS::SpacePoint2dPArray &spnts_by_station,
                                            std::vector<int> &stations_hit,
                                            std::vector<int> &stations_not_hit) {
  stations_hit.clear();
  stations_not_hit.clear();
  for ( int i = 0; i < static_cast<int>(spnts_by_station.size()); ++i ) {
    int unused_sp = 0;
    for ( int j = 0; j < static_cast<int>(spnts_by_station[i].size()); ++j ) {
      if ( !spnts_by_station[i][j]->get_used() ) {
        ++unused_sp;
      }
    }
    if ( unused_sp > 0 ) {
      stations_hit.push_back(i);
    } else if ( unused_sp == 0 ) {
      stations_not_hit.push_back(i);
    }
  }
} // ~stations_with_unused_spnts(...)

} // ~namespace SciFiTools
