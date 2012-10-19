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
namespace reconstruction {
namespace global {

class Particle {
 public:
  enum ID {kNone, kEMinus=11, kElectronNeutrino, kMuMinus, kMuonNeutrino,
           kPhoton=22, kPi0=111, kPiPlus=211, kKPlus=321, kNeutron=2112,
           kProton=2212, kDeuterium=1000010020, kTritium=1000010030,
           kHelium3=1000020030, kHelium4=1000020040, kKLong=130, kKShort=310,
           kK0=311, kLambda=3122, kMuonAntineutrino=-14, kMuPlus,
           kElectronAntineutrino, kEPlus, kPiMinus=-211, kKMinus=-321,
           kAntineutron=-2112, kAntiproton=-2212, kAntilambda=-3122};

   static Particle const * GetInstance();
  ~Particle();

  /* @brief returns the unique name of the particle referenced by id.
   */
  std::string GetName(const ID id) const;

  /* @brief returns the mass (MeV/c^2) of the particle referenced by id.
   */
  double GetMass(const ID id) const;

  /* @brief returns the charge (e) of the particle reference by id.
   */
  int GetCharge(const ID id) const;
 protected:
  Particle();
  static const Particle kSingleton;
  std::map<ID, std::string> names_;
  std::map<ID, double> masses_;
  std::map<ID, int> charges_;
};

std::ostream& operator<<(std::ostream& out, const Particle& vector);

}  // namespace global
}  // namespace reconstruction
}  // namespace MAUS

#endif
