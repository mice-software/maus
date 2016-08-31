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

#include "Utils/Exception.hh"

#include "src/common_cpp/FieldTools/SectorField.hh"

namespace MAUS {

class SectorFieldTestClass : public SectorField {
  public:
    virtual void GetFieldValuePolar
                       (const double* point_polar, double* field_polar) const {}

    virtual void GetFieldValue
                     (const double* point_cartes, double* field_cartes) const {}

    void SetPolarBoundingBox(double bbMinR, double bbMinY, double bbMinPhi,
                             double bbMaxR, double bbMaxY, double bbMaxPhi) {
        SectorField::SetPolarBoundingBox(bbMinR, bbMinY, bbMinPhi,
                             bbMaxR, bbMaxY, bbMaxPhi);
    }

    std::vector<double> GetBoundingBoxMin() {return BTField::bbMin;}
    std::vector<double> GetBoundingBoxMax() {return BTField::bbMax;}
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

void __testBB(const double* bb_polar_in, const double* bb_cart_expected) {
    SectorFieldTestClass test;
    test.SetPolarBoundingBox(bb_polar_in[0], bb_polar_in[1], bb_polar_in[2],
                             bb_polar_in[3], bb_polar_in[4], bb_polar_in[5]);
    std::vector<double> bb_polar_min_out = test.GetPolarBoundingBoxMin();
    std::vector<double> bb_polar_max_out = test.GetPolarBoundingBoxMax();

    std::vector<double> bb_cart_min_out = test.GetBoundingBoxMin();
    std::vector<double> bb_cart_max_out = test.GetBoundingBoxMax();

    for (size_t i = 0; i < 3; ++i) {
        EXPECT_NEAR(bb_polar_min_out[i], bb_polar_in[i], 1e-12);
        EXPECT_NEAR(bb_polar_max_out[i], bb_polar_in[i+3], 1e-12);
    }
    std::string xyz[3] = {"x", "y", "z"};
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_NEAR(bb_cart_min_out[i], bb_cart_expected[i], 1e-3)
           << xyz[i] << "-min r_min " << bb_polar_in[0] << " r_max "
           << bb_polar_in[3] << " phi_min " << bb_polar_in[2]/M_PI*180.
           << " phi_max " << bb_polar_in[5]/M_PI*180.;
        EXPECT_NEAR(bb_cart_max_out[i], bb_cart_expected[i+3], 1e-3)
           << xyz[i] << "-max r_min " << bb_polar_in[0] << " r_max "
           << bb_polar_in[3] << " phi_min " << bb_polar_in[2]/M_PI*180.
           << " phi_max " << bb_polar_in[5]/M_PI*180.;
        EXPECT_GT(bb_cart_max_out[i], bb_cart_min_out[i]);
    }
}

TEST(SectorFieldTest, SetPolarBoundingBoxTest) {
    // basic check - unit circle
    double bb_polar_1[6] = {0., 0., 0., 1., 1., M_PI/2.};
    double bb_cart_1[6] = {0., 0., 0., 1., 1., 1.};
    __testBB(bb_polar_1, bb_cart_1);

    // check coordinates are okay
    double bb_polar_3[6] = {0.5, 1., M_PI*(0.75-1e-9),
                            1.5, 3., M_PI*(0.75+1e-9)};
    double bb_cart_3[6] = {-1.5/::sqrt(2.), 1., 0.5/::sqrt(2.),
                           -0.5/::sqrt(2.), 3., 1.5/::sqrt(2.)};
    __testBB(bb_polar_3, bb_cart_3);

    // check detect axis crossing correctly
    double bb_polar_2[6] = {0.5, -3., -M_PI*1.999, 1.5, 3., M_PI*1.999};
    double bb_cart_2[6] = {-1.5, -3., -1.5, 1.5, 3., 1.5};
    __testBB(bb_polar_2, bb_cart_2);


    double bb_polar_4[6] = {0.5, 1., M_PI*(-0.25),
                            1.5, 3., M_PI*(0.75)};
    double bb_cart_4[6] = {-1.5/::sqrt(2.), 1., -1.5/::sqrt(2.),
                            1.5, 3., 1.5};
    __testBB(bb_polar_4, bb_cart_4);

    double bb_polar_5[6] = {0.5, 1., M_PI*(-1.25),
                            1.5, 3., M_PI*(-0.25)};
    double bb_cart_5[6] = {-1.5, 1., -1.5,
                            1.5/::sqrt(2.), 3., 1.5/::sqrt(2.)};
    __testBB(bb_polar_5, bb_cart_5);


    double bb_polar_6[6] = {0.5, 1., M_PI*(0.75),
                            1.5, 3., M_PI*(1.75)};
    double bb_cart_6[6] = {-1.5, 1., -1.5,
                            1.5/::sqrt(2.), 3., 1.5/::sqrt(2.)};
    __testBB(bb_polar_6, bb_cart_6);
}

TEST(SectorFieldTest, SetPolarBoundingBoxThrowTest) {
    double bb_polar_0[6] = {0.5, 3., M_PI*(-0.25),
                            1.5, 1., M_PI*(0.75)};
    double bb_polar_1[6] = {1.5, 1., M_PI*(-0.25),
                            0.5, 3., M_PI*(0.75)};
    double bb_polar_2[6] = {-0.5, 1., M_PI*(-0.25),
                            0.5, 3., M_PI*(0.75)};
    double bb_polar_3[6] = {1.5, 1., M_PI*(-0.25),
                            -0.5, 3., M_PI*(0.75)};
    double bb_polar_4[6] = {1.5, 1., M_PI*(0.25),
                            -0.5, 3., M_PI*(-0.25)};
    double bb_polar_5[6] = {1.5, 1., M_PI*(-2.1),
                            -0.5, 3., M_PI*(-0.25)};
    double bb_polar_6[6] = {1.5, 1., M_PI*(0.),
                            -0.5, 3., M_PI*(2.1)};

    double * bb_polar[] = {bb_polar_0, bb_polar_1, bb_polar_2, bb_polar_3,
                           bb_polar_4, bb_polar_5, bb_polar_6};
    for (size_t i = 0; i < 7; ++i) {
        SectorFieldTestClass test;
        EXPECT_THROW(test.SetPolarBoundingBox(bb_polar[i][0], bb_polar[i][1],
                                              bb_polar[i][2], bb_polar[i][3],
                                              bb_polar[i][4], bb_polar[i][5]),
                     Exceptions::Exception) << i;
    }
}
}
