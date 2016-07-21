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

#include "src/common_cpp/DataStructure/SciFiNoiseHit.hh"
#include "src/common_cpp/DataStructure/Hit.hh" // ROOT was tripping up on
                                               // forward declaration of this
#include "src/common_cpp/Utils/VersionNumber.hh"

namespace MAUS {

class Track;
class VirtualHit;
class Primary;

class SciFiChannelId;
class TOFChannelId;
class CkovChannelId;
class KLChannelId;
class SpecialVirtualChannelId;
class SciFiNoiseHit;
class SciFiMCLookup;

typedef std::vector<Track> TrackArray;
typedef std::vector<VirtualHit> VirtualHitArray;
typedef std::vector<SciFiHit> SciFiHitArray;
typedef std::vector<SciFiNoiseHit> SciFiNoiseHitArray;
typedef std::vector<TOFHit> TOFHitArray;
typedef std::vector<CkovHit> CkovHitArray;
typedef std::vector<KLHit> KLHitArray;
typedef std::vector<EMRHit> EMRHitArray;
typedef std::vector<SpecialVirtualHit> SpecialVirtualHitArray;

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

  /** Get a single MC event (needed for PyROOT) */
  VirtualHit& GetAVirtualHit(size_t i) const;

  /** Get the MC event size (needed for PyROOT)*/
  size_t GetVirtualHitsSize() const;

  /** Get the hits pertaining to this event MCEvent still owns HitArray*.
   */
  SciFiHitArray* GetSciFiHits() const;

  /** Set the hits pertaining to this event. MCEvent takes ownership of
   *  memory pointed to by hits.
   */
  void SetSciFiHits(SciFiHitArray* hits);

  /** Add a single scifi hit (needed for PyROOT access) */
  void AddSciFiHit(SciFiHit& hit);

  /** Get the hits pertaining to this event MCEvent still owns NoiseHitArray */
  SciFiNoiseHitArray* GetSciFiNoiseHits() const;

  /** Set the hits pertaining to this event. MCEvent takes ownership of
   *  memory pointed to by noise_hits.
   */
  void SetSciFiNoiseHits(SciFiNoiseHitArray* noise_hits);

  /** Add a single scifi noise hit (needed for PyROOT access) */
  void AddSciFiNoiseHit(SciFiNoiseHit& hit);

  /** Get the TOF hits pertaining to this event MCEvent still owns HitArray*.
   */
  TOFHitArray* GetTOFHits() const;

  /** Set the TOF hits pertaining to this event. MCEvent takes ownership of
   *  memory pointed to by hits.
   */
  void SetTOFHits(TOFHitArray* hits);

  /** Get the Ckov hits pertaining to this event MCEvent still owns HitArray*.
   */
  CkovHitArray* GetCkovHits() const;

  /** Set the Ckov hits pertaining to this event. MCEvent takes ownership of
   *  memory pointed to by hits.
   */
  void SetCkovHits(CkovHitArray* hits);

  /** Get the KL hits pertaining to this event MCEvent still owns HitArray*.
   */
  KLHitArray* GetKLHits() const;

  /** Set the KL hits pertaining to this event. MCEvent takes ownership of
   *  memory pointed to by hits.
   */
  void SetKLHits(KLHitArray* hits);

  /** Get the EMR hits pertaining to this event MCEvent still owns HitArray*.
   */
  EMRHitArray* GetEMRHits() const;

  /** Set the EMR hits pertaining to this event. MCEvent takes ownership of
   *  memory pointed to by hits.
   */
  void SetEMRHits(EMRHitArray* hits);

  /** Get the Special Virtual hits pertaining to this event. MCEvent still owns
   *  SpecialVirtualHitArray*.
   */
  SpecialVirtualHitArray* GetSpecialVirtualHits() const;

  /** Set the Special Virtual hits pertaining to this event. MCEvent takes
   * ownership of memory pointed to by hits.
   */
  void SetSpecialVirtualHits(SpecialVirtualHitArray* hits);

  /** Get the primary pertaining to this event. MCEvent still owns Primary*.
   */
  Primary* GetPrimary() const;

  /** Set the primary pertaining to this event. MCEvent takes ownership of
   *  memory pointed to by primary.
   */
  void SetPrimary(Primary* primary);

 private:

  Primary*                _primary;
  VirtualHitArray*        _virtuals;
  SciFiHitArray*          _sci_fi_hits;
  SciFiNoiseHitArray*     _sci_fi_noise_hits;
  TOFHitArray*            _tof_hits;
  CkovHitArray*           _ckov_hits;
  KLHitArray*             _kl_hits;
  EMRHitArray*            _emr_hits;
  SpecialVirtualHitArray* _special_virtual_hits;
  TrackArray*             _tracks;

  MAUS_VERSIONED_CLASS_DEF(MCEvent)
};

typedef std::vector<MCEvent*> MCEventPArray;
}

#endif

