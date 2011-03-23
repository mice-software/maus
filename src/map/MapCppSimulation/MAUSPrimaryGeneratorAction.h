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

#ifndef _SRC_MAP_MAUSPRIMARYGENERATORACTION_H_
#define _SRC_MAP_MAUSPRIMARYGENERATORACTION_H_

#include <string>
#include <queue>

#include <G4ParticleGun.hh>
#include <G4ParticleTable.hh>
#include <G4VUserPrimaryGeneratorAction.hh>  // inherit from

#include "Interface/Squeak.hh"

namespace MAUS {

class MAUSPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
 public:
  class PGParticle {
   public:
    double x, y, z, time, px, py, pz, energy, pid, seed;
  };

  MAUSPrimaryGeneratorAction();

  void GeneratePrimaries(G4Event * argEvent);  // generate primary particles

  // Set next event that generate primaries gives
  void Push(PGParticle particle) {_part_q.push(particle);}

 protected:
  G4ParticleGun*          gun;
  static MAUSPrimaryGeneratorAction* self;

 private:
  std::queue<PGParticle> _part_q;
};


}  // ends MAUS namespace

#endif  // _COMPONENTS_MAP_MAUSPRIMARYGENERATORACTION_H_
