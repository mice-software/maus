///////////////////////////////////////////////////////////////////////////////
/// KLFiber
///
/// \brief A piece of material used in the calorimeter, read out at two opposite ends. 
///
/// This class creates a rectangular cell which is read out at two ends 
/// using PMTs. For now, only a lead fiber glue spaghetti layer is handled
/// by this class.
///
/// \author 2006-03-04 Rikard Sandstrom
////////////////////////////////////////////////////////////////////////////////

#ifndef KLFiber_h
#define KLFiber_h 1

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

class KLFiber
{
public:

  /// The constructor
  KLFiber(MiceModule*, G4Material*, G4VPhysicalVolume*);

  /// The destructor, deletes volumes it has created
 ~KLFiber();

  G4LogicalVolume*   logicalFiber() const { return fLogicFiber; };
  G4PVPlacement*     placementFiber() const { return fPlaceFiber; };


private:

  G4Tubs*            fSolidFiber;
  G4LogicalVolume*   fLogicFiber;

  G4PVPlacement*     fPlaceFiber;
};

//
//
//////////////////////////////////////////////////////////////////////////////

#endif
