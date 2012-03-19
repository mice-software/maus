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

#include "src/common_cpp/FieldTools/ScalingMagnet.hh"

namespace MAUS {

ScalingMagnet::ScalingMagnet() : _radius(0), _b0(0), _k(0), _coefficients() {
}

ScalingMagnet::ScalingMagnet(double radius, double b_0, double field_index)
    : _radius(radius), _b0(b_0), _k(field_index), _coefficients(10, 10) {
    SetCoefficients(10, 10);
}

ScalingMagnet::SetCoefficients(int max_index) {
    _coefficients = MMatrix<double>(max_index, max_index);
    for (size_t i = 0; i < max_index; ++i) {
        for (size_t j = 0; j < max_index; ++j) {
            
        }
    }
}



/*       for i in range(10):
            const = self._radius**(self._k-i)/math.factorial(i)
            for index in range(0, i):
                const *= (self._k-index)
            self._coefficients[i, 0] = const

    def get_field(self, r, phi, y):
        return (0., self.get_by_field(r, y)*self.get_end_field(phi)*self._b0, 0.)

    def get_by_field(self, r, y):
        by = 0.
        for i in range(10):
            for j in range(10):
                by += self._coefficients[i, j]*(r-self._radius)**i*y**j
        return by
*/
}

