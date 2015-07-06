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

#ifndef EMRSD_h
#define EMRSD_h 1

// C++
#include <json/json.h>
#include <string>

// MAUS
#include "DetModel/MAUSSD.hh"
#include "DataStructure/Hit.hh"
#include "DataStructure/EMRChannelId.hh"
#include "DataStructure/MCEvent.hh"
#include "JsonCppProcessors/HitProcessor.hh"

// // Geant 4
#include "Geant4/G4VSensitiveDetector.hh"

class G4Step;
class G4HCofThisEvent;
class MiceModule;

/**
* EMRSD is the sensitive detector for GEANT4 that handles the
* EMR calorimeter simulation.
**/

// class EMRSD : public MAUS::MAUSSD {
//   public:
//     /** @brief Constructor
//      */
//      EMRSD(MiceModule*);
//
//    void Initialize( G4HCofThisEvent* );
//
//     /** @brief Processes all hits in this detector
//      */
//     G4bool ProcessHits(G4Step*, G4TouchableHistory*);
//
//     /** @brief Code that is called at the end of processing of an event
//      *   Does nothing.
//      */
//     void EndOfEvent(G4HCofThisEvent* );
//
// };
//
// #endif

/**
* SAVING DATA IN CPP FILES (TO BE CONVERTED LATER TO JSON OR ROOT)
**/

class EMRSD : public MAUS::MAUSSD {
 public:
  /// Constructor
  explicit EMRSD(MiceModule* mod);

  /// Default destructor
  virtual ~EMRSD() {}

  /// Initialise this this instance with the G4 event information
  void Initialize(G4HCofThisEvent* HCE);

  /// process all hits in this detector
  G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist);

  /// code that is called at the end of processing of an event
  void EndOfEvent(G4HCofThisEvent* HCE);

  /// True if the SD has at least one hit in its _hits array
  virtual bool isHit() {return _hits_cppdata.size() == 0;}

  /// Returns the number of hits in the Sensitive Detector
  int GetNHits() {return _hits_cppdata.size();}

  /// Clears all hits in the sensitive detector
  void ClearHits() {_hits_cppdata = MAUS::EMRHitArray();}

  /// Hand ownership of the detector hits to MCEvent
  void TakeHits(MAUS::MCEvent* event);

 private:
  int findBarHit(int copyNumber);
  int AddBarHit(G4Step* aStep, int barNumber);

//   MAUS::EMRHit          _hit_cppdata;
//   MAUS::EMRChannelId    _ch_id;
  MAUS::EMRHitArray     _hits_cppdata;
  MAUS::EMRHitProcessor _hit_proc;
};

#endif
