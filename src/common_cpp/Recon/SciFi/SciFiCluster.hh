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

/** @class SciFiCluster
 *  Digitize events by running Tracker electronics simulation.
 *
 */

#ifndef SCIFICLUSTER_HH
#define SCIFICLUSTER_HH
// C headers
#include <assert.h>
#include <json/json.h>
#include <CLHEP/Random/RandPoisson.h>
#include <CLHEP/Random/RandGauss.h>
#include <CLHEP/Random/RandExponential.h>
#include <CLHEP/Vector/ThreeVector.h>

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
#include "src/common_cpp/Recon/SciFi/SciFiDigit.hh"
#include "src/legacy/Config/MiceModule.hh"

// namespace MAUS {

class SciFiCluster {
 public:
  SciFiCluster(); // Default constructor

  /** @brief Constructs cluster from a single digit.
   */
  explicit SciFiCluster(SciFiDigit *digit);

  ~SciFiCluster();

  /** @brief Performs the cluster merging.
  */
  void add_digit(SciFiDigit* neigh);

  SciFiCluster(const SciFiCluster &cluster);

  SciFiCluster& operator=(const SciFiCluster &cluster);

  // list of setters and getters
  void set_spill(int spill) { _spill = spill; }

  int get_spill()   const { return _spill; }

  void set_event(int event) { _event = event; }

  int get_event() const { return _event; }

  void set_tracker(int trackerNo) { _tracker = trackerNo; }

  int get_tracker() const { return _tracker; }

  void set_station(int stationNo) { _station = stationNo; }

  int get_station() const { return _station; }

  void set_plane(int planeNo) { _plane = planeNo; }

  int get_plane()  const  { return _plane;   }

  void set_npe(double nPE) { _npe = nPE; }

  double get_npe()     const { return _npe; }

  void set_time(double atime) { _time = atime; }

  double get_time()    const { return _time; }

  void set_channel(double channelNo_w) { _channel_w = channelNo_w; }

  double get_channel()  const { return _channel_w; }

  void set_used() { _used = true; }

  bool is_used() const { return _used; }

  void set_direction(Hep3Vector direction) { _direction = direction; }

  Hep3Vector get_direction() const { return _direction; }

  void set_position(Hep3Vector position) { _position = position; }

  Hep3Vector get_position() const { return _position; }

  void set_relative_position(Hep3Vector rel_pos) { _relat_pos = rel_pos; }

  void set_alpha(double channel) { _alpha = channel; }

  Hep3Vector get_relative_position() const { return _relat_pos; }

  double get_alpha() const { return _alpha; }

  void set_id(int id) { _id = id; }

  int get_id() const { return _id; }

  void set_true_momentum(Hep3Vector p) { _p = p; }

  Hep3Vector get_true_momentum() const { return _p; }

  void set_true_position(Hep3Vector position) { _pos = position; }

  Hep3Vector get_true_position() const { return _pos; }

  // void set_digits(std::vector<SciFiDigit*> digits) { _digits = digits; }

  // std::vector<SciFiDigit*> get_digits()  const { return _digits; }

 private:
  Hep3Vector _pos, _p;

  bool _used;

  int _spill, _event;

  int _tracker, _station, _plane;

  double _channel_w, _npe, _time;

  Hep3Vector _direction;

  Hep3Vector _position;

  Hep3Vector _relat_pos;

  double _alpha;

  int _id;

  // std::vector<SciFiDigit*>  _digits;
};  // Don't forget this trailing colon!!!!
// } // ~namespace MAUS

#endif
