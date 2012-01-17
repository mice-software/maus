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
#include "src/common_cpp/Recon/SciFiDigit.hh"
// #include "src/map/MapCppTrackerRecon/TrackerEvent.hh"

class SciFiCluster {
 public:
  // SciFiDigit(); // Default constructor

  explicit SciFiCluster(SciFiDigit *digit);

  ~SciFiCluster();

  void addDigit(SciFiDigit neigh);

  int get_tracker() const { return _tracker; }

  int get_station() const { return _station; }

  int get_plane()  const  { return _plane;   }

  double get_npe()     const { return _npe; }

  int get_time()    const { return _time; }

  double get_channel()  const { return _channel_w; }

  bool isUsed() const { return _used; }

  void setUsed() { _used = true; }

  void addDigit(SciFiDigit* neigh);

  Hep3Vector get_direction() const { return _direction; }

  Hep3Vector get_position() const { return _position; }

  void construct(std::vector<const MiceModule*> modules, SciFiDigit* digit);

 private:
  int _tracker, _station, _plane;

  double _npe, _channel_w;

  int _time;

  SciFiDigit* seed;

  SciFiDigit* neighbour;

  std::vector<SciFiDigit*> digits;

  int n_chan;

  bool _used;

  Hep3Vector _direction;

  Hep3Vector _position;
};  // Don't forget this trailing colon!!!!

#endif
