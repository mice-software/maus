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
#ifndef _CKOVSD_HH_
#define _CKOVSD_HH_ 1

#include <string>
#include <vector>
#include "Geant4/G4VSensitiveDetector.hh"
#include "DetModel/MAUSSD.hh"
#include "src/common_cpp/DataStructure/Hit.hh"

class G4Step;
class G4HCofThisEvent;
class MICEEvent;
class MiceModule;

/**
* CkovSD is the sensitive detector for GEANT4 that handles the CKOV detector
**/

class CkovSD : public MAUS::MAUSSD {

  public:

    // Constructor
    // I'm just kidding in the var. naming, cpplint was complaining so I was playing with it.
    // Whoever is reading this please don't report to anyone.
    explicit CkovSD(MiceModule* sorry_I_have_to_pass_the_cpplint_style_test_mod);

    // Initialise this this instance with the G4 event information
    void Initialize(G4HCofThisEvent* sorry_I_have_to_pass_the_cpplint_style_test_evt);

    // process all hits in this detector
    G4bool ProcessHits(G4Step*, G4TouchableHistory*);

    void EndOfEvent(G4HCofThisEvent* sorry_I_have_to_pass_the_cpplint_style_test_evt);
    bool isHit() {return _hits != NULL && _hits->size() > 0;}
    int GetNHits() {return _hits->size();}
    void ClearHits();
    void TakeHits(MAUS::MCEvent* event);
    void ProcessAngles(MAUS::CkovHit*, Hep3Vector, Hep3Vector, Hep3Vector);

  private:

    std::vector<MAUS::CkovHit>* _hits;
};
#endif
