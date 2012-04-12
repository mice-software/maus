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
  names.insert(std::pair<int, std::string>(0, std::string("none")));
  masses.insert(std::pair<int, double>(0, 0.));
  charges.insert(std::pair<int, int>(0, 0));

  names.insert(std::pair<int, std::string>(11, std::string("e-")));
  masses.insert(std::pair<int, double>(11, 0.510998910));
  charges.insert(std::pair<int, int>(11, -1));

  names.insert(std::pair<int, std::string>(12, std::string("electron neutrino")));
  masses.insert(std::pair<int, double>(12, 1.9e-6));
  charges.insert(std::pair<int, int>(12, 0));

  names.insert(std::pair<int, std::string>(13, std::string("mu-")));
  masses.insert(std::pair<int, double>(13, 105.6583668));
  charges.insert(std::pair<int, int>(13, -1));

  names.insert(std::pair<int, std::string>(14, std::string("muon neutrino")));
  masses.insert(std::pair<int, double>(14, 1.9e-6));
  charges.insert(std::pair<int, int>(14, 0));

  names.insert(std::pair<int, std::string>(22, std::string("photon")));
  masses.insert(std::pair<int, double>(22, 0.));
  charges.insert(std::pair<int, int>(22, -1));

  names.insert(std::pair<int, std::string>(111, std::string("pi0")));
  masses.insert(std::pair<int, double>(111, 134.9766));
  charges.insert(std::pair<int, int>(111, 0));

  names.insert(std::pair<int, std::string>(211, std::string("pi+")));
  masses.insert(std::pair<int, double>(211, 139.57018));
  charges.insert(std::pair<int, int>(211, +1));

  names.insert(std::pair<int, std::string>(321, std::string("K+")));
  masses.insert(std::pair<int, double>(321, 493.667));
  charges.insert(std::pair<int, int>(321, +1));

  names.insert(std::pair<int, std::string>(2112, std::string("neutron")));
  masses.insert(std::pair<int, double>(2112, 939.56536));
  charges.insert(std::pair<int, int>(2112, 0));

  names.insert(std::pair<int, std::string>(2212, std::string("proton")));
  masses.insert(std::pair<int, double>(2212, 938.271996));
  charges.insert(std::pair<int, int>(2212, +1));

  names.insert(std::pair<int, std::string>(1000010020, std::string("deuterium")));
  masses.insert(std::pair<int, double>(1000010020, 1876.1239));
  charges.insert(std::pair<int, int>(1000010020, 0));

  names.insert(std::pair<int, std::string>(1000010030, std::string("tritium")));
  masses.insert(std::pair<int, double>(1000010030, 2809.432));
  charges.insert(std::pair<int, int>(1000010030, 0));

  names.insert(std::pair<int, std::string>(1000020030, std::string("3He")));
  masses.insert(std::pair<int, double>(1000020030, 2809.41346));
  charges.insert(std::pair<int, int>(1000020030, 0));

  names.insert(std::pair<int, std::string>(1000020040, std::string("4He")));
  masses.insert(std::pair<int, double>(1000020040, 3728.4001));
  charges.insert(std::pair<int, int>(1000020040, 0));

  names.insert(std::pair<int, std::string>(130, std::string("K0L")));
  masses.insert(std::pair<int, double>(130, 497.614));
  charges.insert(std::pair<int, int>(130, 0));

  names.insert(std::pair<int, std::string>(310, std::string("K0S")));
  masses.insert(std::pair<int, double>(310, 497.614));
  charges.insert(std::pair<int, int>(310, 0));

  names.insert(std::pair<int, std::string>(311, std::string("K0")));
  masses.insert(std::pair<int, double>(311, 497.614));
  charges.insert(std::pair<int, int>(311, 0));

  names.insert(std::pair<int, std::string>(3122, std::string("lambda")));
  masses.insert(std::pair<int, double>(3122, 1115.683));
  charges.insert(std::pair<int, int>(3122, 0));

  names.insert(std::pair<int, std::string>(-11, std::string("e+")));
  masses.insert(std::pair<int, double>(-11, 0.510998910));
  charges.insert(std::pair<int, int>(-11, +1));

  names.insert(std::pair<int, std::string>(-12, std::string(
    "electron antineutrino")));
  masses.insert(std::pair<int, double>(-12, 0.));
  charges.insert(std::pair<int, int>(-12, 0));

  names.insert(std::pair<int, std::string>(-13, std::string("mu+")));
  masses.insert(std::pair<int, double>(-13, 105.6583668));
  charges.insert(std::pair<int, int>(-13, +1));

  names.insert(std::pair<int, std::string>(-14, std::string(
    "muon antineutrino")));
  masses.insert(std::pair<int, double>(-14, 0.));
  charges.insert(std::pair<int, int>(-14, 0));

  names.insert(std::pair<int, std::string>(-211, std::string("pi-")));
  masses.insert(std::pair<int, double>(-211, 139.57018));
  charges.insert(std::pair<int, int>(-211, -1));

  names.insert(std::pair<int, std::string>(-321, std::string("K-")));
  masses.insert(std::pair<int, double>(-321, 493.667));
  charges.insert(std::pair<int, int>(-321, -1));

  names.insert(std::pair<int, std::string>(-2112, std::string("antineutron")));
  masses.insert(std::pair<int, double>(-2112, 939.56536));
  charges.insert(std::pair<int, int>(-2112, 0));

  names.insert(std::pair<int, std::string>(-2212, std::string("antiproton")));
  masses.insert(std::pair<int, double>(-2212, 938.271996));
  charges.insert(std::pair<int, int>(-2212, -1));

  names.insert(std::pair<int, std::string>(-3122, std::string("antilambda")));
  masses.insert(std::pair<int, double>(-3122, 1115.683));
  charges.insert(std::pair<int, int>(-3122, 0));
}

Particle::Particle() { }

Particle const * Particle::GetInstance() const {
  return &Particle::singleton;
}

std::string Particle::GetName(int id) const {
  return names.find(id).second;
}

double Particle::GetMass(int id) const {
  return masses.find(id).second;
}

int Particle::GetCharge(int id) const {
  return charges.find(id).second;
}

}  // namespace MAUS
