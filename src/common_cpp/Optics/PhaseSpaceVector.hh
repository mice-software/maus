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
  class PhaseSpaceType;

  /* @brief	Construct with all elements initialized to zero and phase space type
   * temporal.
   */
  PhaseSpaceVector();

  /* @brief	Construct with all elements initialized to zero and given phase
   * space type.
   */
  PhaseSpaceVector(const PhaseSpaceType type);

  /* @brief  Base class copy constructor. This only copies the first 6 elements
   *         of the Vector<double> object.
   */
  PhaseSpaceVector(const Vector<double>& original_instance,
                   const PhaseSpaceType type=PhaseSpaceType::kTemporal);

  /* @brief  Copy constructor.
   */
  PhaseSpaceVector(const PhaseSpaceVector& original_instance);

  PhaseSpaceVector(const PhaseSpaceVector& original_instance,
                   const double mass,
                   const PhaseSpaceType);

  /* @brief Create with coordinates from an array. Order is t, E, x, Px, y, Py
   * if type = aseSpaceVector::kTemporalPhaseSpace or x, Px, y, Py, z, Pz if
   * type = haseSpaceVector::kPositionalPhaseSpace.
   */
  PhaseSpaceVector(double const * const array,
                   const PhaseSpaceType type=PhaseSpaceType::kTemporal);

  /* @brief	Create with the given initial coordinates.
   */
  PhaseSpaceVector(const double x1, const double p1,
                   const double x2, const double p2,
                   const double x3, const double p3,
                   const PhaseSpaceType type=PhaseSpaceType::kTemporal);

  ~PhaseSpaceVector();

  PhaseSpaceVector & operator=(const PhaseSpaceVector & rhs);

  // accessors
  const PhaseSpaceType & type() const {return type_;}
  double time()       const;
  double t()          const {return time();}
  double energy()     const;
  double E()          const {return energy();}
  double x()          const;
  double x_momentum() const;
  double Px()         const {return x_momentum();}
  double y()          const;
  double y_momentum() const;
  double Py()         const {return y_momentum();}
  double z()          const;
  double z_momentum() const;
  double Pz()         const {return z_momentum();}

  // mutators
  void set_time(double time);
  void set_t(double time)         {set_time(time);}
  void set_energy(double energy);
  void set_E(double energy)       {set_energy(energy);}
  void set_x(double x);
  void set_x_momentum(double x_momentum);
  void set_Px(double x_momentum)  {set_x_momentum(x_momentum);}
  void set_y(double y);
  void set_y_momentum(double y_momentum);
  void set_Py(double y_momentum)  {set_y_momentum(y_momentum);}
  void set_z(double z);
  void set_z_momentum(double z_momentum);
  void set_Pz(double z_momentum)  {set_z_momentum(z_momentum);}

  /* @class Identify with strict typing what type of phase space will be used.
   */
  class PhaseSpaceType {
   public:              
    const size_t id() const {             
      return id_;
    }

    const bool operator==(const PhaseSpaceType & rhs) const {
      return id_ == rhs.id_;
    }

    const bool operator!=(const PhaseSpaceType & rhs) const {
      return id_ != rhs.id_;
    }

    static const PhaseSpaceType kTemporal;
    static const PhaseSpaceType kPositional;
   private:
    PhaseSpaceType() : id_(0) {}
    PhaseSpaceType(size_t id) : id_(id) {}
    size_t id_;
  };
 protected:
  const PhaseSpaceType type_;
};

std::ostream& operator<<(std::ostream& out, const PhaseSpaceVector& vector);
}  // namespace MAUS

#endif
