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

#ifndef SciFiSD_h
#define SciFiSD_h 1

// C headers
#include <cmath>

// other headers
#include "Geant4/G4TransportationManager.hh"
#include "Geant4/G4FieldManager.hh"
#include "Geant4/G4Field.hh"
#include "Geant4/G4HCofThisEvent.hh"
#include "Geant4/G4Step.hh"
#include "Geant4/G4ThreeVector.hh"
#include "Geant4/G4SDManager.hh"
#include "Geant4/G4ios.hh"

#include "json/json.h"
#include "Interface/MICEEvent.hh"
#include "src/legacy/Config/MiceModule.hh"
#include "DetModel/MAUSSD.hh"
#include "src/common_cpp/DataStructure/Hit.hh"

/** @brief The Scintilating Fibre Sensitive Detector
 *   SciFiSD is the sensitive detector for GEANT4 that handles the Scintillating
 *   Fibre tracker simulation.
 */

class SciFiSD : public MAUS::MAUSSD {
  public:
    /** @brief Constructor
     */
    explicit SciFiSD(MiceModule* mod);

    /** @brief Processes all hits in this detector
     */
    G4bool ProcessHits(G4Step*, G4TouchableHistory*);

    /** True if the SD has at least one hit in its _hits array */
    bool isHit() {return _hits == NULL || _hits->size() == 0;}

    /** Returns the number of hits in the Sensitive Detector */
    int GetNHits();

    /** Clears all hits in the sensitive detector */
    void ClearHits();

    /** Hand ownership of the detector hits to MCEvent */
    void TakeHits(MAUS::MCEvent* event);

    /** @brief Code that is called at the end of processing of an event
     *   Does nothing.
     */
    void EndOfEvent(G4HCofThisEvent* HCE);
  private:
    std::vector<MAUS::SciFiHit>* _hits;
};

#endif
