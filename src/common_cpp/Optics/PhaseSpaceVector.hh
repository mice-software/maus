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

#ifndef COMMON_CPP_OPTICS_PHASE_SPACE_VECTOR_HH
#define COMMON_CPP_OPTICS_PHASE_SPACE_VECTOR_HH

#include <iostream>

#include "TLorentzVector.h"

#include "DataStructure/ThreeVector.hh"
#include "DataStructure/VirtualHit.hh"
#include "Maths/Vector.hh"

namespace MAUS {

class PhaseSpaceVector : public Vector<double> {
 public:
  class PhaseSpaceType;

  /* @brief	Construct with all elements initialized to zero and phase space type
   * temporal.
   */
  PhaseSpaceVector();

  /* @brief  Base class copy constructor. This only copies the first 6 elements
   *         of the Vector<double> object.
   */
  explicit PhaseSpaceVector(const Vector<double>& original_instance);

  /* @brief  Copy constructor.
   */
  PhaseSpaceVector(const PhaseSpaceVector& original_instance);

  /* @brief Create with coordinates from an array. Order is t, E, x, Px, y, Py
   * if type = aseSpaceVector::kTemporalPhaseSpace or x, Px, y, Py, z, Pz if
   * type = haseSpaceVector::kPositionalPhaseSpace.
   */
  explicit PhaseSpaceVector(double const * const array);

  /* @brief	Create with the given initial coordinates.
   */
  PhaseSpaceVector(const double t, const double E,
                   const double x, const double px,
                   const double y, const double py);

  PhaseSpaceVector(const MAUS::VirtualHit & hit);

  PhaseSpaceVector(const double time,
                   const double energy,
                   const MAUS::ThreeVector position,
                   const MAUS::ThreeVector momentum);

  PhaseSpaceVector(const TLorentzVector position,
                   const TLorentzVector momentum);

  ~PhaseSpaceVector();

  // *************************
  //  Assignment Operators
  // *************************
  PhaseSpaceVector& operator =(
    const PhaseSpaceVector&                  rhs);
  PhaseSpaceVector& operator+=(
    const PhaseSpaceVector&                  rhs);
  PhaseSpaceVector& operator-=(
    const PhaseSpaceVector&                  rhs);
  PhaseSpaceVector& operator*=(
    const PhaseSpaceVector&                  rhs);
  PhaseSpaceVector& operator/=(
    const PhaseSpaceVector&                  rhs);
  PhaseSpaceVector& operator+=(const double& rhs);
  PhaseSpaceVector& operator-=(const double& rhs);
  PhaseSpaceVector& operator*=(const double& rhs);
  PhaseSpaceVector& operator/=(const double& rhs);

  // *************************
  //  Algebraic Operators
  // *************************
  const PhaseSpaceVector operator+(
    const PhaseSpaceVector&                      rhs) const;
  const PhaseSpaceVector operator-(
    const PhaseSpaceVector&                      rhs) const;
  const PhaseSpaceVector operator*(
    const PhaseSpaceVector&                      rhs) const;
  const PhaseSpaceVector operator/(
    const PhaseSpaceVector&                      rhs) const;
  const PhaseSpaceVector operator+(const double& rhs) const;
  const PhaseSpaceVector operator-(const double& rhs) const;
  const PhaseSpaceVector operator*(const double& rhs) const;
  const PhaseSpaceVector operator/(const double& rhs) const;

  // accessors
  double time()       const {return (*this)[0];}
  double t()          const {return time();}
  double energy()     const {return (*this)[1];}
  double E()          const {return energy();}
  double x()          const {return (*this)[2];}
  double x_momentum() const {return (*this)[3];}
  double Px()         const {return x_momentum();}
  double y()          const {return (*this)[4];}
  double y_momentum() const {return (*this)[5];}
  double Py()         const {return y_momentum();}

  // mutators
  void set_time(double time)              {(*this)[0] = time;}
  void set_t(double time)                 {set_time(time);}
  void set_energy(double energy)          {(*this)[1] = energy;}
  void set_E(double energy)               {set_energy(energy);}
  void set_x(double x)                    {(*this)[2] = x;}
  void set_x_momentum(double x_momentum)  {(*this)[3] = x_momentum;}
  void set_Px(double x_momentum)          {set_x_momentum(x_momentum);}
  void set_y(double y)                    {(*this)[4] = y;}
  void set_y_momentum(double y_momentum)  {(*this)[5] = y_momentum;}
  void set_Py(double y_momentum)          {set_y_momentum(y_momentum);}
};

std::ostream& operator<<(std::ostream& out, const PhaseSpaceVector& vector);
}  // namespace MAUS

#endif
