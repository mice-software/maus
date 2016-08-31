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

#include "src/legacy/Interface/Interpolation/Interpolator3dGridTo3d.hh"
#include "Utils/Exception.hh"

Interpolator3dGridTo3d::Interpolator3dGridTo3d
                                           (const Interpolator3dGridTo3d& rhs) {
    _coordinates = new ThreeDGrid(*rhs._coordinates);
    for (int i = 0; i < 3; i++)
        _interpolator[i] = rhs._interpolator[i]->Clone();
}

void Interpolator3dGridTo3d::F(const double Point[3], double Value[3]) const {
    if (Point[0] > _coordinates->MaxX() || Point[0] < _coordinates->MinX() ||
        Point[1] > _coordinates->MaxY() || Point[1] < _coordinates->MinY() ||
        Point[2] > _coordinates->MaxZ() || Point[2] < _coordinates->MinZ() ) {
        Value[0] = 0;
        Value[1] = 0;
        Value[2] = 0;
        return;
    }

    _interpolator[0]->F(Point, &Value[0]);
    _interpolator[1]->F(Point, &Value[1]);
    _interpolator[2]->F(Point, &Value[2]);
}

void Interpolator3dGridTo3d::Set(ThreeDGrid* grid,
                                 double *** Bx, double *** By, double *** Bz,
                                 interpolationAlgorithm algo) {
    if (_coordinates != NULL)
      _coordinates->Remove(this);
    grid->Add(this);
    _coordinates = grid;

    for (int i = 0; i < 3; i++)
        if (_interpolator[i] != NULL)
        delete _interpolator[i];

    switch (algo) {
        case triLinear:
            _interpolator[0] = new TriLinearInterpolator(grid, Bx);
            _interpolator[1] = new TriLinearInterpolator(grid, By);
            _interpolator[2] = new TriLinearInterpolator(grid, Bz);
            break;
        default:
            throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable,
                   "Did not recognise interpolation algorithm",
                   "Interpolator3dGridTo3d::Set")
      );
    }
}


