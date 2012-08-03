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


#ifndef HELICALTRACK_HH
#define HELICALTRACK_HH

// C headers
#include <assert.h>
#include <math.h>

// C++ headers
#include <string>
#include <vector>

#include "TMath.h"
#include "TMatrixD.h"

// #include "src/common_cpp/Recon/Kalman/KalmanSite.hh"
#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"
#include "src/common_cpp/Recon/SciFi/SeedFinder.hh"

class HelicalTrack : public KalmanTrack {
 public:
  explicit HelicalTrack(SciFiHelicalPRTrack const &seed);

  void update_propagator(KalmanSite *old_site, KalmanSite *new_site);

  void calc_system_noise(KalmanSite *site);

  double get_x0() const { return _x0; }

  double get_y0() const { return _y0; }

  double get_r() const { return _r; }

  double get_projected_x() const { return _projected_x; }

  double get_projected_y() const { return _projected_y; }

 private:
  double _x0, _y0, _r;

  double _B, _q;

  int _sign, _h;

  static const double _alpha;

  double _projected_x;

  double _projected_y;

  // static const double ALPHA = -1.0/(300000000.*4.);
};

#endif
