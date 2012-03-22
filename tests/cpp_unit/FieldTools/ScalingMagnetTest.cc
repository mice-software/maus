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

#include "gtest/gtest.h"

#include "src/common_cpp/FieldTools/ScalingMagnet.hh"

namespace MAUS {

TEST(ScalingMagnetTest, MidplaneFieldTest) {
    double k = 1.92;
    double r0 = 2.0;
    double b0 = 0.5;
    ScalingMagnet my_scale(r0, b0, k);
    my_scale.SetCoefficients(10);
    double pos[] = {0., 0., 0., 0.};
//    std::cerr << "Coefficients" << std::endl;
    for (size_t i = 0; i < 10; ++i) {
        for (size_t j = 0; j < 1; ++j) {
//            std::cerr << my_scale.GetCoefficient(i, j) << " ";
        }
    }
//    std::cerr << std::endl;

    for (pos[0] = 0.5*r0; pos[0] < 1.5*r0+1e-9; pos[0] += 0.05*r0) {
        double field[] = {0., 0., 0., 0., 0., 0.};
        my_scale.GetFieldValuePolar(pos, field);
        std::cerr << pos[0] << " " << field[1] << " " << b0*pow(pos[0]/r0, k) << std::endl;
    }
}

}

