// MAUS WARNING: THIS IS LEGACY CODE.
/*
 * Conical Mirror for Upstream Cherenckov
 * 
 * (2007/01/11) Hideyuki Sakamoto
 *
 *  >> Constructing by Boolean operation using CSG Solid
 *  >> (2007/01/11) Simple Cone is used instead of Elliptical Cone due to Geant4 bug.
*/
#ifndef CkovMirror_h
#define CkovMirror_h 1

#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4EllipticalCone.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

#include "G4RotationMatrix.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpBoundaryProcess.hh"

#include "G4ThreeVector.hh"
#include "globals.hh"
#include "Config/MiceModule.hh"
#include <cmath> 

class CkovMirror
{
	public:
      CkovMirror( MiceModule*, G4Material*, G4VPhysicalVolume* );
		~CkovMirror();
		G4LogicalVolume*   logicalMirror()   const { return fLogic; }
		G4PVPlacement*     placementMirror() const { return fPlace; }

	private:
		G4VSolid*					fSolid;
		G4LogicalVolume*			fLogic;
		G4PVPlacement*				fPlace;
		G4LogicalSkinSurface*	MirrorSurface;
		G4OpticalSurface* 		fOpticalSurface;

/*
		// Ellipcital Cone
		G4VSolid* fEcon;
		G4VSolid* fConA;
		G4VSolid* fConB;
*/
		// Simple Cone
		G4VSolid* fCone;

		// Cutters
		G4VSolid* fPmts;
		G4VSolid* fSide;
		G4VSolid* fGate;

		// Solids used for boolean operation
      G4VSolid* fCon1;
      G4VSolid* fCon2;
      G4VSolid* fCon3;
      G4VSolid* fCon4;

		// Rotation Matrices
		G4RotationMatrix* fRot045;
		G4RotationMatrix* fRot090;
		G4RotationMatrix* fRot180;
		G4RotationMatrix* fRotcon;

		// Cutter for Gate
		G4double fDxOfGate;
		G4double fDyOfGate;
		G4double fDzOfGate;
		G4ThreeVector fPosOfGate;

		// Cutter for neighboring
		G4double fDxOfSide;
		G4double fDyOfSide;
		G4double fDzOfSide;
		G4ThreeVector fPosOfSide;

		// Cutter for PMTs
		G4double fDxOfPmts;
		G4double fDyOfPmts;
		G4double fDzOfPmts;
		G4ThreeVector fPosOfPmts;
/*
		// Elliptical Cone
		G4double fXSemiAxis;
		G4double fYSemiAxis;
		G4double fZMax;
		G4double fZTopCut;
		G4ThreeVector fPosOfEcon;
*/
		// Simple Cone
		G4double fRmin1;
		G4double fRmax1;
		G4double fRmin2;
		G4double fRmax2;
		G4double fDz;
		G4double fSPhi;
		G4double fDPhi;
		G4ThreeVector fPosOfCone;

		// Thickness of Cone
		G4double fThickness;

		G4ThreeVector fPosOfZero;
		G4ThreeVector fZero;
		// Calculate vertecies of Cone
		void calVertex( MiceModule* mod);

		// Methods used in calculating vertecies
		G4double getAngle(const G4ThreeVector& a, G4ThreeVector& b) const;
		G4double getLength(const G4ThreeVector& a, const G4ThreeVector& b) const;
		G4ThreeVector getCenter(const G4ThreeVector& a, const G4ThreeVector& b) const;
		G4ThreeVector getCross(const G4ThreeVector& a,const G4ThreeVector&b,
				                 const G4ThreeVector& u,const G4ThreeVector& v) const;
};
#endif

