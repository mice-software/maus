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
#ifndef  SEEDFINDER_HH
#define SEEDFINDER_HH

#include <vector>
#include <algorithm>
#include "TTree.h"
#include "TH1I.h"
#include "TFile.h"

#include "CLHEP/Matrix/Matrix.h"
#include "src/common_cpp/Recon/SciFi/SciFiSpacePoint.hh"
#include "src/common_cpp/Recon/SciFi/SciFiEvent.hh"

class SeedFinder {
 public:
  SeedFinder();

  ~SeedFinder();

  bool process(std::vector<SciFiSpacePoint> spacepoints);

  void momentumFromPoints(SciFiSpacePoint pnt1, SciFiSpacePoint pnt2, SciFiSpacePoint pnt3);

  void determineCentre(Hep3Vector& p1, Hep3Vector& p2, Hep3Vector& p3);

  void determinePtFromR();

  void determinePhi(Hep3Vector& pos, double& phi);

  void print_to_file();

  void print_event_info();

  void set_x0(double x0) { _x0 = x0; }
  double get_x0() const { return _x0; }

  void set_y0(double y0) { _y0 = y0; }
  double get_y0() const { return _y0; }

  void set_r(double r) { _r = r; }
  double get_r() const { return _r; }

  void set_pt(double pt) { _pt = pt; }
  double get_pt() const { return _pt; }

  void set_pz(double pz) { _pz = pz; }
  double get_pz() const { return _pz; }

  void set_phi_0(double phi_0) { _phi_0 = phi_0; }
  double get_phi_0() const { return _phi_0; }

  void set_tan_lambda(double tan_lambda) { _tan_lambda = tan_lambda; }
  double get_tan_lambda() const { return _tan_lambda; }

 private:
  int _tracker;

  static const double pi = 3.141592654;

  double _B, _Q, _p;
  // these 2 should come from datacards
  double _sigx;

  double _sigy;

  /// circle parameters
  double _x0, _y0, _r, _phi_0, _tan_lambda;

  ///
  double _pt, _px, _py, _pz;

  double _direction;

  double _sigpx, _sigpy, _sigpz, _sigr, _sigpt;

  double value;
};

#endif
