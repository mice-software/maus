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

#ifndef EMRBar_h
#define EMRBar_h 1

#include <G4Tubs.hh>
#include <G4Trap.hh>
#include <G4ThreeVector.hh>
#include <globals.hh>

#include "Config/MiceModule.hh"

class G4Tubs;
class G4Box;
class G4Trap;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4VPVParameterisation;
class G4Material;

/** @class EMRBar
 * This class is used in the GEANT4 simulation of the EMR detector to describe
 * the geometry of a triangular scintillator bar and the wavelength shifting fiber.
 */

class EMRBar {
  public:
  /** @brief Constructor
   */
  EMRBar(MiceModule* mod, G4Material* mater, G4VPhysicalVolume* mlv);

  /** @brief Default destructor
   */
  ~EMRBar();

  /** @brief The physical volume of the
   */
  G4VPhysicalVolume* physicalBar() const { return physiBar; }

  /** @brief The logical volume of the core
   */
  G4LogicalVolume*   logicalBar()  const { return logicBar; }

  /** @brief The placement of the core
   */
  G4PVPlacement* placementBar() const { return physiCalorimeter; }

 private:
  G4Box*             solidCalorimeter;  // pointer to the solid Calorimeter
  G4LogicalVolume*   logicCalorimeter;  // pointer to the logical Calorimeter
  G4PVPlacement* physiCalorimeter;  // pointer to the physical Calorimeter

  G4Trap*            solidBar;  // pointer to the solid Bar
  G4LogicalVolume*   logicBar;  // pointer to the logical Bar
  G4VPhysicalVolume* physiBar;  // pointer to the physical Bar

  G4Tubs*            solidHole;
  G4LogicalVolume*   logicHole;
  G4VPhysicalVolume* physiHole;

  G4Tubs*            solidFiberCladding;
  G4LogicalVolume*   logicFiberCladding;
  G4VPhysicalVolume* physiFiberCladding;

  G4Tubs*            solidFiberCore;
  G4LogicalVolume*   logicFiberCore;
  G4VPhysicalVolume* physiFiberCore;

  G4VPVParameterisation* barParam; // pointer to bar parameterisation

  G4bool fAddWLSFiber;
};

#endif
