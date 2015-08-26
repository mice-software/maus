// MAUS WARNING: THIS IS LEGACY CODE.
/* CKOVSD.hh
*/
#ifndef _CKOVSD_HH_
#define _CKOVSD_HH_ 1

#include <string>
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

    //! Constructor
    CkovSD(MiceModule*);

    //! Initialise this this instance with the G4 event information
    void Initialize(G4HCofThisEvent*);

    //! process all hits in this detector
    G4bool ProcessHits(G4Step*, G4TouchableHistory*);

    void EndOfEvent(G4HCofThisEvent*);
    bool isHit() {return _hits != NULL && _hits->size() > 0;}
    int GetNHits() {return _hits->size();}
    void ClearHits();
    void TakeHits(MAUS::MCEvent* event);
    void ProcessAngles(MAUS::CkovHit*, Hep3Vector, Hep3Vector, Hep3Vector);

  private:

    std::vector<MAUS::CkovHit>* _hits;

};

#endif
