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

/** @class SciFiDigit
 *  Digitize events by running Tracker electronics simulation.
 */

#ifndef SCIFIDIGIT_HH
#define SCIFIDIGIT_HH
// C headers
#include <assert.h>
#include <json/json.h>

#include <CLHEP/Random/RandPoisson.h>
#include <CLHEP/Random/RandGauss.h>
#include <CLHEP/Random/RandExponential.h>
#include <CLHEP/Units/PhysicalConstants.h>

// C++ headers
#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

// namespace MAUS {

class SciFiDigit {
 public:
  SciFiDigit(); // Default constructor

  /** @brief Constructs a scifi digit initializing member variables.
   */
  SciFiDigit(int tracker, int station, int plane, int channel, double npe, double time );

  ~SciFiDigit();

//  void set_spill(int spillNo) { _spill = spillNo; }

//  int get_spill()   const { return _spill;   }

//  void set_eventNo(int eventNo) { _eventNo = eventNo; }

//  int get_eventNo() const { return _eventNo; }

  // list of getters and setters.
  void set_tracker(int trackerNo) { _tracker = trackerNo; }

  int get_tracker() const { return _tracker; }

  void set_station(int stationNo) { _station = stationNo; }

  int get_station() const { return _station; }

  void set_plane(int planeNo) { _plane = planeNo; }

  int get_plane()   const { return _plane; }

  void set_channel(int channelNo) { _channel = channelNo; }

  int get_channel() const { return _channel; }

  void set_npe(double npe) { _npe = npe; }

  double get_npe()     const { return _npe;     }

  void set_time(double time) { _time = time; }

  double get_time()    const { return _time;    }

  void set_used()   { _used = true; }

  bool is_used()     const { return _used; }

 private:

  // int _spill, _eventNo;

  int _tracker, _station, _plane, _channel;

  double _npe, _time;

  bool _used;
};  // Don't forget this trailing colon!!!!
// } // ends namespace MAUS

#endif
