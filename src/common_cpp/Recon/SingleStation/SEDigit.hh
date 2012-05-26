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

/** @class SEDigit
 *  Digitize events by running Tracker electronics simulation.
 */

#ifndef SEDIGIT_HH
#define SEDIGIT_HH
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

class SEDigit {
 public:
  SEDigit(); // Default constructor

  /** @brief Constructs a SE digit initializing member variables.
   */
  SEDigit(int spill, int event, int plane, int channel, double npe, double time, int adc);

  ~SEDigit();

  void set_spill(int spillNo) { _spill = spillNo; }

  int get_spill()   const { return _spill;   }

  void set_event(int event) { _event = event; }

  int get_event() const { return _event; }

  void set_plane(int planeNo) { _plane = planeNo; }

  int get_plane()   const { return _plane; }

  void set_channel(int channelNo) { _channel = channelNo; }

  int get_channel() const { return _channel; }

  void set_npe(double npe) { _npe = npe; }

  double get_npe()     const { return _npe; }

  void set_adc(double adc) { _adc = adc; }

  double get_adc()     const { return _adc; }

  void set_time(double time) { _time = time; }

  double get_time()    const { return _time;    }

  void set_used()   { _used = true; }

  bool is_used()     const { return _used; }

 private:

  int _spill, _event;
  int _adc;

  int _plane, _channel;

  double _npe, _time;

  bool _used;
};  // Don't forget this trailing colon!!!!
// } // ends namespace MAUS

#endif
