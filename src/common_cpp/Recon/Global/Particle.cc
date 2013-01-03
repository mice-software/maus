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

#include "Interface/Squeak.hh"

namespace MAUS {
namespace recon {
namespace global {

using MAUS::recon::global::Particle;

Particle::Particle() {
  names_.insert(std::pair<ID, std::string>(kNone, std::string("none")));
  masses_.insert(std::pair<ID, double>(kNone, 0.));
  charges_.insert(std::pair<ID, int>(kNone, 0));

  names_.insert(std::pair<ID, std::string>(kEMinus, std::string("e-")));
  masses_.insert(std::pair<ID, double>(kEMinus, 0.510998910));
  charges_.insert(std::pair<ID, int>(kEMinus, -1));

  names_.insert(std::pair<ID, std::string>(kElectronNeutrino, std::string("electron neutrino")));
  masses_.insert(std::pair<ID, double>(kElectronNeutrino, 1.9e-6));
  charges_.insert(std::pair<ID, int>(kElectronNeutrino, 0));

  names_.insert(std::pair<ID, std::string>(kMuMinus, std::string("mu-")));
  masses_.insert(std::pair<ID, double>(kMuMinus, 105.6583668));
  charges_.insert(std::pair<ID, int>(kMuMinus, -1));

  names_.insert(std::pair<ID, std::string>(kMuonNeutrino, std::string("muon neutrino")));
  masses_.insert(std::pair<ID, double>(kMuonNeutrino, 1.9e-6));
  charges_.insert(std::pair<ID, int>(kMuonNeutrino, 0));

  names_.insert(std::pair<ID, std::string>(kPhoton, std::string("photon")));
  masses_.insert(std::pair<ID, double>(kPhoton, 0.));
  charges_.insert(std::pair<ID, int>(kPhoton, -1));

  names_.insert(std::pair<ID, std::string>(kPi0, std::string("pi0")));
  masses_.insert(std::pair<ID, double>(kPi0, 134.9766));
  charges_.insert(std::pair<ID, int>(kPi0, 0));

  names_.insert(std::pair<ID, std::string>(kPiPlus, std::string("pi+")));
  masses_.insert(std::pair<ID, double>(kPiPlus, 139.57018));
  charges_.insert(std::pair<ID, int>(kPiPlus, +1));

  names_.insert(std::pair<ID, std::string>(kKPlus, std::string("K+")));
  masses_.insert(std::pair<ID, double>(kKPlus, 493.667));
  charges_.insert(std::pair<ID, int>(kKPlus, +1));

  names_.insert(std::pair<ID, std::string>(kNeutron, std::string("neutron")));
  masses_.insert(std::pair<ID, double>(kNeutron, 939.56536));
  charges_.insert(std::pair<ID, int>(kNeutron, 0));

  names_.insert(std::pair<ID, std::string>(kProton, std::string("proton")));
  masses_.insert(std::pair<ID, double>(kProton, 938.271996));
  charges_.insert(std::pair<ID, int>(kProton, +1));

  names_.insert(std::pair<ID, std::string>(kDeuterium, std::string("deuterium")));
  masses_.insert(std::pair<ID, double>(kDeuterium, 1876.1239));
  charges_.insert(std::pair<ID, int>(kDeuterium, 0));

  names_.insert(std::pair<ID, std::string>(kTritium, std::string("tritium")));
  masses_.insert(std::pair<ID, double>(kTritium, 2809.432));
  charges_.insert(std::pair<ID, int>(kTritium, 0));

  names_.insert(std::pair<ID, std::string>(kHelium3, std::string("3He")));
  masses_.insert(std::pair<ID, double>(kHelium3, 2809.41346));
  charges_.insert(std::pair<ID, int>(kHelium3, 0));

  names_.insert(std::pair<ID, std::string>(kHelium4, std::string("4He")));
  masses_.insert(std::pair<ID, double>(kHelium4, 3728.4001));
  charges_.insert(std::pair<ID, int>(kHelium4, 0));

  names_.insert(std::pair<ID, std::string>(kKLong, std::string("K0L")));
  masses_.insert(std::pair<ID, double>(kKLong, 497.614));
  charges_.insert(std::pair<ID, int>(kKLong, 0));

  names_.insert(std::pair<ID, std::string>(kKShort, std::string("K0S")));
  masses_.insert(std::pair<ID, double>(kKShort, 497.614));
  charges_.insert(std::pair<ID, int>(kKShort, 0));

  names_.insert(std::pair<ID, std::string>(kK0, std::string("K0")));
  masses_.insert(std::pair<ID, double>(kK0, 497.614));
  charges_.insert(std::pair<ID, int>(kK0, 0));

  names_.insert(std::pair<ID, std::string>(kLambda, std::string("lambda")));
  masses_.insert(std::pair<ID, double>(kLambda, 1115.683));
  charges_.insert(std::pair<ID, int>(kLambda, 0));

  names_.insert(std::pair<ID, std::string>(kEPlus, std::string("e+")));
  masses_.insert(std::pair<ID, double>(kEPlus, 0.510998910));
  charges_.insert(std::pair<ID, int>(kEPlus, +1));

  names_.insert(std::pair<ID, std::string>(kElectronAntineutrino, std::string(
    "electron antineutrino")));
  masses_.insert(std::pair<ID, double>(kElectronAntineutrino, 0.));
  charges_.insert(std::pair<ID, int>(kElectronAntineutrino, 0));

  names_.insert(std::pair<ID, std::string>(kMuPlus, std::string("mu+")));
  masses_.insert(std::pair<ID, double>(kMuPlus, 105.6583668));
  charges_.insert(std::pair<ID, int>(kMuPlus, +1));

  names_.insert(std::pair<ID, std::string>(kMuonAntineutrino, std::string(
    "muon antineutrino")));
  masses_.insert(std::pair<ID, double>(kMuonAntineutrino, 0.));
  charges_.insert(std::pair<ID, int>(kMuonAntineutrino, 0));

  names_.insert(std::pair<ID, std::string>(kPiMinus, std::string("pi-")));
  masses_.insert(std::pair<ID, double>(kPiMinus, 139.57018));
  charges_.insert(std::pair<ID, int>(kPiMinus, -1));

  names_.insert(std::pair<ID, std::string>(kKMinus, std::string("K-")));
  masses_.insert(std::pair<ID, double>(kKMinus, 493.667));
  charges_.insert(std::pair<ID, int>(kKMinus, -1));

  names_.insert(std::pair<ID, std::string>(kAntineutron, std::string("antineutron")));
  masses_.insert(std::pair<ID, double>(kAntineutron, 939.56536));
  charges_.insert(std::pair<ID, int>(kAntineutron, 0));

  names_.insert(std::pair<ID, std::string>(kAntiproton, std::string("antiproton")));
  masses_.insert(std::pair<ID, double>(kAntiproton, 938.271996));
  charges_.insert(std::pair<ID, int>(kAntiproton, -1));

  names_.insert(std::pair<ID, std::string>(kAntilambda, std::string("antilambda")));
  masses_.insert(std::pair<ID, double>(kAntilambda, 1115.683));
  charges_.insert(std::pair<ID, int>(kAntilambda, 0));
}

Particle::~Particle() { }

Particle const * Particle::GetInstance() {
  return &Particle::kSingleton;
}

std::string Particle::GetName(ID id) const {
  return names_.find(id)->second;
}

double Particle::GetMass(ID id) const {
  return masses_.find(id)->second;
}

int Particle::GetCharge(ID id) const {
  return charges_.find(id)->second;
}

const Particle Particle::kSingleton = Particle();

}  // namespace global
}  // namespace recon
}  // namespace MAUS
