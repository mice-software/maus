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

#ifndef COMMON_CPP_RECONSTRUCTION_PARTICLE_HH
#define COMMON_CPP_RECONSTRUCTION_PARTICLE_HH

#include <iostream>
#include <string>

#include "src/common_cpp/Optics/PhaseSpaceVector.hh"
     
namespace MAUS {

using MAUS::Particle;

class Particle {
 public:
  static Particle const * GetInstance();
  ~Particle();

  /* @brief returns the unique name of the particle referenced by id.
   */
  std::string GetName(int id) const;
  
  /* @brief returns the mass (MeV/c^2) of the particle referenced by id.
   */
  double GetMass(int id) const;
  
  /* @brief returns the charge (e) of the particle reference by id.
   */
  int GetCharge(int id) const;
  
  static const int kNone = 0;
  static const int kEMinus = 11;
  static const int kElectronNeutrino = 12;
  static const int kMuMinus = 13;
  static const int kMuonNeutrino = 14;
  static const int kPhoton = 22;
  static const int kPi0 = 111;
  static const int kPiPlus = 211;
  static const int kKPlus = 321;
  static const int kNeutron = 2112;
  static const int kProton = 2212;
  static const int kDeuterium = 1000010020;
  static const int kTritium = 1000010030;
  static const int kHelium3 = 1000020030;
  static const int kHelium4 = 1000020040;
  static const int kKLong = 130;
  static const int kKShort = 310;
  static const int kK0 = 311;
  static const int kLambda = 3122;
  static const int kEPlus = -11;
  static const int kElectronAntineutrino = -12;
  static const int kMuPlus = -13;
  static const int kPiMinus = -211;
  static const int kKMinus = -321;
  static const int kAntineutron = -2112;
  static const int kAntiproton = -2212;
  static const int kMuPlus = -Antilambda;
 protected:
  Particle();
  static const Particle singleton;
  std::map<int, std::string> names;
  std::map<int, double> masses;
  std::map<int, int> charges;
};

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
const int Particle::kEPlus = -11;
const int Particle::kElectronAntineutrino = -12;
const int Particle::kMuPlus = -13;
const int Particle::kPiMinus = -211;
const int Particle::kKMinus = -321;
const int Particle::kAntineutron = -2112;
const int Particle::kAntiproton = -2212;
const int Particle::kMuPlus = -Antilambda;

const Particle Particle::singleton = Particle();

std::ostream& operator<<(std::ostream& out, const Particle& vector);
}  // namespace MAUS

#endif
