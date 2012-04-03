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

#ifndef COMMON_CPP_RECONSTRUCTION_PARTICLE_TRACK_HH
#define COMMON_CPP_RECONSTRUCTION_PARTICLE_TRACK_HH

#include <iostream>

#include "src/common_cpp/Optics/PhaseSpaceVector.hh"
#include "src/common_cpp/Simulation/MAUSPrimaryGeneratorAction.hh"
     
namespace MAUS {

/* @class ParticleTrack a phase space vector with redundant t/E and z/Pz
 * coordinates as well as an ID that links the track to the detector that
 * measured it.
 *
 * The redundant sets of coordinates can be filled in by explicitly using the
 * FillInCoordinates() function or by setting all of the coordinates at once
 * using the array or parameterised constructors (which implicitly call
 * FillInCoordinates()). If t < 0 it fills in t and E from z, Pz, and the given
 * mass parameter. If t >= 0 and z < 0, it fills in z and Pz from t, E, and
 * the mass.
 */
class ParticleTrack : public MAUS::PhaseSpaceVector {
 public:
  /* @brief	Construct with all elements initialized to zero.
   */
  ParticleTrack();

  /* @brief  Copy constructor.
   */
  ParticleTrack(const ParticleTrack& original_instance);

  /* @brief Create with coordinates from an array.
     Order is t, E, x, Px, y, Py, z, Pz.
   */
  explicit ParticleTrack(double const * const array);

  /* @brief	Create with the given initial coordinates.
   */
  ParticleTrack(const double t, const double E,
                   const double x, const double Px,
                   const double y, const double Py,
                   const double z, const double Pz
                   const unsigned int detector_id);

  ~ParticleTrack();

  // *************************
  //       Accessors
  // *************************

  inline void set_detector_id(unsigned int id) {detector_id_ = id;}
  inline void detector_id() const {return detector_id_;}
 
  inline double z()          const {return z_;}
  inline void set_z(const double z) {z_ = z;}

  inline double z_momentum() const {return z_momentum_;}
  inline void set_z_momentum(const double z_momentum) {z_momentum_ = z_momentum;}
  inline double Pz()         const {return z_momentum_;}
  inline void set_Pz(const double Pz) {z_momentum_ = Pz;}
 
  void FillInCoordinates(const double mass);  
 protected:
  double z_;
  double z_momentum_;
};

std::ostream& operator<<(std::ostream& out, const ParticleTrack& vector);
}  // namespace MAUS

#endif
