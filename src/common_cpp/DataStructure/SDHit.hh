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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_SDHIT_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_SDHIT_HH_

#include <vector>

#include "Rtypes.h" // ROOT

namespace MAUS {

class SDHit {
 public:
  SDHit() {}

  SDHit(const SDHit& md) {}
       
  SDHit& operator=(const SDHit& md) {return *this;}
  
  virtual ~SDHit() {}

 private:


  ClassDef(SDHit, 1)
};

}

#endif

