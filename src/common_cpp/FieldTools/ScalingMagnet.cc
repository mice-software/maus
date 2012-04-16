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

#include <vector>

#include "gsl/gsl_sf_gamma.h"

#include "src/common_cpp/FieldTools/ScalingMagnet.hh"

namespace MAUS {

ScalingMagnet::ScalingMagnet()
              : _radius(0), _b0(0), _k(0), _max_index(0), _coefficients() {
}

ScalingMagnet::ScalingMagnet(double radius, double b_0, double field_index)
    : _radius(radius), _b0(b_0), _k(field_index), _max_index(0), _coefficients() {
    SetCoefficients(10);
}

ScalingMagnet::~ScalingMagnet() {
}

void ScalingMagnet::SetCoefficients(size_t max_index) {
    _max_index = max_index;
    _coefficients = std::vector< std::vector<double> >
                                    (max_index, std::vector<double>(max_index));
    for (size_t i = 0; i < max_index; ++i) {
        _coefficients[i][0] =
                         pow(_radius, _k-static_cast<double>(i))/gsl_sf_fact(i);
//        std::cerr << _coefficients[i][0] << " ";
        for (size_t k = 0; k < i; ++k) {
            _coefficients[i][0] *= _k-static_cast<double>(k);
        }
        std::cerr << _coefficients[i][0] << " ";
    }
    std::cerr << std::endl << std::endl;
}

int ScalingMagnet::GetCoefficient(size_t r_index, size_t y_index) {
    return _coefficients[r_index][y_index];
    if (r_index < _coefficients.size() &&
        y_index < _coefficients[r_index].size()) {
    }
    return 0;
}


// r, y, phi, time
void ScalingMagnet::GetFieldValuePolar(const double* position, double* field) {
    double r = position[0]/_radius-_radius;
    for (size_t r_index = 0; r_index < _max_index; ++r_index) {
        for (size_t y_index = 0; y_index < _max_index; ++y_index) {
            field[1] += _coefficients[r_index][y_index]*
                        pow(r, r_index)*
                        pow(position[1], y_index);
        }
    }
    field[1] *= _b0;
}

/*       for i in range(10):
            const = self._radius**(self._k-i)/math.factorial(i)
            for index in range(0, i):const = self._
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

