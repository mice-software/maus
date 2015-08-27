/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_THREEVECTOR_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_THREEVECTOR_HH_

#include <iostream>

#include "TVector3.h" // ROOT

#include "src/common_cpp/Utils/VersionNumber.hh"

namespace MAUS {

/**
 * @class ThreeVector provides a thin wrapper for TVector3
 *
 * Additional interfaces to provide backwards compatibility to some
 * CLHEP::Hep3Vector types
 *
 * Element 0 - x
 * Element 1 - y
 * Element 2 - z
 */
class ThreeVector : public TVector3 {
  public:
    /** Fills vector with data from an array of doubles of length 3
      *
      * Caller owns memory pointed to by value
      */
	  explicit ThreeVector(const Double_t* value) : TVector3(value) {}

    /** Fills vector with data from an array of floats of length 3
      *
      * Caller owns memory pointed to by value
      */
	  explicit ThreeVector(const Float_t* value) : TVector3(value) {}

    /** Copy constructor from a ROOT TVector3
     *
     *  Beware! This is enabled for implicit conversion - in order to provide
     *  the desired cross-compatibility with ROOT
     */
	  explicit ThreeVector(const TVector3& value) : TVector3(value) {}

    /** Copy constructor */
	  explicit ThreeVector(const ThreeVector& value)
       : TVector3(value.x(), value.y(), value.z()) {
    }

    /** Fills vector with x, y, z data (for 0, 1, 2 elements respectively) */
	  ThreeVector(double x = 0.0, double y = 0.0, double z = 0.0)
      : TVector3(x, y, z) {}

    /** Assignment operators */
    inline ThreeVector& operator=(const ThreeVector& vec) {
        if (this == &vec)
            return *this;
        SetXYZ(vec.x(), vec.y(), vec.z());
        return *this;
    }

    inline ThreeVector& operator/=(const double& d) {
        (*this) *= (1./d);
        return *this;
    }

    /** Destructor - does nothing */
    virtual ~ThreeVector() {}

    /** Set x */
    inline void setX(double x) {TVector3::SetX(x);}
    /** Set y */
    inline void setY(double y) {TVector3::SetY(y);}
    /** Set z */
    inline void setZ(double z) {TVector3::SetZ(z);}

    /** Get x */
    inline double x() const {return TVector3::x();}
    /** Get y */
    inline double y() const {return TVector3::y();}
    /** Get z */
    inline double z() const {return TVector3::z();}

    /** Return cross product of this x p */
    inline ThreeVector cross(const ThreeVector& p) const {
        return TVector3::Cross(p);
    }

    /** Return the magnitude sqrt(this.this) */
    inline double mag() const {return TVector3::Mag();}

    /** Set x, y and z values of the vector */
    void set(double x, double y, double z) {SetXYZ(x, y, z);}

  private:
    MAUS_VERSIONED_CLASS_DEF(ThreeVector)
};

inline ThreeVector operator/(const ThreeVector& v, const double& d) {
    return ThreeVector(v) /= d;
}

inline bool operator==(const ThreeVector& v1, const ThreeVector& v2) {
    return TVector3(v1) == TVector3(v2);
}

inline std::ostream& operator<<(std::ostream& os, const ThreeVector& v) {
    return os << TVector3(v);
}
}

#endif

