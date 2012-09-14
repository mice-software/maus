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

/** @class SECluster
 *  Digitize events by running Tracker electronics simulation.
 *
 */

#ifndef SECLUSTER_HH
#define SECLUSTER_HH
// C headers
#include <assert.h>
#include <json/json.h>
#include <CLHEP/Random/RandPoisson.h>
#include <CLHEP/Random/RandGauss.h>
#include <CLHEP/Random/RandExponential.h>

// C++ headers
#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

// other headers
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Vector/Rotation.h"

#include "src/legacy/Config/MiceModule.hh"
#include "src/common_cpp/Recon/SingleStation/SEDigit.hh"

// namespace MAUS {

class SECluster {
 public:
  SECluster(); // Default constructor

  /** @brief Constructs cluster from a single digit.
   */
  explicit SECluster(SEDigit *digit);

  ~SECluster();

  /** @brief Performs the cluster merging.
  */
  void add_digit(SEDigit* neigh);

  void set_spill(int spill) { _spill = spill; }

  int get_spill()  const  { return _spill; }

  void set_event(int event) { _event = event; }

  int get_event()  const  { return _event;   }

  void set_plane(int planeNo) { _plane = planeNo; }

  int get_plane()  const  { return _plane;   }

  void set_npe(double nPE) { _npe = nPE; }

  double get_npe()     const { return _npe; }

  void set_time(double atime) { _time = atime; }

  double get_time()    const { return _time; }

  void set_channel(double channelNo_w) { _channel_w = channelNo_w; }

  double get_channel()  const { return _channel_w; }

  void set_alpha(double alpha) { _alpha = alpha; }

  double get_alpha()  const { return _alpha; }

  void set_used() { _used = true; }

  bool is_used() const { return _used; }

  void set_direction(Hep3Vector direction) { _direction = direction; }

  Hep3Vector get_direction() const { return _direction; }

  void set_position(Hep3Vector position) { _position = position; }

  Hep3Vector get_position() const { return _position; }

  void set_digits(std::vector<SEDigit*> digits) { _digits = digits; }

  std::vector<SEDigit*> get_digits()  const { return _digits; }

 private:
  int _alpha;

  int _spill, _event;

  bool _used;

  int _plane;

  double _channel_w, _npe, _time;

  Hep3Vector _direction;

  Hep3Vector _position;

  std::vector<SEDigit*>  _digits;
};  // Don't forget this trailing colon!!!!
// } // ~namespace MAUS

#endif
