#ifndef PHASESPACEVECTOR_H
#define PHASESPACEVECTOR_H 1

#include <iostream>

#include "Interface/Vector.hh"

namespace MAUS
{

class PhaseSpaceVector : public MAUS::Vector<double>
{
public:
  PhaseSpaceVector();
  PhaseSpaceVector(const PhaseSpaceVector& original_instance);
	/** @brief Create with the given initial coordinates.
	 */
  PhaseSpaceVector(const double t, const double E,
                   const double x, const double Px,
                   const double y, const double Py);
	/** @brief Create with coordinates from an array. Order is t, E, x, Px, y, Py.
	 */
	PhaseSpaceVector(const double* data);
  ~PhaseSpaceVector();

  //accessors
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
  
  //mutators
  inline void set_time(double time)							{(*this)[0] = time;}
  inline void set_t(double time)								{(*this)[0] = time;}
  inline void set_energy(double energy)					{(*this)[1] = energy;}
  inline void set_E(double energy)							{(*this)[1] = energy;}
  inline void set_x(double x)										{(*this)[2] = x;}
  inline void set_x_momentum(double x_momentum)	{(*this)[3] = x_momentum;}
  inline void set_Px(double x_momentum)					{(*this)[3] = x_momentum;}
  inline void set_y(double y)										{(*this)[4] = y;}
  inline void set_y_momentum(double y_momentum)	{(*this)[5] = y_momentum;}
  inline void set_Py(double y_momentum)					{(*this)[5] = y_momentum;}

protected:
  std::ostream&    Print(std::ostream& out) const; 
};

} //namespace MAUS

#endif



