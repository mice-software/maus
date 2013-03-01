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
 *  @author Ian Taylor, University of Warwick
 *  @date 2013/03/01 - Expanded from original 'blank slate'
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
  void add_primary_chain(MAUS::DataStructure::Global::PrimaryChain* pchain);

  /** If a MAUS::DataStructure::Global::PrimaryChain object is not already a
   *  member of the GlobalEvent, add it in.  Otherwise, return false,
   *  to signify already added.*/
  bool add_primary_chain_check(
      MAUS::DataStructure::Global::PrimaryChain* pchain);

  /** Recursively add a MAUS::DataStructure::Global::PrimaryChain object and
   *  the associated daughter objects */
  void add_primary_chain_recursive(
      MAUS::DataStructure::Global::PrimaryChain* pchain);

  /** Return the std::vector of MAUS::DataStructure::Global::PrimaryChain*,
   *  #_primary_chains. */
  std::vector<MAUS::DataStructure::Global::PrimaryChain*>*
  get_primary_chains() const;

  /** Set the std::vector<MAUS::DataStructure::Global::PrimaryChain*>. */
  void set_primary_chains(
      std::vector<MAUS::DataStructure::Global::PrimaryChain*> *primary_chains);

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
  void add_track_point(MAUS::DataStructure::Global::TrackPoint* track_point);

  /** If a MAUS::DataStructure::Global::TrackPoint object is not
   *  already a member of the GlobalEvent, add it in.  Otherwise,
   *  return false, to signify already added.*/
  bool add_track_point_check(
      MAUS::DataStructure::Global::TrackPoint* track_point);

  /** Recursively add a MAUS::DataStructure::Global::TrackPoint object
   *  and the associated daughter objects */
  void add_track_point_recursive(
      MAUS::DataStructure::Global::TrackPoint* track_point);

  /** Return the std::vector of MAUS::DataStructure::Global::TrackPoint*,
   *  #_track_points. */
  std::vector<MAUS::DataStructure::Global::TrackPoint*>*
  get_track_points() const;

  /** Set the std::vector<MAUS::DataStructure::Global::TrackPoint*>. */
  void set_track_points(
      std::vector<MAUS::DataStructure::Global::TrackPoint*> *track_points);

  /** Add a MAUS::DataStructure::Global::SpacePoint object. */
  void add_space_point(MAUS::DataStructure::Global::SpacePoint* space_point);

  /** If a MAUS::DataStructure::Global::SpacePoint object is not already a
   *  member of the GlobalEvent, add it in.  Otherwise, return false,
   *  to signify already added.*/
  bool add_space_point_check(
      MAUS::DataStructure::Global::SpacePoint* space_point);

  /** Return the std::vector of MAUS::DataStructure::Global::SpacePoint*,
   *  #_space_points. */
  std::vector<MAUS::DataStructure::Global::SpacePoint*>*
  get_space_points() const;

  /** Set the std::vector<MAUS::DataStructure::Global::SpacePoint*>,
   *  #_space_points. */
  void set_space_points(
      std::vector<MAUS::DataStructure::Global::SpacePoint*> *space_points);

 private:

  /** A vector of MAUS::DataStructure::Global::PrimaryChain pointers
   *  associated with this event. Different reconstruction steps can
   *  be distinguished by mapper_name. */
  std::vector<MAUS::DataStructure::Global::PrimaryChain*> *_primary_chains;
  
  /** A vector of MAUS::DataStructure::Global::Track pointers associated with
   *  this event. These should be accessed through the
   *  MAUS::DataStructure::Global::PrimaryChain pointers stored in
   *  #_primary_chains. */
  std::vector<MAUS::DataStructure::Global::Track*> *_tracks;
  
  /** A vector of MAUS::DataStructure::Global::TrackPoint pointers
   *  associated with this event. These should be accessed through the
   *  MAUS::DataStructure::Global::PrimaryChain pointers stored in
   *  #_primary_chains. */
  std::vector<MAUS::DataStructure::Global::TrackPoint*> *_track_points;
  
  /** A vector of MAUS::DataStructure::Global::SpacePoint pointers
   *  associated with this event. These should be accessed through the
   *  MAUS::DataStructure::Global::PrimaryChain pointers stored in
   *  #_primary_chains. */
  std::vector<MAUS::DataStructure::Global::SpacePoint*> *_space_points;

  ClassDef(GlobalEvent, 2)
}; // ~GlobalEvent

} // ~MAUS

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_GLOBALEVENT_

