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

#include "src/common_cpp/FieldTools/SectorMagneticFieldMap.hh"
#include "Utils/Exception.hh"

namespace MAUS {

std::string SECTORMAGNETICMAP =
             "${MAUS_ROOT_DIR}/tests/cpp_unit/FieldTools/test_sector_map.table";

// Check ReadToscaMap function for no symmetry
TEST(SectorMagneticFieldMapTest, TestReadToscaMapNoSymmetry) {
    double _units[6] = {10., 10., 10., 1.e-4, 1.e-4, 1.e-4};
    std::vector<double> units(&_units[0], &_units[6]);
    std::string map_file = SECTORMAGNETICMAP;

    ASSERT_THROW(SectorMagneticFieldMapIO::ReadMap
                  (map_file, "Nonsense", units, "Dipole"), MAUS::Exceptions::Exception);
    ASSERT_THROW(SectorMagneticFieldMapIO::ReadMap
                  ("Nonsense", "tosca_sector_1", units, "Dipole"), MAUS::Exceptions::Exception);
    ASSERT_THROW(SectorMagneticFieldMapIO::ReadMap
                  (map_file, "tosca_sector_1", units, "nonsense"), MAUS::Exceptions::Exception);


    Interpolator3dGridTo3d* interpolator_none = SectorMagneticFieldMapIO::
                               ReadMap(map_file, "tosca_sector_1", units, "None");
    ThreeDGrid* grid_none = interpolator_none->GetMesh();
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

// Check ReadToscaMap function for dipole symmetry
TEST(SectorMagneticFieldMapTest, TestReadToscaMapDipoleSymmetry) {
    double _units[6] = {10., 10., 10., 1.e-4, 1.e-4, 1.e-4};
    std::vector<double> units(&_units[0], &_units[6]);
    std::string map_file = SECTORMAGNETICMAP;

    Interpolator3dGridTo3d* interpolator_dipole = SectorMagneticFieldMapIO::
                             ReadMap(map_file, "tosca_sector_1", units, "Dipole");
    ThreeDGrid* grid_dip = interpolator_dipole->GetMesh();
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

// Check that we can read the same map twice, clear the cache, reread, etc.
TEST(SectorMagneticFieldMapTest, TestFieldMapCache) {
    double _units[6] = {10., 10., 10., 1.e-4, 1.e-4, 1.e-4};
    std::vector<double> units(&_units[0], &_units[6]);

    SectorMagneticFieldMap* map;
    map = new SectorMagneticFieldMap
                         (SECTORMAGNETICMAP, "tosca_sector_1", units, "Dipole");
    delete map;
    map = new SectorMagneticFieldMap
                         (SECTORMAGNETICMAP, "tosca_sector_1", units, "Dipole");
    delete map;
    SectorMagneticFieldMap::ClearFieldCache();
    map = new SectorMagneticFieldMap
                         (SECTORMAGNETICMAP, "tosca_sector_1", units, "Dipole");
    delete map;
    SectorMagneticFieldMap::ClearFieldCache();
}

// Check GetFieldValuePolar function
TEST(SectorMagneticFieldMapTest, TestGetFieldValuePolar) {
    double _units[6] = {10., 10., 10., 1.e-4, 1.e-4, 1.e-4};
    std::vector<double> units(&_units[0], &_units[6]);
    std::string map_file = SECTORMAGNETICMAP;
    SectorMagneticFieldMap map(map_file, "tosca_sector_1", units, "Dipole");

    double b0 = 0.727;
    double r0 = 2350.;
    double k0 = 1.92;

    // along centre, check we are close to the scaling law
    for (double r = 2100.; r < 2120.; r += 5.) {
        double point[4] = {r, 0., 0., 0.};
        double field[6] = {0., 0., 0., 0., 0., 0.};
        map.GetFieldValuePolar(point, field);
        EXPECT_NEAR(field[0], 0., 1e-9) << " at radius " << r;
        EXPECT_NEAR(field[1], b0*::pow(r/r0, k0), 0.1)
                                                          << " at radius " << r;
        EXPECT_NEAR(field[2], 0., 1e-9) << " at radius " << r;
    }

    // at random point in midplane, check bx == 0, by != 0 and bz == 0
    double point[4] = {2110., 0., -20./180.*M_PI, 0.};
    double field[6] = {0., 0., 0., 0., 0., 0.};
    map.GetFieldValuePolar(point, field);
    EXPECT_NEAR(field[0], 0., 1e-9)
                 << "field " << field[0]  << " " << field[1] << " " << field[2];
    EXPECT_GT(fabs(field[1]), 0.)
                 << "field " << field[0]  << " " << field[1] << " " << field[2];
    EXPECT_NEAR(field[2], 0., 1e-9)
                 << "field " << field[0]  << " " << field[1] << " " << field[2];

    // check field at top corners
    double point_0[4] = {2100.+1e-9, 10.-1.e-9, 22.5/180.*M_PI-1.e-9, 0.};
    double field_0[6] = {0., 0., 0., 0., 0., 0.};
    double field_ex_0[3] = {-0.10002656988, -5.3512047987, -0.42126847878E-01};

    double point_1[4] = {2100.+1e-9, 10.-1.e-9, -22.5/180.*M_PI+1.e-9, 0.};
    double field_1[6] = {0., 0., 0., 0., 0., 0.};
    double field_ex_1[3] = {-0.100517390036, -5.35120480497, 0.40941903642E-01};

    double point_2[4] = {2120.-1e-9, 10.-1.e-9, 22.5/180.*M_PI-1.e-9, 0.};
    double field_2[6] = {0., 0., 0., 0., 0., 0.};
    double field_ex_2[3] = {-0.8253847298E-01, -5.542417009, -0.3492049484E-01};

    double point_3[4] = {2120.-1e-9, 10.-1.e-9, -22.5/180.*M_PI+1.e-9, 0.};
    double field_3[6] = {0., 0., 0., 0., 0., 0.};
    double field_ex_3[3] = {-0.8305598395E-01, -5.5424170094, 0.3367111284E-01};

    double* points[] = {point_0, point_1, point_2, point_3};
    double* field_in[] = {field_0, field_1, field_2, field_3};
    double* field_ex[] = {field_ex_0, field_ex_1, field_ex_2, field_ex_3};
    for (int j = 0; j < 4; ++j) {
        SectorField::ConvertToPolar(points[j], field_ex[j]);
        map.GetFieldValuePolar(points[j], field_in[j]);
        for (int k = 0; k < 3; ++k) {
            EXPECT_NEAR(field_in[j][k], field_ex[j][k]*1.e-4, 1e-9)
                                              << "point " << j << " axis " << k;
        }
    }
    SectorMagneticFieldMap::ClearFieldCache();
}

// Check GetFieldValue function
TEST(SectorMagneticFieldMapTest, TestGetFieldValueCartesian) {
    // Exactly same points tested as in polar, but this time we use the
    // cartesian GetFieldValue method (and do appropriate conversions on test
    // data)
    double _units[6] = {10., 10., 10., 1.e-4, 1.e-4, 1.e-4};
    std::vector<double> units(&_units[0], &_units[6]);
    std::string map_file = SECTORMAGNETICMAP;
    SectorMagneticFieldMap map(map_file, "tosca_sector_1", units, "Dipole");

    double b0 = 0.727;
    double r0 = 2350.;
    double k0 = 1.92;

    // along centre, check we are close to the scaling law
    for (double r = 2100.; r < 2120.; r += 5.) {
        double point[4] = {r, 0., 0., 0.};
        double field[6] = {0., 0., 0., 0., 0., 0.};
        map.GetFieldValue(point, field);
        EXPECT_NEAR(field[0], 0., 1e-9) << " at radius " << r;
        EXPECT_NEAR(field[1], b0*::pow(r/r0, k0), 0.1) << " at radius " << r;
        EXPECT_NEAR(field[2], 0., 1e-9) << " at radius " << r;
    }

    // at random point in midplane, check bx == 0, by != 0 and bz == 0
    double point[4] = {2110., 0., -20./180.*M_PI, 0.};
    double field[6] = {0., 0., 0., 0., 0., 0.};
    SectorField::ConvertToCartesian(point);
    map.GetFieldValue(point, field);
    EXPECT_NEAR(field[0], 0., 1e-9)
                 << "field " << field[0]  << " " << field[1] << " " << field[2];
    EXPECT_GT(fabs(field[1]), 0.)
                 << "field " << field[0]  << " " << field[1] << " " << field[2];
    EXPECT_NEAR(field[2], 0., 1e-9)
                 << "field " << field[0]  << " " << field[1] << " " << field[2];

    // check field at top corners
    double point_0[4] = {2100.+1e-9, 10.-1.e-9, 22.5/180.*M_PI-1.e-9, 0.};
    double field_0[6] = {0., 0., 0., 0., 0., 0.};
    double field_ex_0[3] = {-0.10002656988, -5.3512047987, -0.42126847878E-01};

    double point_1[4] = {2100.+1e-9, 10.-1.e-9, -22.5/180.*M_PI+1.e-9, 0.};
    double field_1[6] = {0., 0., 0., 0., 0., 0.};
    double field_ex_1[3] = {-0.100517390036, -5.35120480497, 0.40941903642E-01};

    double point_2[4] = {2120.-1e-9, 10.-1.e-9, 22.5/180.*M_PI-1.e-9, 0.};
    double field_2[6] = {0., 0., 0., 0., 0., 0.};
    double field_ex_2[3] = {-0.8253847298E-01, -5.542417009, -0.3492049484E-01};

    double point_3[4] = {2120.-1e-9, 10.-1.e-9, -22.5/180.*M_PI+1.e-9, 0.};
    double field_3[6] = {0., 0., 0., 0., 0., 0.};
    double field_ex_3[3] = {-0.8305598395E-01, -5.5424170094, 0.3367111284E-01};

    double* points[] = {point_0, point_1, point_2, point_3};
    double* field_in[] = {field_0, field_1, field_2, field_3};
    double* field_ex[] = {field_ex_0, field_ex_1, field_ex_2, field_ex_3};
    for (int j = 0; j < 4; ++j) {
        SectorField::ConvertToCartesian(points[j]);
        map.GetFieldValue(points[j], field_in[j]);
        for (int k = 0; k < 3; ++k) {
            EXPECT_NEAR(field_in[j][k], field_ex[j][k]*1.e-4, 1e-9)
                                              << "point " << j << " axis " << k;
        }
    }
    SectorMagneticFieldMap::ClearFieldCache();
}
}

