//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// Dummy sensitive used only to flag sensitivity
// in cells of RO geometry.
//

#ifndef SpecialDummySD_h
#define SpecialDummySD_h 1

#include "G4VSensitiveDetector.hh"

#include "Interface/Memory.hh" 

class G4Step;

class SpecialDummySD : public G4VSensitiveDetector
{
public:
  SpecialDummySD()		{ miceMemory.addNew( Memory::SpecialdummySD ); };
  ~SpecialDummySD() { miceMemory.addDelete( Memory::SpecialDummySD); };
  
  void Initialize(G4HCofThisEvent*) {}
  G4bool ProcessHits(G4Step*,G4TouchableHistory*) {return false;}
  void EndOfEvent(G4HCofThisEvent*) {}
  void clear() {}
  void DrawAll() {}
  void PrintAll() {}
};
SpecialDummySD::SpecialDummySD()
  : G4VSensitiveDetector("dummySD")
{}
#endif
