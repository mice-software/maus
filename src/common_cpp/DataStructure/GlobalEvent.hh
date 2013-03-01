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

#include "src/common_cpp/DataStructure/Global/PrimaryChain.hh"
#include "src/common_cpp/DataStructure/Global/Track.hh"
#include "src/common_cpp/DataStructure/Global/TrackPoint.hh"
#include "src/common_cpp/DataStructure/Global/SpacePoint.hh"

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

  /** Add a MAUS::DataStructure::Global::PrimaryChain object, daughter objects
   * are ignored. */
  void add_primarychain(MAUS::DataStructure::Global::PrimaryChain* pchain);

  /** If a MAUS::DataStructure::Global::PrimaryChain object is not already a
   *  member of the GlobalEvent, add it in.  Otherwise, return false,
   *  to signify already added.*/
  bool add_primarychain_check(
      MAUS::DataStructure::Global::PrimaryChain* pchain);

  /** Recursively add a MAUS::DataStructure::Global::PrimaryChain object and
   *  the associated daughter objects */
  void add_primarychain_recursive(
      MAUS::DataStructure::Global::PrimaryChain* pchain);

  /** Return the std::vector of MAUS::DataStructure::Global::PrimaryChain*,
   *  #_primarychains. */
  std::vector<MAUS::DataStructure::Global::PrimaryChain*>*
  get_primarychains() const;

  /** Set the std::vector<MAUS::DataStructure::Global::PrimaryChain*>. */
  void set_primarychains(
      std::vector<MAUS::DataStructure::Global::PrimaryChain*> *primarychains);

  /** Add a MAUS::DataStructure::Global::Track object, daughter
   *  objects are ignored. */
  void add_track(MAUS::DataStructure::Global::Track* track);

  /** If a MAUS::DataStructure::Global::Track object is not already a
   *  member of the GlobalEvent, add it in.  Otherwise, return false,
   *  to signify already added.*/
  bool add_track_check(MAUS::DataStructure::Global::Track* track);

  /** Recursively add a MAUS::DataStructure::Global::Track object and
   *  the associated daughter objects */
  void add_track_recursive(MAUS::DataStructure::Global::Track* track);

  /** Return the std::vector of MAUS::DataStructure::Global::Track*,
   *  #_tracks. */
  std::vector<MAUS::DataStructure::Global::Track*>* get_tracks() const;

  /** Set the std::vector<MAUS::DataStructure::Global::Track*>. */
  void set_tracks(std::vector<MAUS::DataStructure::Global::Track*> *tracks);

  /** Add a MAUS::DataStructure::Global::TrackPoint object, daughter
   *  objects are ignored. */
  void add_trackpoint(MAUS::DataStructure::Global::TrackPoint* trackpoint);

  /** If a MAUS::DataStructure::Global::TrackPoint object is not
   *  already a member of the GlobalEvent, add it in.  Otherwise,
   *  return false, to signify already added.*/
  bool add_trackpoint_check(
      MAUS::DataStructure::Global::TrackPoint* trackpoint);

  /** Recursively add a MAUS::DataStructure::Global::TrackPoint object
   *  and the associated daughter objects */
  void add_trackpoint_recursive(
      MAUS::DataStructure::Global::TrackPoint* trackpoint);

  /** Return the std::vector of MAUS::DataStructure::Global::TrackPoint*,
   *  #_trackpoints. */
  std::vector<MAUS::DataStructure::Global::TrackPoint*>*
  get_trackpoints() const;

  /** Set the std::vector<MAUS::DataStructure::Global::TrackPoint*>. */
  void set_trackpoints(
      std::vector<MAUS::DataStructure::Global::TrackPoint*> *trackpoints);

  /** Add a MAUS::DataStructure::Global::SpacePoint object. */
  void add_spacepoint(MAUS::DataStructure::Global::SpacePoint* spacepoint);

  /** If a MAUS::DataStructure::Global::SpacePoint object is not already a
   *  member of the GlobalEvent, add it in.  Otherwise, return false,
   *  to signify already added.*/
  bool add_spacepoint_check(
      MAUS::DataStructure::Global::SpacePoint* spacepoint);

  /** Return the std::vector of MAUS::DataStructure::Global::SpacePoint*,
   *  #_spacepoints. */
  std::vector<MAUS::DataStructure::Global::SpacePoint*>*
  get_spacepoints() const;

  /** Set the std::vector<MAUS::DataStructure::Global::SpacePoint*>,
   *  #_spacepoints. */
  void set_spacepoints(
      std::vector<MAUS::DataStructure::Global::SpacePoint*> *spacepoints);

 private:

  /** A vector of MAUS::DataStructure::Global::PrimaryChain pointers
   *  associated with this event. Different reconstruction steps can
   *  be distinguished by mapper_name. */
  std::vector<MAUS::DataStructure::Global::PrimaryChain*> *_primarychains;
  
  /** A vector of MAUS::DataStructure::Global::Track pointers associated with
   *  this event. These should be accessed through the
   *  MAUS::DataStructure::Global::PrimaryChain pointers stored in
   *  #_primarychains. */
  std::vector<MAUS::DataStructure::Global::Track*> *_tracks;
  
  /** A vector of MAUS::DataStructure::Global::TrackPoint pointers
   *  associated with this event. These should be accessed through the
   *  MAUS::DataStructure::Global::PrimaryChain pointers stored in
   *  #_primarychains. */
  std::vector<MAUS::DataStructure::Global::TrackPoint*> *_trackpoints;
  
  /** A vector of MAUS::DataStructure::Global::SpacePoint pointers
   *  associated with this event. These should be accessed through the
   *  MAUS::DataStructure::Global::PrimaryChain pointers stored in
   *  #_primarychains. */
  std::vector<MAUS::DataStructure::Global::SpacePoint*> *_spacepoints;

  ClassDef(GlobalEvent, 2)
}; // ~GlobalEvent

} // ~MAUS

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_GLOBALEVENT_

