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
#include <G4VPhysicalVolume.hh>
#include <G4ThreeVector.hh>
#include <G4Tubs.hh>

#include "src/common_cpp/DetModel/SciFi/DoubletFiberParam.hh"

// taking the parameters that define a fibre
DoubletFiberParam::DoubletFiberParam(G4double pSensitiveRadius,
                                     G4double pActiveRadius,
                                     G4double pOuterDiameter,
                                     G4double pInnerDiameter,
                                     G4double pFiberDiameter,
                                     G4double pFiberPitch ) {
  sensitiveRadius = pSensitiveRadius;
  activeRadius = pActiveRadius;
  outerDiameter = pOuterDiameter;
  innerDiameter = pInnerDiameter;
  fiberDiameter = pFiberDiameter;
  fiberPitch = pFiberPitch;

  coreRotation = new G4RotationMatrix(CLHEP::HepRotationX(90.0*deg));
}

void DoubletFiberParam::ComputeTransformation(const G4int copyNo,
                                              G4VPhysicalVolume* physVol) const {
  // this is the spacing between fibres
  G4double spacing = sqrt(fiberDiameter*fiberDiameter*
                         (1-fiberPitch*fiberPitch/4));

  // these are the coordinates of the fiber placement
  G4double xPos = - copyNo*fiberDiameter*fiberPitch/2
                  + (activeRadius-fiberDiameter/2);
  G4double yPos = 0.0;
  G4double zPos = (copyNo%2) ? -0.5*spacing : 0.5*spacing;

  physVol->SetRotation(coreRotation);
  physVol->SetTranslation(G4ThreeVector(xPos, yPos, zPos));
}

void DoubletFiberParam::ComputeDimensions
(G4Tubs& fiberElement, const G4int copyNo,
  const G4VPhysicalVolume* physVol) const {
  // this is the number of fibers
  G4double nFiber = 2*floor(activeRadius/(fiberDiameter*fiberPitch/2));

  G4double xPos = 2*activeRadius*(copyNo/nFiber)
                  -(activeRadius-fiberDiameter/2);

  G4double fiberHalfLen = 0.;

  if ( sensitiveRadius > fabs( xPos ) )
    fiberHalfLen = sqrt(sensitiveRadius * sensitiveRadius - xPos * xPos);

  fiberElement.SetInnerRadius(innerDiameter/2);
  fiberElement.SetOuterRadius(outerDiameter/2);
  fiberElement.SetZHalfLength(fiberHalfLen);
  fiberElement.SetStartPhiAngle(0.0*deg);
  fiberElement.SetDeltaPhiAngle(360.0*deg);
}
