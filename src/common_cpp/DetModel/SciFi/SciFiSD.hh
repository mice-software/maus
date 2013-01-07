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

#include "json/json.h"
#include "DetModel/MAUSSD.hh"

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

    /** @brief Code that is called at the end of processing of an event
     *   Does nothing.
     */
    void EndOfEvent(G4HCofThisEvent* HCE);
};

#endif
