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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_MCEVENTARRAY_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_MCEVENTARRAY_HH_

#include "Rtypes.h" // ROOT

#include <vector>

namespace MAUS {

class MCEvent;

class MCEventArray {
 public:
  MCEventArray();

  MCEventArray(const MCEventArray& md);

  MCEventArray& operator=(const MCEventArray& md);

  ~MCEventArray();

  void push_back(MCEvent* ev);

  size_t size() const;

  MCEvent*& operator[](size_t i);

  MCEvent* operator[](size_t i) const;

 private:

  std::vector<MCEvent*> _vec;
  ClassDef(MCEventArray, 1)
};

}

#endif
