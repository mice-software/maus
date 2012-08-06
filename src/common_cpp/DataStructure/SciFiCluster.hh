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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_SCIFICLUSTER_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_SCIFICLUSTER_HH_

// C++ headers
#include <vector>

// ROOT headers
#include "Rtypes.h"

// MAUS headers
#include "src/common_cpp/DataStructure/SciFiDigit.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"


namespace MAUS {

class SciFiCluster {
 public:

  /** Default constructor - initialises to 0/NULL */
  SciFiCluster();

  /** Copy constructor - any pointers are deep copied */
  SciFiCluster(const SciFiCluster &_scificluster);

  /** Constructor - creates cluster from a single digit */
  explicit SciFiCluster(SciFiDigit *_scifidigit);

  /** Destructor - any member pointers are deleted */
  virtual ~SciFiCluster();

  /** Assignment operator - any pointers are deep copied */
  SciFiCluster& operator=(const SciFiCluster &_scificluster);

  /** Performs the cluster merging. */
  void add_digit(SciFiDigit* neigh);

  // Getters and Setters
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

  void set_direction(ThreeVector direction) { _direction = direction; }

  ThreeVector get_direction() const { return _direction; }

  void set_position(ThreeVector position) { _position = position; }

  ThreeVector get_position() const { return _position; }

  void set_relative_position(ThreeVector rel_pos) { _relat_pos = rel_pos; }

  void set_alpha(double channel) { _alpha = channel; }

  ThreeVector get_relative_position() const { return _relat_pos; }

  double get_alpha() const { return _alpha; }

  void set_id(int id) { _id = id; }

  int get_id() const { return _id; }

  void set_true_momentum(ThreeVector p) { _p = p; }

  ThreeVector get_true_momentum() const { return _p; }

  void set_true_position(ThreeVector position) { _pos = position; }

  ThreeVector get_true_position() const { return _pos; }

 private:
  bool _used;

  int _spill, _event, _tracker, _station, _plane, _id;

  double _channel_w, _npe, _time, _alpha;

  ThreeVector _pos, _p;
  ThreeVector _direction, _position, _relat_pos;

  ClassDef(SciFiCluster, 1)
}; // Don't forget this trailing colon!!!!

typedef std::vector<SciFiCluster*> SciFiClusterPArray;

} // ~namespace MAUS

#endif
