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

#ifndef SciFiPlane_h
#define SciFiPlane_h 1

#include "Geant4/G4Tubs.hh"
#include "Geant4/G4ThreeVector.hh"
#include "Geant4/globals.hh"

#include "Config/MiceModule.hh"

class G4Tubs;
class G4LogicalVolume;
class G4VPhysicalVolume;

/** @class SciFiPlane
 * A SciFiPlane describes a plane (or projection, view, doublet layer) of scintillating fibre
 * detector. This class is used in the GEANT4 simulation of the Sci Fi tracker to describe
 * physical arrangement of all the fibres in a doublet layer, including the description of
 * the cladding and core of each scintillating fibre.
 *
 * Modified example 1 from the GEANT4 distribution to simulate the
 * MICE scintillating fiber tracker for the MICE proposal
 */

class SciFiPlane {
 public:
  /** @brief Constructor taking information from the SciFiTrackerGeom class
   */
  SciFiPlane(MiceModule* mod, G4Material* mater, G4VPhysicalVolume* mlv);

  /** @brief Default destructor
   */
  ~SciFiPlane();

  /** @brief The physical volume of the core
   */
  G4VPhysicalVolume* physicalCore() const { return physiCore; }

  /** @brief The logical volume of the core
   */
  G4LogicalVolume*   logicalCore()  const { return logicCore; }

  /** @brief The placement of the core
   */
  G4PVPlacement*    placementCore() const { return placeCore; }

  int get_numb_fibres() const { return _numFibres; }

  /** @brief Set the flag to choose to keep the solid doublet memory after destruction of the plane
   */
  void setKeepSolidDoublet(bool aKeep) { keepSolidDoublet = aKeep; }

  /** @brief Set the flag to choose to keep the logic doublet memory after destruction of the plane
   */
  void setKeepLogicDoublet(bool aKeep) { keepLogicDoublet = aKeep; }

  /** @brief Set the flag to choose to keep the physi doublet memory after destruction of the plane
   */
  void setKeepPhysiDoublet(bool aKeep) { keepPhysiDoublet = aKeep; }

    /** @brief Set the flag to choose to keep the solid core memory after destruction of the plane
   */
  void setKeepSolidCore(bool aKeep) { keepSolidCore = aKeep; }

  /** @brief Set the flag to choose to keep the logic core memory after destruction of the plane
   */
  void setKeepLogicCore(bool aKeep) { keepLogicCore = aKeep; }

  /** @brief Set the flag to choose to keep the physi core memory after destruction of the plane
   */
  void setKeepPhysiCore(bool aKeep) { keepPhysiCore = aKeep; }

 private:
  bool keepSolidDoublet;
  bool keepLogicDoublet;
  bool keepPhysiDoublet;
  bool keepSolidCore;
  bool keepLogicCore;
  bool keepPhysiCore;

  int _numFibres;

  G4Tubs*            solidDoublet;
  G4LogicalVolume*   logicDoublet;
  G4VPhysicalVolume* physiDoublet;

  G4Tubs*            solidCore;
  G4LogicalVolume*   logicCore;
  G4VPhysicalVolume* physiCore;

  G4PVPlacement*     placeCore;
};

#endif
