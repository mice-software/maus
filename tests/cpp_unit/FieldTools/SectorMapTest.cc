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

#include "gtest/gtest.h"

#include "src/common_cpp/FieldTools/SectorMap.hh"

namespace MAUS {

TEST(SectorMapTest, TestReadToscaMapNoSymmetry) {
    double _units[6] = {10., 10., 10., 1.e-4, 1.e-4, 1.e-4};
    std::vector<double> units(&_units[0], &_units[6]);
    std::string map_file = "tests/cpp_unit/FieldTools/test_sector_map.table";

    ASSERT_THROW(SectorMagneticFieldMapIO::ReadMap
                               (map_file, "Nonsense", units, "Dipole"), Squeal);
    ASSERT_THROW(SectorMagneticFieldMapIO::ReadMap
                         ("Nonsense", "tosca_sector", units, "Dipole"), Squeal);
    ASSERT_THROW(SectorMagneticFieldMapIO::ReadMap
                         (map_file, "tosca_sector", units, "nonsense"), Squeal);


    Interpolator3dGridTo3d* interpolator_none = SectorMagneticFieldMapIO::
                               ReadMap(map_file, "tosca_sector", units, "None");
    ThreeDGrid* grid_none = interpolator_none->GetGrid();
    EXPECT_NEAR(grid_none->MinX(), 2100., 1.);
    EXPECT_NEAR(grid_none->MaxX(), 2120., 1.);
    EXPECT_NEAR(grid_none->MinY(), 0., 1.);
    EXPECT_NEAR(grid_none->MaxY(), 10., 1.);
    EXPECT_NEAR(grid_none->MinZ()/M_PI*180., -22.5, 0.1);
    EXPECT_NEAR(grid_none->MaxZ()/M_PI*180., 22.5, 0.1);

    double pos[3] = {2110., 1.e-9, 0.};
    double field_1[3] = {0., 0., 0.};
    double field_2[3] = {0., 0., 0.};
    interpolator_none->F(pos, field_1);
    EXPECT_NEAR(field_1[0], 0., 1e-9);
    EXPECT_NEAR(field_1[1], 0.53468070737600004, 1e-5);
    EXPECT_NEAR(field_1[2], 0., 1e-9);
    pos[2] = +0.1;
    interpolator_none->F(pos, field_1);
    pos[2] = -0.1;
    interpolator_none->F(pos, field_2);
    EXPECT_NEAR(field_1[0], -field_2[0], 1e-9);
    EXPECT_NEAR(field_1[1], field_2[1], 1e-4);
    EXPECT_NEAR(field_1[2], -field_2[2], 1e-9);
}

TEST(SectorMapTest, TestReadToscaMapDipoleSymmetry) {
    double _units[6] = {10., 10., 10., 1.e-4, 1.e-4, 1.e-4};
    std::vector<double> units(&_units[0], &_units[6]);
    std::string map_file = "tests/cpp_unit/FieldTools/test_sector_map.table";

    Interpolator3dGridTo3d* interpolator_dipole = SectorMagneticFieldMapIO::
                             ReadMap(map_file, "tosca_sector", units, "Dipole");
    ThreeDGrid* grid_dip = interpolator_dipole->GetGrid();
    EXPECT_NEAR(grid_dip->MinX(), 2100., 1.);
    EXPECT_NEAR(grid_dip->MaxX(), 2120., 1.);
    EXPECT_NEAR(grid_dip->MinY(), -10., 1.);
    EXPECT_NEAR(grid_dip->MaxY(), 10., 1.);
    EXPECT_NEAR(grid_dip->MinZ()/M_PI*180., -22.5, 0.1);
    EXPECT_NEAR(grid_dip->MaxZ()/M_PI*180., 22.5, 0.1);

    double pos[3] = {2110., 0., 0.};
    double field_1[3] = {0., 0., 0.};
    double field_2[3] = {0., 0., 0.};
    interpolator_dipole->F(pos, field_1);
    EXPECT_NEAR(field_1[0], 0., 1e-9);
    EXPECT_NEAR(field_1[1], 0.53468070737600004, 1e-4);
    EXPECT_NEAR(field_1[2], 0., 1e-9);
    pos[2] = +0.1;
    interpolator_dipole->F(pos, field_1);
    pos[2] = -0.1;
    interpolator_dipole->F(pos, field_2);
    EXPECT_NEAR(field_1[0], -field_2[0], 1e-9);
    EXPECT_NEAR(field_1[1], field_2[1], 1e-4);
    EXPECT_NEAR(field_1[2], -field_2[2], 1e-9);

    pos[2] = 0.;

    pos[1] = +0.1;
    interpolator_dipole->F(pos, field_1);
    pos[1] = -0.1;
    interpolator_dipole->F(pos, field_2);
    EXPECT_NEAR(field_1[0], -field_2[0], 1e-9);
    EXPECT_NEAR(field_1[1], field_2[1], 1e-4);
    EXPECT_NEAR(field_1[2], -field_2[2], 1e-9);
}

TEST(SectorMapTest, TestGetFieldValuePolar) {
    double _units[6] = {10., 10., 10., 1.e-4, 1.e-4, 1.e-4};
    std::vector<double> units(&_units[0], &_units[6]);
    std::string map_file = "tests/cpp_unit/FieldTools/test_sector_map.table";
    SectorMagneticFieldMap map(map_file, "tosca_sector", units, "Dipole");

    double b0 = 0.727;
    double r0 = 2350.;
    double k0 = 1.92;
    
    for (double r=2100.; r<2120.; r += 5.) {
        double point[4] = {r, 0., 0., 0.};
        double field[6] = {0., 0., 0., 0., 0., 0.};
        map.GetFieldValuePolar(point, field);
        EXPECT_NEAR(field[0], 0., 1e-9) << " at radius " << r;
        EXPECT_NEAR(field[1], b0*::pow(double(r/r0), double(k0)), 1e-3)
                                                          << " at radius " << r;
        EXPECT_NEAR(field[2], 0., 1e-9) << " at radius " << r;
    }

    double point[4] = {2110., -20./180.*M_PI, 5., 0.};
    double field[6] = {0., 0., 0., 0., 0., 0.};
    map.GetFieldValuePolar(point, field);
    EXPECT_NEAR(field[0], 0., 1e-9);
    EXPECT_GT(fabs(field[1]), 0.);
    EXPECT_NEAR(field[2], 0., 1e-9);
}

TEST(SectorMapTest, TestGetFieldValueCartesian) {
    double _units[6] = {10., 10., 10., 1.e-4, 1.e-4, 1.e-4};
    std::vector<double> units(&_units[0], &_units[6]);
    std::string map_file = "tests/cpp_unit/FieldTools/test_sector_map.table";
    SectorMagneticFieldMap map(map_file, "tosca_sector", units, "Dipole");

    double polar[4] = {2110., 5., -20./180.*M_PI, 0.};
    double cart[4] = {polar[0]*::cos(polar[2]), polar[1],
                      polar[0]*::sin(polar[2]), 0.};
    double field_cart[6] = {0., 0., 0., 0., 0., 0.};
    double field_polar[6] = {0., 0., 0., 0., 0., 0.};
    double field_carted[6] = {0., 0., 0., 0., 0., 0.};
    map.GetFieldValue(cart, field_cart);
    map.GetFieldValuePolar(polar, field_polar);
    field_carted[0] = field_polar[0]*::cos(polar[2])+
                     field_polar[2]*::sin(-polar[2]);
    field_carted[1] = field_polar[1];
    field_carted[2] = field_polar[0]*::sin(-polar[2])+
                     field_polar[2]*::cos(polar[2]);

    EXPECT_NEAR(field_carted[0], field_cart[0], 1e-9)
                              << polar[0] << " " << polar[1] << " " << polar[2];
    EXPECT_NEAR(field_carted[1], field_cart[1], 1e-9)
                              << polar[0] << " " << polar[1] << " " << polar[2];
    EXPECT_NEAR(field_carted[2], field_cart[2], 1e-9)
                              << polar[0] << " " << polar[1] << " " << polar[2];
}

}

