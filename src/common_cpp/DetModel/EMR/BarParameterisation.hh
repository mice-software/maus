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

#ifndef BarParameterisation_H
#define BarParameterisation_H 1

#include "Geant4/globals.hh"
#include "Geant4/G4VPVParameterisation.hh"
#include "Geant4/G4RotationMatrix.hh"

class G4VPhysicalVolume;
class G4Box;

// Dummy declarations to get rid of warnings ...
class G4Trd;
class G4Trap;
class G4Cons;
class G4Orb;
class G4Sphere;
class G4Torus;
class G4Para;
class G4Hype;
class G4Tubs;
class G4Polycone;
class G4Polyhedra;

class BarParameterisation : public G4VPVParameterisation {
 public:
  BarParameterisation(G4double StartZ,
                      G4double BarWidth,
                      G4int BarHeight,
                      G4double Gap);

  virtual ~BarParameterisation();

  void ComputeTransformation(const G4int copyNo, G4VPhysicalVolume* physVol) const;

 private:
  // Dummy declarations to get rid of warnings ...
  void ComputeDimensions(G4Box&,       const G4int, const G4VPhysicalVolume*) const {}
  void ComputeDimensions(G4Trd&,       const G4int, const G4VPhysicalVolume*) const {}
  void ComputeDimensions(G4Trap&,      const G4int, const G4VPhysicalVolume*) const {}
  void ComputeDimensions(G4Cons&,      const G4int, const G4VPhysicalVolume*) const {}
  void ComputeDimensions(G4Sphere&,    const G4int, const G4VPhysicalVolume*) const {}
  void ComputeDimensions(G4Orb&,       const G4int, const G4VPhysicalVolume*) const {}
  void ComputeDimensions(G4Torus&,     const G4int, const G4VPhysicalVolume*) const {}
  void ComputeDimensions(G4Para&,      const G4int, const G4VPhysicalVolume*) const {}
  void ComputeDimensions(G4Hype&,      const G4int, const G4VPhysicalVolume*) const {}
  void ComputeDimensions(G4Tubs&,      const G4int, const G4VPhysicalVolume*) const {}
  void ComputeDimensions(G4Polycone&,  const G4int, const G4VPhysicalVolume*) const {}
  void ComputeDimensions(G4Polyhedra&, const G4int, const G4VPhysicalVolume*) const {}

  G4double fBarWidth;
  G4double fBarHeight;
  G4int fNbOfBars;
  G4double fGap;

  G4RotationMatrix *rotation;
};

#endif


