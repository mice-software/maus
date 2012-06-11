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

/** @class SciFiHit
 *  Hitize events by running Tracker electronics simulation.
 *
 */

#ifndef SCIFIHit_HH
#define SCIFIHit_HH
// C headers
#include <assert.h>
#include <json/json.h>

#include <CLHEP/Random/RandPoisson.h>
#include <CLHEP/Random/RandGauss.h>
#include <CLHEP/Random/RandExponential.h>
#include <CLHEP/Units/PhysicalConstants.h>
#include "CLHEP/Vector/ThreeVector.h"

// C++ headers
#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

// namespace MAUS {

class SciFiHit {
 public:
  SciFiHit(); // Default constructor

  SciFiHit(int tracker, int station, int plane, int fibre, double edep, double time);

  ~SciFiHit();

  void set_spill(int spillNo) { _spill = spillNo; }

  int get_spill()   const { return _spill;   }

  void set_eventNo(int eventNo) { _eventNo = eventNo; }

  int get_eventNo() const { return _eventNo; }

  void set_tracker(int trackerNo) { _tracker = trackerNo; }

  int get_tracker() const { return _tracker; }

  void set_station(int stationNo) { _station = stationNo; }

  int get_station() const { return _station; }

  void set_plane(int planeNo) { _plane = planeNo; }

  int get_plane()   const { return _plane; }

  void set_fibre(int fibreNo) { _fibre = fibreNo; }

  int get_fibre() const { return _fibre; }

  void set_edep(double edep) { _edep = edep; }

  double get_edep()     const { return _edep;     }

  void set_time(double time) { _time = time; }

  double get_time()    const { return _time;    }

  void set_used()   { _used = true; }

  bool is_used()     const { return _used; }

  void set_true_momentum(CLHEP::Hep3Vector p) { _p = p; }

  CLHEP::Hep3Vector get_true_momentum() const { return _p; }

  void set_true_position(CLHEP::Hep3Vector  position) { _position = position; }

  CLHEP::Hep3Vector get_true_position() const { return _position; }

 private:
  CLHEP::Hep3Vector _position, _p;

  int _spill, _eventNo;

  int _tracker, _station, _plane, _fibre;

  double _edep, _time;

  bool _used;
};  // Don't forget this trailing colon!!!!
// } // ~namespace MAUS

#endif
