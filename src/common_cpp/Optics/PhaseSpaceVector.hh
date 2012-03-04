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

#include "Maths/Vector.hh"

namespace MAUS {

class PhaseSpaceVector : public Vector<double> {
 public:
  /* @brief	Construct with all elements initialized to zero.
   */
  PhaseSpaceVector();

  /* @brief  Base class copy constructor. This only copies the first 6 elements
   *         of the Vector<double> object.
   */
  explicit PhaseSpaceVector(const Vector<double>& original_instance);

  /* @brief  Copy constructor.
   */
  PhaseSpaceVector(const PhaseSpaceVector& original_instance);

  /* @brief Create with coordinates from an array. Order is t, E, x, Px, y, Py.
   */
  explicit PhaseSpaceVector(double const * const array);

  /* @brief	Create with the given initial coordinates.
   */
  PhaseSpaceVector(const double t, const double E,
                   const double x, const double Px,
                   const double y, const double Py);

  ~PhaseSpaceVector();

  // accessors
  inline double time()       const {return (*this)[0];}
  inline double t()          const {return (*this)[0];}
  inline double energy()     const {return (*this)[1];}
  inline double E()          const {return (*this)[1];}
  inline double x()          const {return (*this)[2];}
  inline double x_momentum() const {return (*this)[3];}
  inline double Px()         const {return (*this)[3];}
  inline double y()          const {return (*this)[4];}
  inline double y_momentum() const {return (*this)[5];}
  inline double Py()         const {return (*this)[5];}

  // mutators
  inline void set_time(double time)              {(*this)[0] = time;}
  inline void set_t(double time)                {(*this)[0] = time;}
  inline void set_energy(double energy)          {(*this)[1] = energy;}
  inline void set_E(double energy)              {(*this)[1] = energy;}
  inline void set_x(double x)                    {(*this)[2] = x;}
  inline void set_x_momentum(double x_momentum)  {(*this)[3] = x_momentum;}
  inline void set_Px(double x_momentum)          {(*this)[3] = x_momentum;}
  inline void set_y(double y)                    {(*this)[4] = y;}
  inline void set_y_momentum(double y_momentum)  {(*this)[5] = y_momentum;}
  inline void set_Py(double y_momentum)          {(*this)[5] = y_momentum;}
};

std::ostream& operator<<(std::ostream& out, const PhaseSpaceVector& vector);
}  // namespace MAUS

#endif
