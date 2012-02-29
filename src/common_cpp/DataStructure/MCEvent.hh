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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_MCEVENT_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_MCEVENT_HH_

#include <vector>

#include "Rtypes.h" // ROOT

namespace MAUS {

class Track;
class VirtualHit;
class Hit;
class Primary;

typedef std::vector<Track> TrackArray;
typedef std::vector<VirtualHit> VirtualHitArray;
typedef std::vector<Hit> HitArray;

/** @class MCEvent describes data pertaining to a single Monte Carlo event
 *
 *  Each Monte Carlo event represents a single particle entering the detector
 *  system. Multiple tracks can be created by knock on electrons or particle
 *  decays.
 *
 *  MCEvent holds a list of tracks if the appropriate data card is set; a list
 *  of virtual hits (crossings of virtual detector planes); a list of sensitive
 *  detector hits (crossings of real detectors); and the initial primary for the
 *  event
 */

class MCEvent {
 public:
  /** Constructor initialises everything to NULL
   */
  MCEvent();

  /** Deep copy constructor - copies memory referenced by md
   */
  MCEvent(const MCEvent& md);
       
  /** Deep copy - copies memory referenced by md
   */
  MCEvent& operator=(const MCEvent& md);
  
  /** Delete all pointers
   */
  virtual ~MCEvent();

  /** Get the tracks pertaining to this event. MCEvent still owns TrackArray*
   */
  TrackArray* GetTracks() const;

  /** Set the tracks pertaining to this event. MCEvent takes ownership of memory
   *  pointed to by tracks.
   */
  void SetTracks(TrackArray* tracks);

  /** Get the virtual hits pertaining to this event. MCEvent still owns
   *  VirtualHitArray*.
   */
  VirtualHitArray* GetVirtualHits() const;

  /** Set the virtual hits pertaining to this event. MCEvent takes ownership of
   *  memory pointed to by hits.
   */
  void SetVirtualHits(VirtualHitArray* hits);

  /** Set the hits pertaining to this event MCEvent still owns HitArray*.
   */
  HitArray* GetHits() const;

  /** Set the hits pertaining to this event. MCEvent takes ownership of
   *  memory pointed to by hits.
   */
  void SetHits(HitArray* hits);

  /** Get the primary pertaining to this event. MCEvent still owns Primary*.
   */
  Primary* GetPrimary() const;

  /** Set the primary pertaining to this event. MCEvent takes ownership of
   *  memory pointed to by primary.
   */
  void SetPrimary(Primary* primary);

 private:

  Primary* _primary;
  VirtualHitArray* _virtuals;
  HitArray* _hits;
  TrackArray* _tracks;

  ClassDef(MCEvent, 1)
};

}

#endif

