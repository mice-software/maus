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

//#ifndef _UTILS_THREEVECTOR_UTILS_
//#define _UTILS_THREEVECTOR_UTILS_

#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/Rotation.h"

#include "src/common_cpp/DataStructure/ThreeVector.hh"

/**
  Defines a couple of Utilities for managing three vector conversions from
  ROOT to CLHEP and associated bureaucracy - this is intended to keep CLHEP out
  of the data structure (so we can make it portable)
 */

namespace MAUS {
/** Sets rv = rot*rv and returns a reference to rv
 */
inline ThreeVector& operator*=(ThreeVector& rv, const CLHEP::HepRotation& rot) {
    CLHEP::Hep3Vector cv(rv.x(), rv.y(), rv.z());
    cv *= rot;
    rv.set(cv.x(), cv.y(), cv.z());
    return rv;
}

/** Returns a ThreeVector with x, y, z values corresponding to vec */
inline ThreeVector clhep_to_root(CLHEP::Hep3Vector vec) {
    return ThreeVector(vec.x(), vec.y(), vec.z());
}
}

//#endif // _UTILS_THREEVECTOR_UTILS_


