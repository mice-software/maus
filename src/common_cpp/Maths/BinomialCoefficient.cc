/* This file is part of MAUS: http:// micewww.pp.rl.ac.uk:8080/projects/maus
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
 * along with MAUS.  If not, see <http:// www.gnu.org/licenses/>.
 */

/* Author: Peter Lane
 */

#include "Maths/BinomialCoefficient.hh"

namespace MAUS {

unsigned int BinomialCoefficient::Choose(
    const unsigned int n, const unsigned int k) {
  if ((n == 0) or (k > n)) {
    return 0;
  } else if (k == 0) {
    return 1;
  }

  return n * Choose(n-1, k-1) / k;
}

}  // namespace MAUS

