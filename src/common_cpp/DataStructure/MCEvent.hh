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

class MCTrack;
class VirtualHit;
class SDHit;
class Primary;

typedef std::vector<MCTrack> TrackArray;
typedef std::vector<VirtualHit> VirtualHitArray;
typedef std::vector<SDHit> SDHitArray;

class MCEvent {
 public:
  MCEvent();

  MCEvent(const MCEvent& md);
       
  MCEvent& operator=(const MCEvent& md);
  
  virtual ~MCEvent();

  TrackArray* GetTracks();
  void SetTracks(TrackArray* tracks);

  VirtualHitArray* GetVirtualHits();
  void SetVirtualHits(VirtualHitArray* hits);

  SDHitArray* GetSDHits();
  void SetSDHits(SDHitArray* hits);

  Primary* GetPrimary();
  void SetPrimary(Primary* primary);

 private:

  Primary* _primary;
  VirtualHitArray* _virtuals;
  SDHitArray* _sdhits;
  TrackArray* _tracks;

  ClassDef(MCEvent, 1)
};

}

#endif

