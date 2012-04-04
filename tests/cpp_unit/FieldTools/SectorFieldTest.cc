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

#include "src/common_cpp/FieldTools/SectorField.hh"

namespace MAUS {

class SectorFieldTestClass : public SectorField {
  public:
    virtual void GetFieldValuePolar
                             (const double* point_polar, double* field_polar) {}

    virtual void GetFieldValue
                           (const double* point_cartes, double* field_cartes) {}
};

void __testConvertPosToPolarAndCartesian(const double* position_in,
                        const double* position_expected) {
    double position_test[3];
    position_test[0] = position_in[0];
    position_test[1] = position_in[1];
    position_test[2] = position_in[2];
    SectorField::ConvertToPolar(position_test);
    if (position_expected != NULL) {
        EXPECT_NEAR(position_test[0], position_expected[0], 1e-9)
            << position_in[0] << " " << position_in[1] << " " << position_in[2];
        EXPECT_NEAR(position_test[1], position_expected[1], 1e-9)
            << position_in[0] << " " << position_in[1] << " " << position_in[2];
        EXPECT_NEAR(position_test[2], position_expected[2], 1e-9)
            << position_in[0] << " " << position_in[1] << " " << position_in[2];
    }
    SectorField::ConvertToCartesian(position_test);
    EXPECT_NEAR(position_test[0], position_in[0], 1e-9)
            << position_in[0] << " " << position_in[1] << " " << position_in[2];
    EXPECT_NEAR(position_test[1], position_in[1], 1e-9)
            << position_in[0] << " " << position_in[1] << " " << position_in[2];
    EXPECT_NEAR(position_test[2], position_in[2], 1e-9)
            << position_in[0] << " " << position_in[1] << " " << position_in[2];
}

void __testConvertValueToPolarAndCartesian(const double* position, 
                                           const double* value_in,
                                           const double* value_expected) {
    double value_test[3];
    value_test[0] = value_in[0];
    value_test[1] = value_in[1];
    value_test[2] = value_in[2];
    SectorField::ConvertToPolar(position, value_test);
    if (value_expected != NULL) {
        EXPECT_NEAR(value_test[0], value_expected[0], 1e-9)
            << value_in[0] << " " << value_in[1] << " " << value_in[2];
        EXPECT_NEAR(value_test[1], value_expected[1], 1e-9)
            << value_in[0] << " " << value_in[1] << " " << value_in[2];
        EXPECT_NEAR(value_test[2], value_expected[2], 1e-9)
            << value_in[0] << " " << value_in[1] << " " << value_in[2];
    }
    SectorField::ConvertToCartesian(position, value_test);
    EXPECT_NEAR(value_test[0], value_in[0], 1e-9)
            << value_in[0] << " " << value_in[1] << " " << value_in[2];
    EXPECT_NEAR(value_test[1], value_in[1], 1e-9)
            << value_in[0] << " " << value_in[1] << " " << value_in[2];
    EXPECT_NEAR(value_test[2], value_in[2], 1e-9)
            << value_in[0] << " " << value_in[1] << " " << value_in[2];

}

TEST(SectorFieldTest, ConvertPositionToPolarAndCartesianTest) {
    double position_in_1[3] = {1., -1., 0.};
    double position_ex_1[3] = {1., -1., 0.};
    __testConvertPosToPolarAndCartesian(position_in_1, position_ex_1);
    double position_in_2[3] = {7., 17., 7.};
    double position_ex_2[3] = {::sqrt(2.)*7., 17., 0.25*M_PI};
    __testConvertPosToPolarAndCartesian(position_in_2, position_ex_2);
    double position_in_3[3] = {-1., 21., 1.};
    double position_ex_3[3] = {::sqrt(2.), 21., 0.75*M_PI};
    __testConvertPosToPolarAndCartesian(position_in_3, position_ex_3);
    double position_in_4[3] = {-1., -21., -1.};
    double position_ex_4[3] = {::sqrt(2.), -21., -0.75*M_PI};
    __testConvertPosToPolarAndCartesian(position_in_4, position_ex_4);
    double position_in_5[3] = {1., 0., -1.};
    double position_ex_5[3] = {::sqrt(2.), 0., -0.25*M_PI};
    __testConvertPosToPolarAndCartesian(position_in_5, position_ex_5);
    double position_in_6[3] = {0., 0., 0.};
    double position_ex_6[3] = {0., 0., 0.};
    __testConvertPosToPolarAndCartesian(position_in_6, position_ex_6);
}

TEST(SectorFieldTest, ConvertValueToPolarAndCartesianTest) {
    double position_1[3] = {2., 0., 0.25*M_PI};
    double position_2[3] = {3., -2., 0.75*M_PI};
    double position_3[3] = {4., 4., 1.25*M_PI};
    double position_4[3] = {5., -6., 1.75*M_PI};
    double* position[4] = {position_1, position_2, position_3, position_4};

    double value_in_1[3] = {1., 0., 1.};
    double value_in_2[3] = {-1., -2., 1.};
    double value_in_3[3] = {-1., 4., -1.};
    double value_in_4[3] = {1., -6., -1.};
    double* value_in[4] = {value_in_1, value_in_2, value_in_3, value_in_4};

    double value_ex_2_1[3] = {0., 0., -::sqrt(2)};
    double value_ex_2_2[3] = {::sqrt(2.), -2., 0.};
    double value_ex_2_3[3] = {0., 4., ::sqrt(2)};
    double value_ex_2_4[3] = {-::sqrt(2.), -6., 0.};
    double* value_ex_2[4] =
                       {value_ex_2_1, value_ex_2_2, value_ex_2_3, value_ex_2_4};

    double value_ex_4_1[3] = {0., 0., ::sqrt(2)};
    double value_ex_4_2[3] = {-::sqrt(2.), -2., 0.};
    double value_ex_4_3[3] = {0., 4., -::sqrt(2)};
    double value_ex_4_4[3] = {::sqrt(2.), -6., 0.};
    double* value_ex_4[4] =
                       {value_ex_4_1, value_ex_4_2, value_ex_4_3, value_ex_4_4};


    // check for selected values
    for (size_t i = 0; i < 4; ++i) {
        __testConvertValueToPolarAndCartesian
                                      (position[1], value_in[i], value_ex_2[i]);
        __testConvertValueToPolarAndCartesian
                                      (position[3], value_in[i], value_ex_4[i]);
    }

    // check that the routine inverts properly
    for (size_t i = 0; i < 4; ++i) {
        for (size_t j = 0; j < 4; ++j) {
            __testConvertValueToPolarAndCartesian
                                              (position[i], value_in[j], NULL);
        }
    }
}

TEST(SectorFieldTest, SetPolarBoundingBoxTest) {
    EXPECT_TRUE(false);
}

}
