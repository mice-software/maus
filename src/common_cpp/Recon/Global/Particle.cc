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
#include "Interface/Squeak.hh"

namespace MAUS {
namespace recon {
namespace global {
  using MAUS::DataStructure::Global::PID;
  using MAUS::DataStructure::Global::kNoPID;
  using MAUS::DataStructure::Global::kEMinus;
  using MAUS::DataStructure::Global::kEPlus;
  using MAUS::DataStructure::Global::kElectronNeutrino;
  using MAUS::DataStructure::Global::kElectronAntineutrino;
  using MAUS::DataStructure::Global::kMuMinus;
  using MAUS::DataStructure::Global::kMuPlus;
  using MAUS::DataStructure::Global::kMuonNeutrino;
  using MAUS::DataStructure::Global::kMuonAntineutrino;
  using MAUS::DataStructure::Global::kPhoton;
  using MAUS::DataStructure::Global::kPiMinus;
  using MAUS::DataStructure::Global::kPi0;
  using MAUS::DataStructure::Global::kPiPlus;
  using MAUS::DataStructure::Global::kKMinus;
  using MAUS::DataStructure::Global::kK0;
  using MAUS::DataStructure::Global::kKLong;
  using MAUS::DataStructure::Global::kKShort;
  using MAUS::DataStructure::Global::kKPlus;
  using MAUS::DataStructure::Global::kNeutron;
  using MAUS::DataStructure::Global::kAntineutron;
  using MAUS::DataStructure::Global::kProton;
  using MAUS::DataStructure::Global::kAntiproton;
  using MAUS::DataStructure::Global::kLambda;
  using MAUS::DataStructure::Global::kAntilambda;
  using MAUS::DataStructure::Global::kDeuterium;
  using MAUS::DataStructure::Global::kTritium;
  using MAUS::DataStructure::Global::kHelium3;
  using MAUS::DataStructure::Global::kHelium4;

Particle::Particle() {
  names_.insert(std::pair<PID, std::string>(kNoPID, std::string("none")));
  masses_.insert(std::pair<PID, double>(kNoPID, 0.));
  charges_.insert(std::pair<PID, int>(kNoPID, 0));

  names_.insert(std::pair<PID, std::string>(kEMinus, std::string("e-")));
  masses_.insert(std::pair<PID, double>(kEMinus, 0.510998910));
  charges_.insert(std::pair<PID, int>(kEMinus, -1));

  names_.insert(std::pair<PID, std::string>(kElectronNeutrino, std::string("electron neutrino")));
  masses_.insert(std::pair<PID, double>(kElectronNeutrino, 1.9e-6));
  charges_.insert(std::pair<PID, int>(kElectronNeutrino, 0));

  names_.insert(std::pair<PID, std::string>(kMuMinus, std::string("mu-")));
  masses_.insert(std::pair<PID, double>(kMuMinus, 105.6583668));
  charges_.insert(std::pair<PID, int>(kMuMinus, -1));

  names_.insert(std::pair<PID, std::string>(kMuonNeutrino, std::string("muon neutrino")));
  masses_.insert(std::pair<PID, double>(kMuonNeutrino, 1.9e-6));
  charges_.insert(std::pair<PID, int>(kMuonNeutrino, 0));

  names_.insert(std::pair<PID, std::string>(kPhoton, std::string("photon")));
  masses_.insert(std::pair<PID, double>(kPhoton, 0.));
  charges_.insert(std::pair<PID, int>(kPhoton, -1));

  names_.insert(std::pair<PID, std::string>(kPi0, std::string("pi0")));
  masses_.insert(std::pair<PID, double>(kPi0, 134.9766));
  charges_.insert(std::pair<PID, int>(kPi0, 0));

  names_.insert(std::pair<PID, std::string>(kPiPlus, std::string("pi+")));
  masses_.insert(std::pair<PID, double>(kPiPlus, 139.57018));
  charges_.insert(std::pair<PID, int>(kPiPlus, +1));

  names_.insert(std::pair<PID, std::string>(kKPlus, std::string("K+")));
  masses_.insert(std::pair<PID, double>(kKPlus, 493.667));
  charges_.insert(std::pair<PID, int>(kKPlus, +1));

  names_.insert(std::pair<PID, std::string>(kNeutron, std::string("neutron")));
  masses_.insert(std::pair<PID, double>(kNeutron, 939.56536));
  charges_.insert(std::pair<PID, int>(kNeutron, 0));

  names_.insert(std::pair<PID, std::string>(kProton, std::string("proton")));
  masses_.insert(std::pair<PID, double>(kProton, 938.271996));
  charges_.insert(std::pair<PID, int>(kProton, +1));

  names_.insert(std::pair<PID, std::string>(kDeuterium, std::string("deuterium")));
  masses_.insert(std::pair<PID, double>(kDeuterium, 1876.1239));
  charges_.insert(std::pair<PID, int>(kDeuterium, 0));

  names_.insert(std::pair<PID, std::string>(kTritium, std::string("tritium")));
  masses_.insert(std::pair<PID, double>(kTritium, 2809.432));
  charges_.insert(std::pair<PID, int>(kTritium, 0));

  names_.insert(std::pair<PID, std::string>(kHelium3, std::string("3He")));
  masses_.insert(std::pair<PID, double>(kHelium3, 2809.41346));
  charges_.insert(std::pair<PID, int>(kHelium3, 0));

  names_.insert(std::pair<PID, std::string>(kHelium4, std::string("4He")));
  masses_.insert(std::pair<PID, double>(kHelium4, 3728.4001));
  charges_.insert(std::pair<PID, int>(kHelium4, 0));

  names_.insert(std::pair<PID, std::string>(kKLong, std::string("K0L")));
  masses_.insert(std::pair<PID, double>(kKLong, 497.614));
  charges_.insert(std::pair<PID, int>(kKLong, 0));

  names_.insert(std::pair<PID, std::string>(kKShort, std::string("K0S")));
  masses_.insert(std::pair<PID, double>(kKShort, 497.614));
  charges_.insert(std::pair<PID, int>(kKShort, 0));

  names_.insert(std::pair<PID, std::string>(kK0, std::string("K0")));
  masses_.insert(std::pair<PID, double>(kK0, 497.614));
  charges_.insert(std::pair<PID, int>(kK0, 0));

  names_.insert(std::pair<PID, std::string>(kLambda, std::string("lambda")));
  masses_.insert(std::pair<PID, double>(kLambda, 1115.683));
  charges_.insert(std::pair<PID, int>(kLambda, 0));

  names_.insert(std::pair<PID, std::string>(kEPlus, std::string("e+")));
  masses_.insert(std::pair<PID, double>(kEPlus, 0.510998910));
  charges_.insert(std::pair<PID, int>(kEPlus, +1));

  names_.insert(std::pair<PID, std::string>(kElectronAntineutrino, std::string(
    "electron antineutrino")));
  masses_.insert(std::pair<PID, double>(kElectronAntineutrino, 0.));
  charges_.insert(std::pair<PID, int>(kElectronAntineutrino, 0));

  names_.insert(std::pair<PID, std::string>(kMuPlus, std::string("mu+")));
  masses_.insert(std::pair<PID, double>(kMuPlus, 105.6583668));
  charges_.insert(std::pair<PID, int>(kMuPlus, +1));

  names_.insert(std::pair<PID, std::string>(kMuonAntineutrino, std::string(
    "muon antineutrino")));
  masses_.insert(std::pair<PID, double>(kMuonAntineutrino, 0.));
  charges_.insert(std::pair<PID, int>(kMuonAntineutrino, 0));

  names_.insert(std::pair<PID, std::string>(kPiMinus, std::string("pi-")));
  masses_.insert(std::pair<PID, double>(kPiMinus, 139.57018));
  charges_.insert(std::pair<PID, int>(kPiMinus, -1));

  names_.insert(std::pair<PID, std::string>(kKMinus, std::string("K-")));
  masses_.insert(std::pair<PID, double>(kKMinus, 493.667));
  charges_.insert(std::pair<PID, int>(kKMinus, -1));

  names_.insert(std::pair<PID, std::string>(kAntineutron, std::string("antineutron")));
  masses_.insert(std::pair<PID, double>(kAntineutron, 939.56536));
  charges_.insert(std::pair<PID, int>(kAntineutron, 0));

  names_.insert(std::pair<PID, std::string>(kAntiproton, std::string("antiproton")));
  masses_.insert(std::pair<PID, double>(kAntiproton, 938.271996));
  charges_.insert(std::pair<PID, int>(kAntiproton, -1));

  names_.insert(std::pair<PID, std::string>(kAntilambda, std::string("antilambda")));
  masses_.insert(std::pair<PID, double>(kAntilambda, 1115.683));
  charges_.insert(std::pair<PID, int>(kAntilambda, 0));
}

Particle::~Particle() { }

const Particle & Particle::GetInstance() {
  return Particle::kSingleton;
}

std::string Particle::GetName(PID id) const {
  return names_.find(id)->second;
}

double Particle::GetMass(PID id) const {
  return masses_.find(id)->second;
}

int Particle::GetCharge(PID id) const {
  return charges_.find(id)->second;
}

const Particle Particle::kSingleton = Particle();

}  // namespace global
}  // namespace recon
}  // namespace MAUS
