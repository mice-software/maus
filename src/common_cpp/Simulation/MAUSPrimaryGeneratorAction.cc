// MAUS WARNING: THIS IS LEGACY CODE.
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

#include <limits>
#include <queue>

#include "Geant4/G4Event.hh"
#include "Geant4/G4PrimaryVertex.hh"
#include "Geant4/G4Track.hh"
#include "Geant4/G4ios.hh"

#include "CLHEP/Random/Random.h"

#include "src/legacy/Interface/MICERun.hh"
#include "src/legacy/Config/MiceModule.hh"

#include "src/common_cpp/Simulation/MAUSPrimaryGeneratorAction.hh"

namespace MAUS {

/// Define static location of generator action
MAUSPrimaryGeneratorAction*
MAUSPrimaryGeneratorAction::self = 0;

MAUSPrimaryGeneratorAction::MAUSPrimaryGeneratorAction() {
  gun = new G4ParticleGun();
}

MAUSPrimaryGeneratorAction::~MAUSPrimaryGeneratorAction() {
  delete gun;
}

MAUSPrimaryGeneratorAction::PGParticle MAUSPrimaryGeneratorAction::Pop() {
  MAUSPrimaryGeneratorAction::PGParticle part = _part_q.front();
  _part_q.pop();
  return part;
}

void MAUSPrimaryGeneratorAction::GeneratePrimaries(G4Event* argEvent) {
  if (_part_q.size() == 0)
    throw(Squeal(Squeal::recoverable,
                 "No primary particles",
                 "MAUSPrimaryGeneratorAction::GeneratePrimaries"));
  PGParticle part = Pop();
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle = particleTable->FindParticle(part.pid);
  if ( particle == NULL )
    throw(Squeal(Squeal::recoverable,
                 "Particle pid not recognised",
                 "MAUSPrimaryGeneratorAction::GeneratePrimaries"));
  if ( part.energy < particle->GetPDGMass() )
    throw(Squeal(Squeal::recoverable,
                 "Particle total energy less than particle mass",
                 "MAUSPrimaryGeneratorAction::GeneratePrimaries"));
  if (!isInWorldVolume(part.x, part.y, part.z)) {
    throw(Squeal(Squeal::recoverable,
                 "Particle is outside world volume",
                 "MAUSPrimaryGeneratorAction::GeneratePrimaries"));
  }
  gun->SetParticleDefinition(particle);

  // Get this class' variables to define next event.
  gun->SetParticlePosition(G4ThreeVector
                                    (part.x, part.y, part.z));
  gun->SetParticleTime(part.time);
  gun->SetParticleEnergy(part.energy-particle->GetPDGMass());
  gun->SetParticleMomentumDirection(G4ThreeVector
                                 (part.px, part.py, part.pz));
  gun->GeneratePrimaryVertex(argEvent);
  unsigned int uint_max = std::numeric_limits<unsigned int>::max();
  if ( part.seed < 0 || part.seed > uint_max ) {
    throw(Squeal(Squeal::recoverable,
                 "Random seed out of range",
                 "MAUSPrimaryGeneratorAction::GeneratePrimaries"));
  }

  CLHEP::HepRandom::setTheSeed(part.seed);
}

bool MAUSPrimaryGeneratorAction::isInWorldVolume(double x, double y, double z) {
  MiceModule* root = MICERun::getInstance()->miceModule;
  if (root == NULL) {
      return true;
  }
  bool isInside = fabs(x) < root->dimensions().x()/2. &&
                  fabs(y) < root->dimensions().y()/2. &&
                  fabs(z) < root->dimensions().z()/2.;
  return isInside;
}

MAUSPrimaryGeneratorAction::PGParticle::PGParticle()
  : x(0.), y(0.), z(0.), time(0.), px(0.), py(0.), pz(0.), energy(0.), pid(0),
  seed(0) {
}

void MAUSPrimaryGeneratorAction::PGParticle::ReadJson(Json::Value particle) {
  Json::Value pos = JsonWrapper::GetProperty
                             (particle, "position", JsonWrapper::objectValue);
  Json::Value mom = JsonWrapper::GetProperty
                             (particle, "momentum", JsonWrapper::objectValue);
  pid = JsonWrapper::GetProperty
                       (particle, "particle_id", JsonWrapper::intValue).asInt();
  seed = JsonWrapper::GetProperty
                     (particle, "random_seed", JsonWrapper::intValue).asUInt();
  x = JsonWrapper::GetProperty(pos, "x", JsonWrapper::realValue).asDouble();
  y = JsonWrapper::GetProperty(pos, "y", JsonWrapper::realValue).asDouble();
  z = JsonWrapper::GetProperty(pos, "z", JsonWrapper::realValue).asDouble();
  px = JsonWrapper::GetProperty(mom, "x", JsonWrapper::realValue).asDouble();
  py = JsonWrapper::GetProperty(mom, "y", JsonWrapper::realValue).asDouble();
  pz = JsonWrapper::GetProperty(mom, "z", JsonWrapper::realValue).asDouble();
  energy = JsonWrapper::GetProperty
                        (particle, "energy", JsonWrapper::realValue).asDouble();
  time = JsonWrapper::GetProperty
                        (particle, "time", JsonWrapper::realValue).asDouble();
}

Json::Value MAUSPrimaryGeneratorAction::PGParticle::WriteJson() {
  Json::Value pos(Json::objectValue);
  pos["x"] = Json::Value(x);
  pos["y"] = Json::Value(y);
  pos["z"] = Json::Value(z);

  Json::Value mom(Json::objectValue);
  mom["x"] = Json::Value(px);
  mom["y"] = Json::Value(py);
  mom["z"] = Json::Value(pz);

  Json::Value particle(Json::objectValue);
  particle["position"] = pos;
  particle["momentum"] = mom;
  particle["particle_id"] = Json::Value(pid);
  particle["random_seed"] = Json::Value(Json::Int(seed));

  particle["energy"] = Json::Value(energy);
  particle["time"] = Json::Value(time);
  return particle;
}


MAUSPrimaryGeneratorAction::PGParticle::PGParticle(VirtualHit hit) {
    x = hit.GetPos().x();
    y = hit.GetPos().y();
    z = hit.GetPos().z();
    time = hit.GetTime();
    px = hit.GetMomentum().x();
    py = hit.GetMomentum().y();
    pz = hit.GetMomentum().z();
    energy = hit.GetEnergy();
    pid = hit.GetPID();
    seed = 0;
}

}  // ends MAUS namespace
