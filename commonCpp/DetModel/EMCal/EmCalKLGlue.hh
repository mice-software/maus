///////////////////////////////////////////////////////////////////////////////
/// EmCalKLGlue
///
/// \brief A piece of material used in the calorimeter, read out at two opposite ends. 
///
/// This class creates a rectangular cell which is read out at two ends 
/// using PMTs. For now, only a lead fiber glue spaghetti layer is handled
/// by this class.
///
/// \author 2006-03-04 Rikard Sandstrom
////////////////////////////////////////////////////////////////////////////////

#ifndef EmCalKLGlue_h
#define EmCalKLGlue_h 1

#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "Config/MiceModule.hh"

#include "Interface/Memory.hh" 

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4PVPlacement;
class G4Material;

class EmCalKLGlue
{
public:

  /// The constructor
  EmCalKLGlue(MiceModule*, G4Material*, G4VPhysicalVolume*);

  /// The destructor, deletes volumes it has created
 ~EmCalKLGlue();

  G4LogicalVolume*   logicalStrip() const { return fLogicGlueStrip; };
  G4PVPlacement*     placementStrip() const { return fPlaceStrip; };


private:
  G4Box*             fSolidGlueStrip;
  G4LogicalVolume*   fLogicGlueStrip;

  G4Tubs*            fSolidHole;
  G4LogicalVolume*   fLogicHole;


  G4PVPlacement*     fPlaceStrip;
};

//
//
//////////////////////////////////////////////////////////////////////////////

#endif
