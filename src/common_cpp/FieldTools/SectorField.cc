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

#include "src/common_cpp/FieldTools/SectorField.hh"

namespace MAUS {

SectorField::SectorField() : BTField() {}

SectorField::SectorField(double bbMinR, double bbMinY, double bbMinPhi,
            double bbMaxR, double bbMaxY, double bbMaxPhi) {
    SetBoundingBoxMinPolar(bbMinR, bbMinY, bbMinPhi);
    SetBoundingBoxMaxPolar(bbMaxR, bbMaxY, bbMaxPhi);
}

virtual void GetFieldValuePolar
                         (const double* point_polar, double* field_polar) {}

virtual void GetFieldValue
                       (const double* point_cartes, double* field_cartes) {}

virtual void ConvertToPolar(double* position) {
    double x = ::sqrt(position[0]*position[0]+position[2]*position[2]);
    double z = ::atan2(point[2], point[0]);
    position[0] = x;
    position[2] = z;
}

virtual void ConvertToCartesian(double* position) {
    double x = position[0]*::cos(position[2]);  // r cos(phi)
    double z = -position[0]*::sin(position[2]);  // r sin(phi)
    position[0] = x;
    position[2] = z;
}

virtual void ConvertToCartesian(const double* position, double* value) {
    double x = value[0]*::cos(position[2]);  // r cos(phi)
    double z = -value[0]*::sin(position[2]);  // r sin(phi)
    position[0] = x;
    position[2] = z;
}


void SetPolarBoundingBoxMin(double bbMinR, double bbMinY, double bbMinPhi) {
}

void SetPolarBoundingBoxMax(double bbMaxR, double bbMaxY, double bbMaxPhi) {
}

}


