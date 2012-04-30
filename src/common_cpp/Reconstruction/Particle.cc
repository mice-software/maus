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

#include "Reconstruction/Particle.hh"

#include "Interface/Squeak.hh"

namespace MAUS {

using MAUS::Particle;

Particle::Particle() {
  names_.insert(std::pair<int, std::string>(kNone, std::string("none")));
  masses_.insert(std::pair<int, double>(kNone, 0.));
  charges_.insert(std::pair<int, int>(kNone, 0));

  names_.insert(std::pair<int, std::string>(kEMinus, std::string("e-")));
  masses_.insert(std::pair<int, double>(kEMinus, 0.510998910));
  charges_.insert(std::pair<int, int>(kEMinus, -1));

  names_.insert(std::pair<int, std::string>(kElectronNeutrino, std::string("electron neutrino")));
  masses_.insert(std::pair<int, double>(kElectronNeutrino, 1.9e-6));
  charges_.insert(std::pair<int, int>(kElectronNeutrino, 0));

  names_.insert(std::pair<int, std::string>(kMuMinus, std::string("mu-")));
  masses_.insert(std::pair<int, double>(kMuMinus, 105.6583668));
  charges_.insert(std::pair<int, int>(kMuMinus, -1));

  names_.insert(std::pair<int, std::string>(kMuonNeutrino, std::string("muon neutrino")));
  masses_.insert(std::pair<int, double>(kMuonNeutrino, 1.9e-6));
  charges_.insert(std::pair<int, int>(kMuonNeutrino, 0));

  names_.insert(std::pair<int, std::string>(kPhoton, std::string("photon")));
  masses_.insert(std::pair<int, double>(kPhoton, 0.));
  charges_.insert(std::pair<int, int>(kPhoton, -1));

  names_.insert(std::pair<int, std::string>(kPi0, std::string("pi0")));
  masses_.insert(std::pair<int, double>(kPi0, 134.9766));
  charges_.insert(std::pair<int, int>(kPi0, 0));

  names_.insert(std::pair<int, std::string>(kPiPlus, std::string("pi+")));
  masses_.insert(std::pair<int, double>(kPiPlus, 139.57018));
  charges_.insert(std::pair<int, int>(kPiPlus, +1));

  names_.insert(std::pair<int, std::string>(kKPlus, std::string("K+")));
  masses_.insert(std::pair<int, double>(kKPlus, 493.667));
  charges_.insert(std::pair<int, int>(kKPlus, +1));

  names_.insert(std::pair<int, std::string>(kNeutron, std::string("neutron")));
  masses_.insert(std::pair<int, double>(kNeutron, 939.56536));
  charges_.insert(std::pair<int, int>(kNeutron, 0));

  names_.insert(std::pair<int, std::string>(kProton, std::string("proton")));
  masses_.insert(std::pair<int, double>(kProton, 938.271996));
  charges_.insert(std::pair<int, int>(kProton, +1));

  names_.insert(std::pair<int, std::string>(kDeuterium, std::string("deuterium")));
  masses_.insert(std::pair<int, double>(kDeuterium, 1876.1239));
  charges_.insert(std::pair<int, int>(kDeuterium, 0));

  names_.insert(std::pair<int, std::string>(kTritium, std::string("tritium")));
  masses_.insert(std::pair<int, double>(kTritium, 2809.432));
  charges_.insert(std::pair<int, int>(kTritium, 0));

  names_.insert(std::pair<int, std::string>(kHelium3, std::string("3He")));
  masses_.insert(std::pair<int, double>(kHelium3, 2809.41346));
  charges_.insert(std::pair<int, int>(kHelium3, 0));

  names_.insert(std::pair<int, std::string>(kHelium4, std::string("4He")));
  masses_.insert(std::pair<int, double>(kHelium4, 3728.4001));
  charges_.insert(std::pair<int, int>(kHelium4, 0));

  names_.insert(std::pair<int, std::string>(kKLong, std::string("K0L")));
  masses_.insert(std::pair<int, double>(kKLong, 497.614));
  charges_.insert(std::pair<int, int>(kKLong, 0));

  names_.insert(std::pair<int, std::string>(kKShort, std::string("K0S")));
  masses_.insert(std::pair<int, double>(kKShort, 497.614));
  charges_.insert(std::pair<int, int>(kKShort, 0));

  names_.insert(std::pair<int, std::string>(kK0, std::string("K0")));
  masses_.insert(std::pair<int, double>(kK0, 497.614));
  charges_.insert(std::pair<int, int>(kK0, 0));

  names_.insert(std::pair<int, std::string>(kLambda, std::string("lambda")));
  masses_.insert(std::pair<int, double>(kLambda, 1115.683));
  charges_.insert(std::pair<int, int>(kLambda, 0));

  names_.insert(std::pair<int, std::string>(kEPlus, std::string("e+")));
  masses_.insert(std::pair<int, double>(kEPlus, 0.510998910));
  charges_.insert(std::pair<int, int>(kEPlus, +1));

  names_.insert(std::pair<int, std::string>(kElectronAntineutrino, std::string(
    "electron antineutrino")));
  masses_.insert(std::pair<int, double>(kElectronAntineutrino, 0.));
  charges_.insert(std::pair<int, int>(kElectronAntineutrino, 0));

  names_.insert(std::pair<int, std::string>(kMuPlus, std::string("mu+")));
  masses_.insert(std::pair<int, double>(kMuPlus, 105.6583668));
  charges_.insert(std::pair<int, int>(kMuPlus, +1));

  names_.insert(std::pair<int, std::string>(kMuonAntineutrino, std::string(
    "muon antineutrino")));
  masses_.insert(std::pair<int, double>(kMuonAntineutrino, 0.));
  charges_.insert(std::pair<int, int>(kMuonAntineutrino, 0));

  names_.insert(std::pair<int, std::string>(kPiMinus, std::string("pi-")));
  masses_.insert(std::pair<int, double>(kPiMinus, 139.57018));
  charges_.insert(std::pair<int, int>(kPiMinus, -1));

  names_.insert(std::pair<int, std::string>(kKMinus, std::string("K-")));
  masses_.insert(std::pair<int, double>(kKMinus, 493.667));
  charges_.insert(std::pair<int, int>(kKMinus, -1));

  names_.insert(std::pair<int, std::string>(kAntineutron, std::string("antineutron")));
  masses_.insert(std::pair<int, double>(kAntineutron, 939.56536));
  charges_.insert(std::pair<int, int>(kAntineutron, 0));

  names_.insert(std::pair<int, std::string>(kAntiproton, std::string("antiproton")));
  masses_.insert(std::pair<int, double>(kAntiproton, 938.271996));
  charges_.insert(std::pair<int, int>(kAntiproton, -1));

  names_.insert(std::pair<int, std::string>(kAntilambda, std::string("antilambda")));
  masses_.insert(std::pair<int, double>(kAntilambda, 1115.683));
  charges_.insert(std::pair<int, int>(kAntilambda, 0));
}

Particle::~Particle() { }

Particle const * Particle::GetInstance() {
  return &Particle::kSingleton;
}

std::string Particle::GetName(int id) const {
  return names_.find(id)->second;
}

double Particle::GetMass(int id) const {
  return masses_.find(id)->second;
}

int Particle::GetCharge(int id) const {
  return charges_.find(id)->second;
}

const int Particle::kNone = 0;
const int Particle::kEMinus = 11;
const int Particle::kElectronNeutrino = 12;
const int Particle::kMuMinus = 13;
const int Particle::kMuonNeutrino = 14;
const int Particle::kPhoton = 22;
const int Particle::kPi0 = 111;
const int Particle::kPiPlus = 211;
const int Particle::kKPlus = 321;
const int Particle::kNeutron = 2112;
const int Particle::kProton = 2212;
const int Particle::kDeuterium = 1000010020;
const int Particle::kTritium = 1000010030;
const int Particle::kHelium3 = 1000020030;
const int Particle::kHelium4 = 1000020040;
const int Particle::kKLong = 130;
const int Particle::kKShort = 310;
const int Particle::kK0 = 311;
const int Particle::kLambda = 3122;
const int Particle::kEPlus = -kEMinus;
const int Particle::kElectronAntineutrino = -kElectronNeutrino;
const int Particle::kMuPlus = -kMuMinus;
const int Particle::kMuonAntineutrino = -kMuonNeutrino;
const int Particle::kPiMinus = -kPiPlus;
const int Particle::kKMinus = -kKPlus;
const int Particle::kAntineutron = -kNeutron;
const int Particle::kAntiproton = -kProton;
const int Particle::kAntilambda = -kLambda;

const Particle Particle::kSingleton = Particle();

}  // namespace MAUS
