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

#include "src/common_cpp/DataStructure/SimpleLine.hh"
#include "gtest/gtest.h"

namespace MAUS {

class SimpleLineTestDS : public ::testing::Test {
  protected:
    SimpleLineTestDS()  {}
    virtual ~SimpleLineTestDS() {}
    virtual void SetUp()    {}
    virtual void TearDown() {}
};

TEST_F(SimpleLineTestDS, test_default_constructor) {
  SimpleLine l1;
  EXPECT_EQ(l1.get_c(), 0.0);
  EXPECT_EQ(l1.get_c_err(), 0.0);
  EXPECT_EQ(l1.get_m(), 0.0);
  EXPECT_EQ(l1.get_m_err(), 0.0);
  EXPECT_EQ(l1.get_chisq(), 0.0);
  EXPECT_EQ(l1.get_chisq_dof(), 0.0);
}

TEST_F(SimpleLineTestDS, test_first_parameter_constructor) {
  double c = 1.0;
  double m = 2.0;

  SimpleLine l1(c, m);

  EXPECT_EQ(l1.get_c(), c);
  EXPECT_EQ(l1.get_m(), m);
}

TEST_F(SimpleLineTestDS, test_second_parameter_constructor) {
  double c = 1.0;
  double c_err = 0.1;
  double m = 2.0;
  double m_err = 0.2;
  double chisq = 1.5;
  double chisq_dof = 0.5;

  SimpleLine l1(c, c_err, m, m_err, chisq, chisq_dof);

  EXPECT_EQ(l1.get_c(), c);
  EXPECT_EQ(l1.get_c_err(), c_err);
  EXPECT_EQ(l1.get_m(), m);
  EXPECT_EQ(l1.get_m_err(), m_err);
  EXPECT_EQ(l1.get_chisq(), chisq);
  EXPECT_EQ(l1.get_chisq_dof(), chisq_dof);
}

TEST_F(SimpleLineTestDS, test_getters_setters_clear) {
  double c = 1.0;
  double c_err = 0.1;
  double m = 2.0;
  double m_err = 0.2;
  double chisq = 1.5;
  double chisq_dof = 0.5;

  SimpleLine l1;

  l1.set_c(c);
  l1.set_c_err(c_err);
  l1.set_m(m);
  l1.set_m_err(m_err);
  l1.set_chisq(chisq);
  l1.set_chisq_dof(chisq_dof);

  EXPECT_EQ(l1.get_c(), c);
  EXPECT_EQ(l1.get_c_err(), c_err);
  EXPECT_EQ(l1.get_m(), m);
  EXPECT_EQ(l1.get_m_err(), m_err);
  EXPECT_EQ(l1.get_chisq(), chisq);
  EXPECT_EQ(l1.get_chisq_dof(), chisq_dof);

  l1.clear();

  EXPECT_EQ(l1.get_c(), 0.0);
  EXPECT_EQ(l1.get_c_err(), 0.0);
  EXPECT_EQ(l1.get_m(), 0.0);
  EXPECT_EQ(l1.get_m_err(), 0.0);
  EXPECT_EQ(l1.get_chisq(), 0.0);
  EXPECT_EQ(l1.get_chisq_dof(), 0.0);
}

} // ~namespace MAUS
