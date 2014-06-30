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

#include "DetModel/EMR/BarParameterisation.hh"

// Geant4
#include "Geant4/G4VPhysicalVolume.hh"
#include "Geant4/G4ThreeVector.hh"
#include "Geant4/G4Box.hh"

BarParameterisation::BarParameterisation(G4double BarWidth,
                                         G4double BarHeight,
                                         G4int NbOfBars,
                                         G4double Gap) {
  fBarWidth = BarWidth;
  fBarHeight = BarHeight;
  fNbOfBars = NbOfBars;
  fGap = Gap;
  rotation = new G4RotationMatrix();
}

BarParameterisation::~BarParameterisation() {
}

void BarParameterisation::ComputeTransformation(const G4int copyNo,
                                                G4VPhysicalVolume* physVol) const {
  G4ThreeVector placement(0.0, 0.0, 0.0);

  G4ThreeVector rowX(1.0, 0.0, 0.0);
  G4ThreeVector rowY(0.0, 1.0, 0.0);
  G4ThreeVector rowZ(0.0, 0.0, 1.0);

  rotation->setRows(rowX, rowY, rowZ);

  G4int plainID = copyNo/59;
  G4int barIDinPlain = copyNo - 59*plainID;
  G4int Nplains = fNbOfBars/59 + 1;

  G4int NbarsInPlain;
  if (fNbOfBars < 59)
    NbarsInPlain = fNbOfBars;
  else if (plainID < (fNbOfBars/59))
    NbarsInPlain = 59;
  else
    NbarsInPlain = 59-fNbOfBars%59;


  G4double StartX = -(NbarsInPlain/2)*(0.5*fBarWidth+fGap);
  G4double StartY = StartX;
  G4double StartZ = - (Nplains/2)*(fBarHeight + fGap);

  placement.setZ(StartZ + plainID*(fBarHeight + fGap));
  rotation->rotateY(0.0*deg);

  if (plainID%2 == 0) {
    if (copyNo%2 == 0)
      rotation->rotateX(0.0*deg);
    else
      rotation->rotateX(180.0*deg);

    rotation->rotateZ(0.0*deg);
    placement.setX(0.0*cm);
    placement.setY(StartY+barIDinPlain*(0.5*fBarWidth + fGap));
  } else {
    if (copyNo%2 == 0)
      rotation->rotateX(0.0*deg);
    else
      rotation->rotateX(180.0*deg);

    rotation->rotateZ(90.0*deg);
    placement.setX(StartX + barIDinPlain*(0.5*fBarWidth + fGap));
    placement.setY(0.0*cm);
  }

  physVol->SetTranslation(placement);
  physVol->SetRotation(rotation);
}




