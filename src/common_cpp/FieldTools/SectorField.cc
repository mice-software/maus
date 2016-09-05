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

#include <math.h>

#include <vector>
#include <algorithm>

#include "Utils/Exception.hh"

#include "src/common_cpp/FieldTools/SectorField.hh"

namespace MAUS {

SectorField::SectorField() : BTField(), _polarBBMin(3, 0),  _polarBBMax(3, 0) {
    _polarBBMin[1] = bbMin[1];
    _polarBBMax[1] = bbMax[1];
    _polarBBMax[0] = bbMax[0];
    _polarBBMin[2] = -2.*M_PI;
    _polarBBMax[2] = 2.*M_PI;
}

SectorField::~SectorField() {}

SectorField::SectorField(double bbMinR, double bbMinY, double bbMinPhi,
            double bbMaxR, double bbMaxY, double bbMaxPhi) {
    SetPolarBoundingBox(bbMinR, bbMinY, bbMinPhi, bbMaxR, bbMaxY, bbMaxPhi);
}

void SectorField::ConvertToPolar(double* position) {
    double x = ::sqrt(position[0]*position[0]+position[2]*position[2]);
    double z = ::atan2(position[2], position[0]);
    position[0] = x;
    position[2] = z;
}

void SectorField::ConvertToPolar(const double* position, double* value) {
    double x = +value[0]*::cos(position[2])
               +value[2]*::sin(position[2]);
    double z = +value[2]*::cos(position[2])
               -value[0]*::sin(position[2]);
    value[0] = x;
    value[2] = z;
}

void SectorField::ConvertToCartesian(double* position) {
    double x = position[0]*::cos(position[2]);  // r cos(phi)
    double z = position[0]*::sin(position[2]);  // r sin(phi)
    position[0] = x;
    position[2] = z;
}

void SectorField::ConvertToCartesian(const double* position, double* value) {
    double x = +value[0]*::cos(position[2])
               -value[2]*::sin(position[2]);
    double z = +value[2]*::cos(position[2])
               +value[0]*::sin(position[2]);
    value[0] = x;
    value[2] = z;
}

void SectorField::SetPolarBoundingBox
                        (double bbMinR, double bbMinY, double bbMinPhi,
                         double bbMaxR, double bbMaxY, double bbMaxPhi) {
    if (bbMinR > bbMaxR) {
        throw(Exceptions::Exception(Exceptions::recoverable,
               "Bounding box minimum radius was greater than maximum radius",
               "SectorField::SetPolarBoundingBox"));
    }
    if (bbMinR < 0.) {
        throw(Exceptions::Exception(Exceptions::recoverable,
               "Bounding box radius must be positive",
               "SectorField::SetPolarBoundingBox"));
    }
    if (bbMinY > bbMaxY) {
        throw(Exceptions::Exception(Exceptions::recoverable,
               "Bounding box minimum y was greater than maximum y",
               "SectorField::SetPolarBoundingBox"));
    }
    if (bbMinY > bbMaxY) {
        throw(Exceptions::Exception(Exceptions::recoverable,
               "Bounding box minimum angle was greater than maximum angle",
               "SectorField::SetPolarBoundingBox"));
    }
    if (bbMinPhi < -2.*M_PI || bbMinPhi > 2.*M_PI ||
        bbMaxPhi < -2.*M_PI || bbMaxPhi > 2.*M_PI) {
        throw(Exceptions::Exception(Exceptions::recoverable,
               "Bounding box angles must be in range -2*M_PI < phi < 2*M_PI",
               "SectorField::SetPolarBoundingBox"));
    }

    _polarBBMin[0] = bbMinR;
    _polarBBMin[1] = bbMinY;
    _polarBBMin[2] = bbMinPhi;

    _polarBBMax[0] = bbMaxR;
    _polarBBMax[1] = bbMaxY;
    _polarBBMax[2] = bbMaxPhi;

    // bounding box from corner coordinates
    std::vector< std::vector<double> > corner_coords(
                                GetCorners(bbMinR, bbMinPhi, bbMaxR, bbMaxPhi));
    BTField::bbMin[0] =
            *std::min_element(corner_coords[0].begin(), corner_coords[0].end());
    BTField::bbMax[0] =
            *std::max_element(corner_coords[0].begin(), corner_coords[0].end());
    BTField::bbMin[1] = bbMinY;
    BTField::bbMax[1] = bbMaxY;
    BTField::bbMin[2] =
            *std::min_element(corner_coords[1].begin(), corner_coords[1].end());
    BTField::bbMax[2] =
            *std::max_element(corner_coords[1].begin(), corner_coords[1].end());

    // if the magnet crosses an axis, then the corners are no longer at the max
    // extent
    if ( (bbMaxPhi > 0.5*M_PI && bbMinPhi < 0.5*M_PI) ||
         (bbMaxPhi > -1.5*M_PI && bbMinPhi < -1.5*M_PI) ) {
        BTField::bbMax[2] = bbMaxR;
    }
    if ((bbMaxPhi > M_PI && bbMinPhi < M_PI) ||
        (bbMaxPhi > -M_PI && bbMinPhi < -M_PI)) {
        BTField::bbMin[0] = -bbMaxR;
    }
    if ((bbMaxPhi > 1.5*M_PI && bbMinPhi < 1.5*M_PI) ||
        (bbMaxPhi > -0.5*M_PI && bbMinPhi < -0.5*M_PI)) {
        BTField::bbMin[2] = -bbMaxR;
    }
    if ((bbMaxPhi > 0.*M_PI && bbMinPhi < 0.*M_PI)) {
        BTField::bbMax[0] = bbMaxR;
    }
}

std::vector< std::vector<double> > SectorField::GetCorners
              (double bbMinR, double bbMinPhi, double bbMaxR, double bbMaxPhi) {
    std::vector< std::vector<double> > corner_coords(2);
    corner_coords[0] = std::vector<double>(4);
    corner_coords[1] = std::vector<double>(4);
    // corners in polar coordinates
    double corner_0[3] = {bbMinR, 0., bbMinPhi};
    double corner_1[3] = {bbMinR, 0., bbMaxPhi};
    double corner_2[3] = {bbMaxR, 0., bbMaxPhi};
    double corner_3[3] = {bbMaxR, 0., bbMinPhi};
    ConvertToCartesian(corner_0);
    ConvertToCartesian(corner_1);
    ConvertToCartesian(corner_2);
    ConvertToCartesian(corner_3);
    // corners in rectangular coordinates (ignore y)
    corner_coords[0][0] = corner_0[0];
    corner_coords[0][1] = corner_1[0];
    corner_coords[0][2] = corner_2[0];
    corner_coords[0][3] = corner_3[0];
    corner_coords[1][0] = corner_0[2];
    corner_coords[1][1] = corner_1[2];
    corner_coords[1][2] = corner_2[2];
    corner_coords[1][3] = corner_3[2];
    return corner_coords;
}


std::vector<double> SectorField::GetPolarBoundingBoxMin() const {
    return _polarBBMin;
}

std::vector<double> SectorField::GetPolarBoundingBoxMax() const {
    return _polarBBMax;
}
}


