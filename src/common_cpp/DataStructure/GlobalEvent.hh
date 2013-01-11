/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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
 */

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_GLOBALEVENT_
#define _SRC_COMMON_CPP_DATASTRUCTURE_GLOBALEVENT_

#include "Rtypes.h"  // ROOT

#include "src/common_cpp/DataStructure/GlobalPrimaryChain.hh"
#include "src/common_cpp/DataStructure/GlobalTrack.hh"
#include "src/common_cpp/DataStructure/GlobalTrackPoint.hh"
#include "src/common_cpp/DataStructure/GlobalSpacePoint.hh"

namespace MAUS {

/** @class GlobalEvent comment
 *
 */

class GlobalEvent {
 public:
  /** Default constructor - initialises to 0/NULL */
  GlobalEvent();
  
  /** Copy constructor - any pointers are deep copied */
  GlobalEvent(const GlobalEvent& globalevent);
  
  /** Equality operator - any pointers are deep copied */
  GlobalEvent& operator=(const GlobalEvent& globalevent);

  /** Destructor - any member pointers are deleted */
  virtual ~GlobalEvent();

  /** Add a MAUS::GlobalPrimaryChain object, daughter objects
   * are ignored. */
  void add_primarychain(MAUS::GlobalPrimaryChain* pchain) {
    _primarychains->push_back(pchain);
  };

  /** If a MAUS::GlobalPrimaryChain object is not already a
   *  member of the GlobalEvent, add it in.  Otherwise, return false,
   *  to signify already added.*/
  bool add_primarychain_check(MAUS::GlobalPrimaryChain* pchain);

  /** Recursively add a MAUS::GlobalPrimaryChain object and
   *  the associated daughter objects */
  void add_primarychain_recursive(MAUS::GlobalPrimaryChain* pchain);

  /** Return the std::vector of MAUS::GlobalPrimaryChain*,
   *  #_primarychains. */
  GlobalPrimaryChainPArray* get_primarychains() const {
    return _primarychains;
  };

  /** Set the std::vector<MAUS::recon::global::PrimaryChain*>. */
  void set_primarychains(GlobalPrimaryChainPArray *primarychains) {
    _primarychains = primarychains;
  };

  /** Add a MAUS::GlobalTrack object, daughter objects are ignored. */
  void add_track(MAUS::GlobalTrack* track) {
    _tracks->push_back(track);
  };

  /** If a MAUS::GlobalTrack object is not already a member of the
   *  GlobalEvent, add it in.  Otherwise, return false, to signify
   *  already added.*/
  bool add_track_check(MAUS::GlobalTrack* track);

  /** Recursively add a MAUS::GlobalTrack object and the associated
   *  daughter objects */
  void add_track_recursive(MAUS::GlobalTrack* track);

  /** Return the std::vector of MAUS::GlobalTrack*,
   *  #_tracks. */
  GlobalTrackPArray* get_tracks() const {
    return _tracks;
  };

  /** Set the std::vector<MAUS::recon::global::Track*>. */
  void set_tracks(GlobalTrackPArray *tracks) {
    _tracks = tracks;
  };

  /** Add a MAUS::GlobalTrackPoint object, daughter objects are ignored. */
  void add_trackpoint(MAUS::GlobalTrackPoint* trackpoint) {
    _trackpoints->push_back(trackpoint);
  };

  /** If a MAUS::GlobalTrackPoint object is not already a member of the
   *  GlobalEvent, add it in.  Otherwise, return false, to signify
   *  already added.*/
  bool add_trackpoint_check(MAUS::GlobalTrackPoint* trackpoint);

  /** Recursively add a MAUS::GlobalTrackPoint object and the associated
   *  daughter objects */
  void add_trackpoint_recursive(MAUS::GlobalTrackPoint* trackpoint);

  /** Return the std::vector of MAUS::GlobalTrackPoint*,
   *  #_trackpoints. */
  GlobalTrackPointPArray* get_trackpoints() const {
    return _trackpoints;
  };

  /** Set the std::vector<MAUS::recon::global::TrackPoint*>. */
  void set_trackpoints(GlobalTrackPointPArray *trackpoints) {
    _trackpoints = trackpoints;
  };

  /** Add a MAUS::GlobalSpacePoint object. */
  void add_spacepoint(MAUS::GlobalSpacePoint* spacepoint) {
    _spacepoints->push_back(spacepoint);
  };

  /** If a MAUS::GlobalSpacePoint object is not already a
   *  member of the GlobalEvent, add it in.  Otherwise, return false,
   *  to signify already added.*/
  bool add_spacepoint_check(MAUS::GlobalSpacePoint* spacepoint);

  /** Return the std::vector of MAUS::GlobalSpacePoint*,
   *  #_spacepoints. */
  GlobalSpacePointPArray* get_spacepoints() const {
    return _spacepoints;
  };

  /** Set the std::vector<MAUS::recon::global::SpacePoint*>, #_spacepoints. */
  void set_spacepoints(GlobalSpacePointPArray *spacepoints) {
    _spacepoints = spacepoints;
  };

 private:

  /** A vector of MAUS::GlobalPrimaryChain pointers
   *  associated with this event. Different reconstruction steps can
   *  be distinguished by mapper_name. */
  GlobalPrimaryChainPArray *_primarychains;
  
  /** A vector of MAUS::GlobalTrack pointers associated with
   *  this event. These should be accessed through the
   *  MAUS::GlobalPrimaryChain pointers stored in
   *  #_primarychains. */
  GlobalTrackPArray *_tracks;
  
  /** A vector of MAUS::GlobalTrackPoint pointers associated with
   *  this event. These should be accessed through the
   *  MAUS::GlobalPrimaryChain pointers stored in
   *  #_primarychains. */
  GlobalTrackPointPArray *_trackpoints;
  
  /** A vector of MAUS::GlobalSpacePoint pointers associated with
   *  this event. These should be accessed through the
   *  MAUS::GlobalPrimaryChain pointers stored in
   *  #_primarychains. */
  GlobalSpacePointPArray *_spacepoints;

  ClassDef(GlobalEvent, 2)
}; // ~GlobalEvent

} // ~MAUS

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_GLOBALEVENT_

