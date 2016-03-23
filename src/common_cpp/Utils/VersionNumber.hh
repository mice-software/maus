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
 *
 */

#include "Rtypes.h"

// Don't use macros / #define! I have to use it here to tie it into the ROOT
// ClassDef macro (issue #1188).
// version number is x.y.z, encoded as a single int as xxxyyyzzz where each of
// xxx is 0 padded
#ifndef MAUS_VERSION_NUMBER
#define MAUS_VERSION_NUMBER_X 2
#define MAUS_VERSION_NUMBER_Y 1
#define MAUS_VERSION_NUMBER_Z 0
#define MAUS_VERSION_NUMBER() \
        (MAUS_VERSION_NUMBER_X*1000+\
        MAUS_VERSION_NUMBER_Y*100+\
        MAUS_VERSION_NUMBER_Z)

// Macro wrapper for ClassDef
#define MAUS_VERSIONED_CLASS_DEF(Class) ClassDef(Class, MAUS_VERSION_NUMBER());

#endif

