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

#include "src/legacy/Interface/Interpolation/TriLinearInterpolator.hh"

TriLinearInterpolator::TriLinearInterpolator(const TriLinearInterpolator& lhs) {
    _coordinates = new ThreeDGrid(*lhs._coordinates);
    _F = new double**[_coordinates->xSize()];
    for (int i = 0; i < _coordinates->xSize(); i++) {
        _F[i] = new double*[_coordinates->ySize()];
        for (int j = 0; j < _coordinates->ySize(); j++) {
            _F[i][j] = new double[_coordinates->zSize()];
            for (int k = 0; k < _coordinates->zSize(); k++)
                _F[i][j][k] = lhs._F[i][j][k];
        }
    }
    _coordinates->Add(this);
}

void TriLinearInterpolator::F(const double Point[3], double Value[1]) const {
    int i, j, k;  // position indices
    _coordinates->LowerBound(Point[0], i, Point[1], j, Point[2], k);
    // bound checking
    if (i + 2 > _coordinates->xSize() ||
        j + 2 > _coordinates->ySize() ||
        k + 2 > _coordinates->zSize()) {
        Value[0]=0;
        return;
    }
    if (i  < 0 || j < 0 || k < 0) {
        Value[0]=0;
        return;
    }
    // interpolation in x
    double dx = (Point[0]-_coordinates->x(i+1))/
                (_coordinates->x(i+2)-_coordinates->x(i+1));
    double f_x[2][2];
    f_x[0][0] = (_F[i+1][j][k] - _F[i][j][k]) * dx + _F[i][j][k];
    f_x[1][0] = (_F[i+1][j+1][k] - _F[i][j+1][k]) * dx + _F[i][j+1][k];
    f_x[0][1] = (_F[i+1][j][k+1] - _F[i][j][k+1]) * dx + _F[i][j][k+1];
    f_x[1][1] = (_F[i+1][j+1][k+1] - _F[i][j+1][k+1]) * dx + _F[i][j+1][k+1];
    // interpolation in y
    double f_xy[2];
    double dy = (Point[1]-_coordinates->y(j+1))/
                (_coordinates->y(j+2) - _coordinates->y(j+1));
    f_xy[0] = (f_x[1][0] - f_x[0][0])*dy + f_x[0][0];
    f_xy[1] = (f_x[1][1] - f_x[0][1])*dy + f_x[0][1];
    // interpolation in z
    Value[0]  = (f_xy[1] - f_xy[0])/
                (_coordinates->z(k+2) - _coordinates->z(k+1))*
                (Point[2] - _coordinates->z(k+1)) + f_xy[0];
}

