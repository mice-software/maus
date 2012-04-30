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
#include <map>
#include <string>

#include "src/common_cpp/Optics/PhaseSpaceVector.hh"
     
namespace MAUS {

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
  
  static const int kNone;
  static const int kEMinus;
  static const int kElectronNeutrino;
  static const int kMuMinus;
  static const int kMuonNeutrino;
  static const int kPhoton;
  static const int kPi0;
  static const int kPiPlus;
  static const int kKPlus;
  static const int kNeutron;
  static const int kProton;
  static const int kDeuterium;
  static const int kTritium;
  static const int kHelium3;
  static const int kHelium4;
  static const int kKLong;
  static const int kKShort;
  static const int kK0;
  static const int kLambda;
  static const int kEPlus;
  static const int kElectronAntineutrino;
  static const int kMuPlus;
  static const int kMuonAntineutrino;
  static const int kPiMinus;
  static const int kKMinus;
  static const int kAntineutron;
  static const int kAntiproton;
  static const int kAntilambda;
 protected:
  Particle();
  static const Particle kSingleton;
  std::map<int, std::string> names_;
  std::map<int, double> masses_;
  std::map<int, int> charges_;
};

std::ostream& operator<<(std::ostream& out, const Particle& vector);
}  // namespace MAUS

#endif
