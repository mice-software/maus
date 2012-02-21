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

class MCEvent {
 public:
  MCEvent();

  MCEvent(const MCEvent& md);
       
  MCEvent& operator=(const MCEvent& md);
  
  virtual ~MCEvent();

  TrackArray* GetTracks() const;
  void SetTracks(TrackArray* tracks);

  VirtualHitArray* GetVirtualHits() const;
  void SetVirtualHits(VirtualHitArray* hits);

  HitArray* GetHits() const;
  void SetHits(HitArray* hits);

  Primary* GetPrimary() const;
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

