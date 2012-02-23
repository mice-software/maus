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
  std::string GetName(int id);
  
  /* @brief returns the mass (MeV/c^2) of the particle referenced by id.
   */
  double GetMass(int id);
  
  /* @brief returns the charge (e) of the particle reference by id.
   */
  int GetCharge(int id);
 protected:
  Particle();
  static const Particle singleton;
  std::map<int, std::string> names;
  std::map<int, double> masses;
  std::map<int, int> charges;
};

const Particle Particle::singleton = Particle();

std::ostream& operator<<(std::ostream& out, const Particle& vector);
}  // namespace MAUS

#endif
