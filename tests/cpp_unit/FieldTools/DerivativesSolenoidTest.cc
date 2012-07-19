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

#include "src/common_cpp/FieldTools/DerivativesSolenoid.hh"

namespace MAUS {
EndFieldModel* test_end_field() {
    return new BTMultipole::TanhEndField(1., 0.1, 3);
}

TEST(DerivativesSolenoidTest, TestConstructorDestructor) {
    DerivativesSolenoid sol_default;
    EXPECT_EQ(sol_default.GetPeakField(), 0.);
    EXPECT_EQ(sol_default.GetRMax(), 0.);
    EXPECT_EQ(sol_default.GetZMax(), 0.);
    EXPECT_EQ(sol_default.GetHighestOrder(), 0);
    EXPECT_EQ
            (sol_default.GetEndFieldModel(), static_cast<EndFieldModel*>(NULL));

    DerivativesSolenoid sol_null(2., 10., 11., 3, NULL);
    EXPECT_EQ(sol_null.GetPeakField(), 2.);
    EXPECT_EQ(sol_null.GetRMax(), 10.);
    EXPECT_EQ(sol_null.GetZMax(), 11.);
    EXPECT_EQ(sol_null.GetHighestOrder(), 3);
    EXPECT_EQ(sol_null.GetEndFieldModel(), static_cast<EndFieldModel*>(NULL));

    EndFieldModel* end = test_end_field();
    DerivativesSolenoid sol_value(2., 10., 11., 3, end);
    EXPECT_EQ(sol_value.GetPeakField(), 2.);
    EXPECT_EQ(sol_value.GetRMax(), 10.);
    EXPECT_EQ(sol_value.GetZMax(), 11.);
    EXPECT_EQ(sol_value.GetHighestOrder(), 3);
    EXPECT_EQ(sol_value.GetEndFieldModel(), end);
    EXPECT_EQ(sol_value.BoundingBoxMin()[0], -10.);
    EXPECT_EQ(sol_value.BoundingBoxMin()[1], -10.);
    EXPECT_EQ(sol_value.BoundingBoxMin()[2], -11.);
    EXPECT_EQ(sol_value.BoundingBoxMax()[0], +10.);
    EXPECT_EQ(sol_value.BoundingBoxMax()[1], +10.);
    EXPECT_EQ(sol_value.BoundingBoxMax()[2], +11.);
}

// all
TEST(DerivativesSolenoidTest, TestNullCopyConstructorCloneEquality) {
    DerivativesSolenoid sol_null;
    DerivativesSolenoid sol_null_copy(sol_null);
    EXPECT_EQ(sol_null.GetPeakField(), sol_null_copy.GetPeakField());
    EXPECT_EQ(sol_null.GetRMax(), sol_null_copy.GetRMax());
    EXPECT_EQ(sol_null.GetZMax(), sol_null_copy.GetZMax());
    EXPECT_EQ(sol_null.GetHighestOrder(), sol_null_copy.GetHighestOrder());
    EXPECT_EQ(sol_null.GetEndFieldModel(), sol_null_copy.GetEndFieldModel());

    DerivativesSolenoid sol_null_equ;  // force gcc not to use copy ctor
    EXPECT_EQ(sol_null_equ.GetHighestOrder(), 0);
    sol_null_equ = sol_null;
    EXPECT_EQ(sol_null.GetPeakField(), sol_null_equ.GetPeakField());
    EXPECT_EQ(sol_null.GetRMax(), sol_null_equ.GetRMax());
    EXPECT_EQ(sol_null.GetZMax(), sol_null_equ.GetZMax());
    EXPECT_EQ(sol_null.GetHighestOrder(), sol_null_equ.GetHighestOrder());
    EXPECT_EQ(sol_null.GetEndFieldModel(), sol_null_equ.GetEndFieldModel());

    DerivativesSolenoid& sol_null_clone = *sol_null.Clone();
    EXPECT_EQ(sol_null.GetPeakField(), sol_null_clone.GetPeakField());
    EXPECT_EQ(sol_null.GetRMax(), sol_null_clone.GetRMax());
    EXPECT_EQ(sol_null.GetZMax(), sol_null_clone.GetZMax());
    EXPECT_EQ(sol_null.GetHighestOrder(), sol_null_clone.GetHighestOrder());
    EXPECT_EQ(sol_null.GetEndFieldModel(), sol_null_clone.GetEndFieldModel());
}

// In all these checks, I only check that copying end_field produces something,
// never check that it is right
TEST(DerivativesSolenoidTest, TestCopyConstructorCloneEquality) {
    EndFieldModel* end = test_end_field();
    DerivativesSolenoid sol(2., 10., 11., 3, end);

    DerivativesSolenoid sol_copy(sol);
    EXPECT_EQ(sol.GetPeakField(), sol_copy.GetPeakField());
    EXPECT_EQ(sol.GetRMax(), sol_copy.GetRMax());
    EXPECT_EQ(sol.GetZMax(), sol_copy.GetZMax());
    EXPECT_EQ(sol.GetHighestOrder(), sol_copy.GetHighestOrder());
    EXPECT_NE(sol.GetEndFieldModel(), sol_copy.GetEndFieldModel());
    EXPECT_NE(sol_copy.GetEndFieldModel(), static_cast<EndFieldModel*>(NULL));

    DerivativesSolenoid sol_equ;  // force gcc not to use copy ctor
    EXPECT_EQ(sol_equ.GetHighestOrder(), 0);
    sol_equ = sol;
    EXPECT_EQ(sol.GetPeakField(), sol_equ.GetPeakField());
    EXPECT_EQ(sol.GetRMax(), sol_equ.GetRMax());
    EXPECT_EQ(sol.GetZMax(), sol_equ.GetZMax());
    EXPECT_EQ(sol.GetHighestOrder(), sol_equ.GetHighestOrder());
    EXPECT_NE(sol.GetEndFieldModel(), sol_equ.GetEndFieldModel());
    EXPECT_NE(sol_equ.GetEndFieldModel(), static_cast<EndFieldModel*>(NULL));

    DerivativesSolenoid& sol_clone = *sol.Clone();
    EXPECT_EQ(sol.GetPeakField(), sol_clone.GetPeakField());
    EXPECT_EQ(sol.GetRMax(), sol_clone.GetRMax());
    EXPECT_EQ(sol.GetZMax(), sol_clone.GetZMax());
    EXPECT_EQ(sol.GetHighestOrder(), sol_clone.GetHighestOrder());
    EXPECT_NE(sol.GetEndFieldModel(), sol_clone.GetEndFieldModel());
    EXPECT_NE(sol_clone.GetEndFieldModel(), static_cast<EndFieldModel*>(NULL));

    EXPECT_EQ(sol_clone.BoundingBoxMin()[0], -10.);
    EXPECT_EQ(sol_clone.BoundingBoxMin()[1], -10.);
    EXPECT_EQ(sol_clone.BoundingBoxMin()[2], -11.);
    EXPECT_EQ(sol_clone.BoundingBoxMax()[0], +10.);
    EXPECT_EQ(sol_clone.BoundingBoxMax()[1], +10.);
    EXPECT_EQ(sol_clone.BoundingBoxMax()[2], +11.);
}

std::vector<double> test_get_field_value
                      (DerivativesSolenoid& sol, double x, double y, double z) {
    double point[4] = {x, y, z, 0};
    std::vector<double> field(6);
    for (size_t i = 0; i < 6; ++i) {
        field[i] = 0.;
    }
    sol.GetFieldValue(point, &field[0]);
    return field;
}

TEST(DerivativesSolenoidTest, TestBoundingBox) {
    EndFieldModel* end = test_end_field();
    DerivativesSolenoid sol(2., 2., 1., 3, end);
    // centre
    EXPECT_NEAR(test_get_field_value(sol, 0., 0., 0.)[2], 2., 1e-6);
    // in bounding box
    EXPECT_GT(test_get_field_value(sol, 2.-1e-9, 0., 1.-1e-9)[2], 1e-3);
    EXPECT_GT(test_get_field_value(sol, 2.-1e-9, 0., 1.-1e-9)[0], 1e-3);
    // out of bounding box
    EXPECT_NEAR(test_get_field_value(sol, 2.-1e-9, 0., 1.+1e-9)[0], 0., 1e-12);
    EXPECT_NEAR(test_get_field_value(sol, 2.-1e-9, 0., 1.+1e-9)[2], 0., 1e-12);
    // out of bounding box
    EXPECT_NEAR(test_get_field_value(sol, 2.+1e-9, 0., 1.-1e-9)[0], 0., 1e-12);
    EXPECT_NEAR(test_get_field_value(sol, 2.+1e-9, 0., 1.-1e-9)[2], 0., 1e-12);
}

TEST(DerivativesSolenoidTest, TestFieldOnAxis) {
    EndFieldModel* end = test_end_field();
    DerivativesSolenoid sol(2., 2., 10., 3, end);
    for (double z = -10.; z < 10.; z+= 0.1)
      EXPECT_NEAR(
        test_get_field_value(sol, 0., 0., z)[2], end->Function(z, 0)*2., 1e-9);
}

TEST(DerivativesSolenoidTest, TestFieldMaxwellianness) {
    EndFieldModel* end = test_end_field();
    DerivativesSolenoid sol(2., 1.e9, 1.e9, 3, end);
    double dr = 0.001;
    double z = 1.;
    for (double r = 0.; r < 0.1; r+= dr) {
        double dbxdx = (test_get_field_value(sol, r+dr/2., 0.,     z)[0]
                       -test_get_field_value(sol, r-dr/2., 0.,     z)[0]);
        double dbydy = (test_get_field_value(sol, r,       +dr/2., z)[1]
                       -test_get_field_value(sol, r,       -dr/2., z)[1]);
        double dbzdz = (test_get_field_value(sol, r,       0.,     z+dr/2.)[2]
                       -test_get_field_value(sol, r,       0.,     z-dr/2.)[2]);
        double div_b = dbxdx+dbydy+dbzdz;
        double sum_delta_b = fabs(dbxdx)+fabs(dbydy)+fabs(dbzdz);
        // std::cerr << r << " ** " << dbxdx << " " << dbydy << " " << dbzdz
        //           << " ** " << div_b << std::endl;
        EXPECT_LT(fabs(div_b), 1e-4*sum_delta_b);
    }
}
}

