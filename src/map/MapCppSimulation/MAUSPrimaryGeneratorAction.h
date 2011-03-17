/** @class  MAUSPrimaryGeneratorAction
 *  Geant4 calls this class to determine next event
 *
 *  @author Christopher Tunnell <c.tunnell1@physics.ox.ac.uk>
 *
 *  This class gets called as the primary event generator for
 *  geant4.  When setting up geant4, you can use this but must
 *  be sure to call the 'Set' methods of this class to tell
 *  it what the next event to simulate is.  Otherwise, it'll
 *  simulate the last event.
 *
 *  Copyright 2010 c.tunnell1@physics.ox.ac.uk
 *
 */

#ifndef _COMPONENTS_MAP_MAUSPRIMARYGENERATORACTION_H_
#define _COMPONENTS_MAP_MAUSPRIMARYGENERATORACTION_H_

#include <G4ParticleGun.hh>
#include <G4ParticleTable.hh>
#include <G4VUserPrimaryGeneratorAction.hh>  // inherit from
#include <string>
#include "Interface/Squeak.hh"

namespace MAUS {

class MAUSPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
 public:
  MAUSPrimaryGeneratorAction();

  void GeneratePrimaries(G4Event * argEvent);  // generate primary particles

  // Set next event that generate primaries gives
  void SetNextParticleID(int argPid)
  { pid = argPid;  // geant4 particle number
    updated = true; }

  void SetNextPositionVector(double argX, double argY, double argZ)
  { X = argX;  // mm
    Y = argY;  // mm
    Z = argZ;  // mm
    updated = true; }

  void SetNextEnergy(double argEnergy)
  { Energy = argEnergy;  // MeV/c^2
    updated = true; }

  void SetNextMomentumUnitVector(double argPxu, double argPyu, double argPzu)
  { Pxu = argPxu;  // NOTE: UNIT VECTOR! (Rogers: G4 normalises so okay)
    Pyu = argPyu;  // NOTE: UNIT VECTOR!
    Pzu = argPzu;  // NOTE: UNIT VECTOR!
    updated = true; }

 protected:
  G4ParticleGun*          gun;
  static MAUSPrimaryGeneratorAction* self;

  int pid;               // Geant4 particle ID
  double X, Y, Z;        // Position vector (mm)
  double Energy;         // Energy (MeV)
  double Pxu, Pyu, Pzu;  // Momentum unit vector

 private:
  /// State of class if update
  //
  //  This variable is checked during GeneratePrimaries
  //  to ensure that the program has given new set values,
  //  and will complain otherwise.
  bool updated;
};

}  // ends MAUS namespace

#endif  // _COMPONENTS_MAP_MAUSPRIMARYGENERATORACTION_H_
