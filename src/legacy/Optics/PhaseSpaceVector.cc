#include "Interface/Squeak.hh"
#include "Interface/Vector.h"

#include "PhaseSpaceVector.hh"

namespace MAUS
{

PhaseSpaceVector::PhaseSpaceVector()
  : Vector<double>()
{ }

PhaseSpaceVector::PhaseSpaceVector(const PhaseSpaceVector& original_instance)
	: Vector<double>(original_instance)
{ }

PhaseSpaceVector::PhaseSpaceVector(const double t, const double E,
                                   const double x, const double Px,
                                   const double y, const double Py)
  : Vector<double>()
{
  const double data[6] = {
    t, E, x, Px, y, Py
  };
  build_vector(6, data);
}

PhaseSpaceVector::PhaseSpaceVector(const double* data)
	:Vector<double>(6, data)
{ }

std::ostream& PhaseSpaceVector::Print(std::ostream& out) const 
{
	out << "t: " << (*this)[0] << "E: "   << (*this)[1]
      << "x: " << (*this)[2] << "Px: "  << (*this)[3]
      << "y: " << (*this)[4] << "Py: "  << (*this)[5];
	return out;
}

std::ostream& operator<<(std::ostream& out, const PhaseSpaceVector& ps) 
{
  return ps.Print(out);
}

} //namespace MAUS