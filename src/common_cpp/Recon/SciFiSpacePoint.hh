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

/** @class SciFiSpacePoint
 *
 *
 */

#ifndef SCIFISPACEPOINT_HH
#define SCIFISPACEPOINT_HH

// C headers
#include <assert.h>
#include <json/json.h>

#include <CLHEP/Random/RandPoisson.h>
#include <CLHEP/Random/RandGauss.h>
#include <CLHEP/Random/RandExponential.h>
#include <CLHEP/Units/PhysicalConstants.h>

// C++ headers
#include <string>
#include <vector>

#include "src/common_cpp/Recon/SciFiCluster.hh"

class SciFiSpacePoint {
 public:
  SciFiSpacePoint(); // Default constructor

  SciFiSpacePoint(SciFiCluster *clust1, SciFiCluster *clust2, SciFiCluster *clust3);

  SciFiSpacePoint(SciFiCluster *clust1, SciFiCluster *clust2);

  ~SciFiSpacePoint();

  void set_tracker(double tracker) { _tracker = tracker; }

  int get_tracker() const { return _tracker; }

  void set_station(double station) { _station = station; }

  int get_station() const { return _station; }

  void set_npe(double nPE) { _npe = nPE; }

  double get_npe()     const { return _npe; }

  // void set_time(double time) { _time = time; }

  // double get_time()    const { return _time; }

  void set_type(std::string type) { _type = type; }

  std::string get_type()  const { return _type; }

  void set_position(Hep3Vector position) { _position = position; }

  Hep3Vector get_position() const { return _position; }

  void set_chi2(double chi2) { _chi2 = chi2; }

  double get_chi2()    const { return _chi2; }

  void set_used() { _used = true; }

  bool is_used() const { return _used; }

  void set_channels(std::vector<SciFiCluster*> channels) { _channels = channels; }

  std::vector<SciFiCluster*> get_channels()  const { return _channels; }

 private:
  int _tracker, _station;

  double _npe;

  Hep3Vector _position;

  int _chi2;

  bool _used;
  // int _time, _time_error, _time_res;

  std::string _type;

  std::vector<SciFiCluster*>  _channels;
};  // Don't forget this trailing colon!!!!

#endif
