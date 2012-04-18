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


#ifndef KALMANSITE_HH
#define KALMANSITE_HH

// C headers
#include <assert.h>

// C++ headers
#include <string>
#include <vector>
#include "CLHEP/Vector/ThreeVector.h"
// #include "/home/edward/boost_1_49_0/boost/numeric/ublas/matrix.hpp"
// #include "/home/edward/boost_1_49_0/boost/numeric/ublas/lu.hpp"
#include "TMath.h"
#include "TMatrixD.h"

#include "src/common_cpp/Recon/SciFi/SciFiCluster.hh"

// namespace ublas = boost::numeric::ublas;

class KalmanSite {
 public:
  KalmanSite();

  ~KalmanSite();

  KalmanSite(const KalmanSite &site);

  KalmanSite& operator=(const KalmanSite& site);

  void set_state_vector(double x0,
                        double y0,
                        double mx,
                        double my,
                        double p) { _a(0, 0) = x0;
                                    _a(1, 0) = y0;
                                    _a(2, 0) = mx;
                                    _a(3, 0) = my;
                                    _a(4, 0) = p; }

  void set_state_vector(TMatrixD a) { _a = a; }

  TMatrixD get_state_vector() const { return _a; }

  void set_projected_state_vector(TMatrixD ap) { _projected_a = ap; }

  TMatrixD get_projected_state_vector() const { return _projected_a; }

  void set_covariance_matrix(TMatrixD C) { _C = C; }

  TMatrixD get_covariance_matrix() const { return _C; }

  void set_projected_covariance_matrix(TMatrixD Cp) { _projected_C = Cp; }

  TMatrixD get_projected_covariance_matrix() const { return _projected_C; }

  void set_measurement(double alpha) { _v(0, 0) = alpha;
                                       _v(1, 0) = 0.0; }

  TMatrixD get_measurement() const { return _v; }

  void set_direction(Hep3Vector dir) { _direction = dir; }

  Hep3Vector get_direction() const { return _direction; }

  void set_z(double z) { _z = z; }

  double get_z() const { return _z; }

  void set_id(int id) { _id = id; }

  int get_id() const { return _id; }

  void set_extrapolated_alpha(double alpha) { _alpha_extrapolated = alpha; }

  double get_extrapolated_alpha() const { return _alpha_extrapolated; }

  //void set_alpha(double alpha) { _alpha = alpha; }

  double get_alpha() const { return _v(0, 0); }

 private:
  TMatrixD _a;  // state vector

  TMatrixD _projected_a;

  TMatrixD _v;  // measurement

  TMatrixD _C;  // covariance matrix

  TMatrixD _projected_C;

  double _z, _alpha, _alpha_extrapolated;

  int _id;

  Hep3Vector _direction;
};

#endif
