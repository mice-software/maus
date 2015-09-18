// MAUS WARNING: THIS IS LEGACY CODE.
//-----------------------------------------------------------------------------
/*
** MICE Special Virtual detector for the MICE proposal
** 12 Aug 2003
*/
//-----------------------------------------------------------------------------

#ifndef _SPECIALVIRTUALSD_HH_
#define _SPECIALVIRTUALSD_HH_  1

#include "Geant4/G4VSensitiveDetector.hh"
#include "Config/MiceModule.hh"
#include "Interface/MICEEvent.hh"
#include "src/common_cpp/DataStructure/Hit.hh"
#include "src/common_cpp/DetModel/MAUSSD.hh"

class G4Step;
class G4HCofThisEvent;

// TODO(rogers):
//         * remove "fake" geometry information
//         * add field information to output
//         * do we want to keep the local/global rotation stuff?

class SpecialVirtualSD : public MAUS::MAUSSD
{
  public:
      SpecialVirtualSD(MICEEvent* , MiceModule* );
      ~SpecialVirtualSD();
  void Initialize(G4HCofThisEvent*);
      G4bool ProcessHits(G4Step*, G4TouchableHistory*);
      void EndOfEvent(G4HCofThisEvent*);
      void clear();
      void DrawAll();
      void PrintAll();
      std::string getTypeName()  const;
      int  idFromName();
      void SetStepping(bool SteppingThrough, bool SteppingInto, bool SteppingOutOf, bool SteppingAcross);
      bool isHit() {return _hits != NULL && _hits->size() > 0;}
      int GetNHits() {return _hits->size();}
      void ClearHits();
      void TakeHits(MAUS::MCEvent* event);
  private:

      MICEEvent * simEvent;
      MiceModule * _module;
      std::string intToString(int number);
      bool isSpecial(G4String&) const;
      int _numberCellsInZ;
      int _numberCellsInR;
      int _numberCellsInPhi;
      int _iCellZ, _iCellR, _iCellPhi;
      int* _cellID;
      int  index(int, int, int);
      int  _stationNumber;
      std::string  _tname;
      void getCellIndeces(G4Step*);
      static int _uniqueID; //unique identifier for tname
      bool _steppingThrough, _steppingInto, _steppingOutOf, _steppingAcross;
      HepRotation _localRotation;
      HepRotation _globalRotation;
      Hep3Vector  _localPosition;
      Hep3Vector  _globalPosition;

      std::vector<MAUS::SpecialVirtualHit>* _hits;
};
#endif
