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

#include "src/common_cpp/Recon/Kalman/Global/Measurement.hh"

namespace MAUS {
namespace Kalman {
namespace Global {

void test_is_equal(TMatrixD test, TMatrixD ref) {
    EXPECT_EQ(test.GetNcols(), ref.GetNcols());
    EXPECT_EQ(test.GetNrows(), ref.GetNrows());
    for (int i = 0; i < test.GetNrows(); i++) {
        for (int j = 0; j < test.GetNcols(); j++) {
            EXPECT_NEAR(test[i][j], ref[i][j], 1e-9);
        }
    }
}

TEST(MeasurementTest, ConstructorTest) {
    State state(1);
    TMatrixD ref(2, 3);
    for (size_t i = 0; i < 2; i++) {
        for (size_t j = 0; j < 3; j++) {
            ref[i][j] = (i+1) + (j+1)*10;
        }
    }
    // Default constructor
    std::cerr << "A " << std::flush;
    Measurement meas;
    test_is_equal(meas.CalculateMeasurementMatrix(state), TMatrixD());
    // SetMeasurementMatrix
    std::cerr << "B " << std::flush;
    // meas.SetMeasurementMatrix(ref);
    // test_is_equal(meas.CalculateMeasurementMatrix(state), ref);
    // std::cerr << "C " << std::flush;
    // constructor taking matrix arg
    Measurement meas1(ref);
    test_is_equal(meas1.CalculateMeasurementMatrix(state), ref);
    std::cerr << "C " << std::flush;
    // Clone
    Measurement* meas2 = meas1.Clone();
    test_is_equal(meas2->CalculateMeasurementMatrix(state), ref);
    delete meas2;
    std::cerr << "D " << std::flush;
    // copy constructor
    Measurement meas3(meas1);
    test_is_equal(meas3.CalculateMeasurementMatrix(state), ref);
    std::cerr << "E " << std::flush;
}

TEST(MeasurementTest, DetectorToMeasurementMapTest) {
    using namespace DataStructure::Global;
    Measurement::DetType type_list[] = {
        kTOF0, kTOF1, kTOF2,
        kTracker0_1, kTracker0_2, kTracker0_3, kTracker0_4, kTracker0_5,
        kTracker1_1, kTracker1_2, kTracker1_3, kTracker1_4, kTracker1_5,
        kVirtual
    };
    Measurement::SetupDetectorToMeasurementMap();
    std::map<Measurement::DetType, Measurement_base*>& map =
                                     Measurement::GetDetectorToMeasurementMap();
    EXPECT_EQ(map.size(), 14); // check that elements of each type exist
    for (size_t i = 0; i < 14; ++i) {
        map.at(type_list[i]);
    }
}
}
}
}



