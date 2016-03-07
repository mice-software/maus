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

#ifndef DoubletFiberParam_H
#define DoubletFiberParam_H 1

#include "Geant4/G4RotationMatrix.hh"
#include "Geant4/globals.hh"
#include "Geant4/G4VPVParameterisation.hh"

class G4VPhysicalVolume;
class G4Tubs;

/** @class DoubletFiberParam
 *  stores physical parameters of the Scintillating Fibre doublets (or planes)
 *  which are used to describe the Scintillating Fibre tracker in GEANT4
 *
 */

class DoubletFiberParam : public G4VPVParameterisation {
 public:
  /** @brief Constructor taking the parameters that define the doublet planes
   *   the parameters are stored at src/Models/Modules/Tracker/
   */
  DoubletFiberParam(G4double pCentralFibre,
                    G4double pActiveRadius,
                    G4double pOuterDiameter,
                    G4double pInnerDiameter,
                    G4double pFiberDiameter,
                    G4double pFiberPitch);

  /** @brief Calculates the transformations needed to instantiate an instance
      of the SciFi doublet in a certain G4 physical volume
  */
  void ComputeTransformation(const G4int copyNo,
                             G4VPhysicalVolume* physVol) const;

  /** @brief Calculate the dimensions of the fibres that make up a doublet layer
   */
  void ComputeDimensions(G4Tubs& fiberElement,
                         const G4int copyNo,
                         const G4VPhysicalVolume* physVol) const;

  G4double   getCentralFiber()    const { return centralFiber;  }
  G4double   getActiveRadius()    const { return activeRadius;  }
//  G4double   getSensitiveRadius() const { return sensitiveRadius; }
  G4double   getInnerDiameter()   const { return innerDiameter; }
  G4double   getOuterDiameter()   const { return outerDiameter; }
  G4double   getFiberDiameter()   const { return fiberDiameter; }
  G4double   getFiberPitch()      const { return fiberPitch;    }

 private:
  G4double centralFiber;
  G4double activeRadius;
//  G4double sensitiveRadius;
  G4double innerDiameter;
  G4double outerDiameter;
  G4double fiberDiameter;
  G4double fiberPitch;
  G4double zSpacing;
  G4RotationMatrix* coreRotation;
};

#endif
