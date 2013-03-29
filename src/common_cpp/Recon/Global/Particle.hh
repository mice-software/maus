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

#include "src/common_cpp/DataStructure/Global/ReconEnums.hh"
#include "src/common_cpp/Optics/PhaseSpaceVector.hh"

namespace MAUS {
namespace recon {
namespace global {

class Particle {
 public:
  static const Particle & GetInstance();
  ~Particle();

  /* @brief returns the unique name of the particle referenced by id.
   */
  std::string GetName(const MAUS::DataStructure::Global::PID id) const;

  /* @brief returns the mass (MeV/c^2) of the particle referenced by id.
   */
  double GetMass(const MAUS::DataStructure::Global::PID id) const;

  /* @brief returns the charge (e) of the particle reference by id.
   */
  int GetCharge(const MAUS::DataStructure::Global::PID id) const;
 protected:
  Particle();
  static const Particle kSingleton;
  std::map<MAUS::DataStructure::Global::PID, std::string> names_;
  std::map<MAUS::DataStructure::Global::PID, double> masses_;
  std::map<MAUS::DataStructure::Global::PID, int> charges_;
 private:
  void AddParticleDefinition(MAUS::DataStructure::Global::PID pid,
                             std::string name,
                             double mass,
                             int charge);
};

std::ostream& operator<<(std::ostream& out, const Particle& vector);

}  // namespace global
}  // namespace recon
}  // namespace MAUS

#endif
