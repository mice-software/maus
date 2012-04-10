/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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

// C headers
#include <assert.h>

// Other headers
#include "Config/MiceModule.hh"
#include "src/common_cpp/Recon/SciFi/PatternRecognition.hh"
#include "src/common_cpp/Recon/SciFi/SimpleLine.hh"

#include "gtest/gtest.h"

// namespace MAUS {
class SimpleLineTest : public ::testing::Test {
  protected:
    SimpleLineTest()  {}
    virtual ~SimpleLineTest() {}
    virtual void SetUp()    {}
    virtual void TearDown() {}
};

TEST_F(SimpleLineTest, test_setters_getters) {
  double c = 2.3;
  double c_err = .001;
  double m = 45.34;
  double m_err = 0.023;
  double chisq = 5.;
  double chisq_dof = 1.0;

  SimpleLine line;

  line.set_c(c);
  line.set_c_err(c_err);
  line.set_m(m);
  line.set_m_err(m_err);
  line.set_chisq(chisq);
  line.set_chisq_dof(chisq_dof);

  EXPECT_EQ(line.get_c(), c);
  EXPECT_EQ(line.get_cerr(), c_err);
  EXPECT_EQ(line.get_m(), m);
  EXPECT_EQ(line.get_m_err(), m_err);
  EXPECT_EQ(line.get_chisq(), chisq);
  EXPECT_EQ(line.get_chisq_dof(), chisq_dof);
}
// } // namespace
