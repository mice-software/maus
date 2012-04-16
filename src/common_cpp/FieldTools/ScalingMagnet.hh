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

#ifndef _SRC_COMMON_CPP_FIELDTOOLS_SCALINGMAGNET_HH_
#define _SRC_COMMON_CPP_FIELDTOOLS_SCALINGMAGNET_HH_

#include <vector>

#include "src/legacy/BeamTools/BTField.hh"

namespace MAUS {

class ScalingMagnet : public BTField {
  public:
    ScalingMagnet();
    ScalingMagnet(double radius, double b_0, double field_index);
    ~ScalingMagnet();

    void SetRadius(double radius);
    double GetRadius();

    void SetB0(double b0);
    double GetB0();

    void SetFieldIndex(double field_index);
    double GetFieldIndex();

    void SetCoefficients(size_t max_index);
    int GetCoefficient(size_t r_index, size_t y_index);

    void GetFieldValue(const double* position, double* field);

    void GetFieldValuePolar(const double* position, double* field);

  private:
    double GetEndField(double phi) {return 1.;}

    double _radius;
    double _b0;
    double _k;
    size_t _max_index;
    std::vector< std::vector<double> > _coefficients;
};
}
#endif

/*
    def __init__(self):
        self._b0 = 0.
        self._k = 0.
        self._phi_max = 0.
        self._radius = 0.
        self._coefficients = numpy.zeros((10, 10))

    def set_coefficient_lookup(self):
        for i in range(10):
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
