/* This file is part of MAUS: http://  micewww.pp.rl.ac.uk:8080/projects/maus
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
 * along with MAUS.  If not, see <http://  www.gnu.org/licenses/>.
 */

/* Author: Peter Lane
 */

#include "Recon/Global/Particle.hh"

#include "src/common_cpp/DataStructure/Global/ReconEnums.hh"
#include "Utils/Squeak.hh"

namespace MAUS {
namespace recon {
namespace global {
  using MAUS::DataStructure::Global::PID;

Particle::Particle() {
  AddParticleDefinition(MAUS::DataStructure::Global::kNoPID,
                        "none",
                        0.,
                        0);

  AddParticleDefinition(MAUS::DataStructure::Global::kEMinus,
                        "e-",
                        0.510998910,
                        -1);

  AddParticleDefinition(MAUS::DataStructure::Global::kElectronNeutrino,
                        "electron neutrino",
                        1.9e-6,
                        0);

  AddParticleDefinition(MAUS::DataStructure::Global::kMuMinus,
                        "mu-",
                        105.6583668,
                        -1);

  AddParticleDefinition(MAUS::DataStructure::Global::kMuonNeutrino,
                        "muon neutrino",
                        1.9e-6,
                        0);

  AddParticleDefinition(MAUS::DataStructure::Global::kPhoton,
                        "photon",
                        0.,
                        -1);

  AddParticleDefinition(MAUS::DataStructure::Global::kPi0,
                        "pi0",
                        134.9766,
                        0);

  AddParticleDefinition(MAUS::DataStructure::Global::kPiPlus,
                        "pi+",
                        139.57018,
                        +1);

  AddParticleDefinition(MAUS::DataStructure::Global::kKPlus,
                        "K+",
                        493.667,
                        +1);

  AddParticleDefinition(MAUS::DataStructure::Global::kNeutron,
                        "neutron",
                        939.56536,
                        0);

  AddParticleDefinition(MAUS::DataStructure::Global::kProton,
                        "proton",
                        938.271996,
                        +1);

  AddParticleDefinition(MAUS::DataStructure::Global::kDeuterium,
                        "deuterium",
                        1876.1239,
                        0);

  AddParticleDefinition(MAUS::DataStructure::Global::kTritium,
                        "tritium",
                        2809.432,
                        0);

  AddParticleDefinition(MAUS::DataStructure::Global::kHelium3,
                        "3He",
                        2809.41346,
                        0);

  AddParticleDefinition(MAUS::DataStructure::Global::kHelium4,
                        "4He",
                        3728.4001,
                        0);

  AddParticleDefinition(MAUS::DataStructure::Global::kKLong,
                        "K0L",
                        497.614,
                        0);

  AddParticleDefinition(MAUS::DataStructure::Global::kKShort,
                        "K0S",
                        497.614,
                        0);

  AddParticleDefinition(MAUS::DataStructure::Global::kK0,
                        "K0",
                        497.614,
                        0);

  AddParticleDefinition(MAUS::DataStructure::Global::kLambda,
                        "lambda",
                        1115.683,
                        0);

  AddParticleDefinition(MAUS::DataStructure::Global::kEPlus,
                        "e+",
                        0.510998910,
                        +1);

  AddParticleDefinition(MAUS::DataStructure::Global::kElectronAntineutrino,
                        "electron antineutrino",
                        0.,
                        0);

  AddParticleDefinition(MAUS::DataStructure::Global::kMuPlus,
                        "mu+",
                        105.6583668,
                        +1);

  AddParticleDefinition(MAUS::DataStructure::Global::kMuonAntineutrino,
                        "muon antineutrino",
                        0.,
                        0);

  AddParticleDefinition(MAUS::DataStructure::Global::kPiMinus,
                        "pi-",
                        139.57018,
                        -1);

  AddParticleDefinition(MAUS::DataStructure::Global::kKMinus,
                        "K-",
                        493.667,
                        -1);

  AddParticleDefinition(MAUS::DataStructure::Global::kAntineutron,
                        "antineutron",
                        939.56536,
                        0);

  AddParticleDefinition(MAUS::DataStructure::Global::kAntiproton,
                        "antiproton",
                        938.271996,
                        -1);

  AddParticleDefinition(MAUS::DataStructure::Global::kAntilambda,
                        "antilambda",
                        1115.683,
                        0);
}

Particle::~Particle() { }

const Particle & Particle::GetInstance() {
  return Particle::kSingleton;
}

std::string Particle::GetName(MAUS::DataStructure::Global::PID id) const {
  return names_.find(id)->second;
}

double Particle::GetMass(MAUS::DataStructure::Global::PID id) const {
  return masses_.find(id)->second;
}

int Particle::GetCharge(MAUS::DataStructure::Global::PID id) const {
  return charges_.find(id)->second;
}

const Particle Particle::kSingleton = Particle();

void Particle::AddParticleDefinition(MAUS::DataStructure::Global::PID pid,
                                     std::string name,
                                     double mass,
                                     int charge) {
  names_.insert(
      std::pair<MAUS::DataStructure::Global::PID, std::string>(pid, name));
  masses_.insert(
      std::pair<MAUS::DataStructure::Global::PID, double>(pid, mass));
  charges_.insert(
      std::pair<MAUS::DataStructure::Global::PID, int>(pid, charge));
}


}  // namespace global
}  // namespace recon
}  // namespace MAUS
