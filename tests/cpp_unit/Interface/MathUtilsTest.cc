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

#include <math.h>

#include <vector>

#include "gtest/gtest.h"

#include "src/legacy/Interface/MathUtils.hh"
#include "src/legacy/Interface/STLUtils.hh"

namespace {

// Used to remove duplicated values in e.g. lists of polynomials (see docs)
TEST(MathUtils, CompactVectorTest) {
  std::vector< std::vector<int> > vec;
  vec.push_back(std::vector<int>(1, 0));
  vec.push_back(std::vector<int>(4, 1));
  vec[1][0] = -2;
  vec[1][2] = 0;
  vec.push_back(std::vector<int>(4, 2));
  vec.push_back(std::vector<int>(4, 1));
  vec.push_back(std::vector<int>(4, 1));
  vec[2][2] = 0;
  vec[3][2] = 0;
  vec.push_back(std::vector<int>(4, 1));
  vec[4][2] = 0;
  std::vector< std::vector<int> > out = MathUtils::CompactVector(vec);

  std::vector< std::vector<int> > all_tests;
  all_tests.push_back(std::vector<int>(1, 0));
  all_tests.push_back(std::vector<int>(4, 2));
  all_tests.push_back(std::vector<int>(4, 1));
  all_tests.push_back(std::vector<int>(4, 1));
  all_tests[1][2] = 0;
  all_tests[2][0] = 0;
  all_tests[2][2] = 0;
  // expect vector to contain test1, test2 and test3 in some order
  ASSERT_EQ(out.size(), all_tests.size());
  for (size_t i = 0; i < out.size(); i++)
    for (size_t j = 0; j < all_tests.size(); j++)
      if (STLUtils::IterableEquality(all_tests[j], out[i]))
        all_tests.erase(all_tests.begin()+j);
  ASSERT_EQ(all_tests.size(), size_t(0));
}

// Helper function (comparator) to do sort before we compact
TEST(Mathutils, GreaterThanTest) {
  std::vector<int> v1(3, 1);
  std::vector<int> v2(3, 1);
  std::vector<int> v3(2, 1);

  EXPECT_FALSE(MathUtils::GreaterThan(v1, v2))
            << "v1 == v2 should return false";
  v1[1] = 2;
  v2[0] = 2;
  EXPECT_TRUE(MathUtils::GreaterThan(v1, v2));
  EXPECT_FALSE(MathUtils::GreaterThan(v2, v1));
  v1[1] = 1;
  EXPECT_FALSE(MathUtils::GreaterThan(v1, v3));
  v3[1] = 0;
  EXPECT_TRUE(MathUtils::GreaterThan(v1, v3));
}

class EngeTest : public ::testing::Test {
  protected:
    EngeTest() {
      _lambda = 0.5;
      double _enge_a[4] = {0., 1., 0., 0.};
      _a = std::vector<double>(_enge_a, _enge_a+4);
      _x0 = 3.;
      _enge = Enge(_a, _x0, _lambda, 10);
    }
    virtual ~EngeTest() {}
    virtual void SetUp() {}
    virtual void TearDown() {}
    Enge _e_zero, _enge;
    std::vector<double> _a;
    double _lambda, _x0;
};

// compare numerical derivative with analytical derivative

// check that f' < tolerance in the range xmin -> xmax, for all derivatives up to
// max_n (where n=1 is the first derivative). (n+1)th numerical derivative is
// calculated from nth analytical derivative using standard formula
// f' = ( f(x+dx)-f(x-dx) )/2dx
// Deal with two cases:
// * where f is large, requiring large tolerance on numerical derivative
// * where f is small, requiring fixed tolerance on numerical derivative
// we use the largest of y*dy_tol and dy_tol as the test tolerance on difference
// between numerical and analytical derivative
template<class T>
void test_deriv(T object, double (T::*function)(double, int) const, double dx,
                double xmin, double xmax, double dy_tol, int max_n) {
  for (int n = 1; n <= max_n; ++n) {
    for (double x = xmin; x < xmax; x += (xmax-xmin)/100.) {
      double y = (object.*function)(x, n);
      if (y*dy_tol > dy_tol) dy_tol = y*dy_tol;
      ASSERT_NEAR(y,
                 ((object.*function)(x+dx, n-1)
                 -(object.*function)(x-dx, n-1))/2./dx, dy_tol) << n << " " << x;
    }
  }
}

// check the constructor is okay - just test that it sets parameters correctly
TEST_F(EngeTest, EngeConstructorTest) {
  std::vector<double> b, c;
  b = _e_zero.GetEngeParameters();
  EXPECT_TRUE(STLUtils::IterableEquality(b, c) );
  EXPECT_EQ(_e_zero.GetLambda(), 0);

  Enge enge_alt_zero(c, 0., 0., 10);
  b = enge_alt_zero.GetEngeParameters();
  EXPECT_TRUE(STLUtils::IterableEquality(b, c) );
  EXPECT_EQ(enge_alt_zero.GetLambda(), 0.);
  EXPECT_EQ(enge_alt_zero.GetX0(), 0.);

  b = _enge.GetEngeParameters();
  EXPECT_TRUE(STLUtils::IterableEquality(b, _a) );
  EXPECT_EQ(_enge.GetLambda(), _lambda);
  EXPECT_EQ(_enge.GetX0(), _x0);
}

// check Set functions actually set stuff
TEST_F(EngeTest, MutatorAccessorTest) {
  std::vector<double> b;
  _enge.SetEngeParameters(b);
  EXPECT_TRUE(STLUtils::IterableEquality(b, _enge.GetEngeParameters()));
  _enge.SetEngeParameters(_a);
  EXPECT_TRUE(STLUtils::IterableEquality(_a, _enge.GetEngeParameters()));

  _enge.SetLambda(0.);
  EXPECT_EQ(_enge.GetLambda(), 0.);
  _enge.SetLambda(_lambda);
  EXPECT_EQ(_enge.GetLambda(), _lambda);

  _enge.SetX0(0.);
  EXPECT_EQ(_enge.GetX0(), 0.);
  _enge.SetX0(_x0);
  EXPECT_EQ(_enge.GetX0(), _x0);
}

// check that we set diff indices correctly - compare indices calculated by hand
// with indices calculated by code
// nb: diff indices index e.g. powers of d^n/dx^n (1+e^f) in enge(n)
TEST_F(EngeTest, SetEngeDiffIndicesTest) {
  int qt[] = {  // test q
    -6, -4, 3, /**/ 4, -3, 1, 1, /**/ 2, -3, 1, 1, /**/ -1, -2, 0, 0, 1};

  std::vector< std::vector<int> > q = Enge::GetQIndex(3);
  ASSERT_EQ(q.size(), size_t(4));
  ASSERT_EQ(q[0].size(), size_t(3));
  for (size_t i = 0; i < 3; ++i) EXPECT_EQ(qt[0+i], q[0][i]);
  ASSERT_EQ(q[1].size(), size_t(4));
  for (size_t i = 0; i < 4; ++i) EXPECT_EQ(qt[3+i], q[1][i]);
  ASSERT_EQ(q[2].size(), size_t(4));
  for (size_t i = 0; i < 4; ++i) EXPECT_EQ(qt[7+i], q[2][i]);
  ASSERT_EQ(q[3].size(), size_t(5));
  for (size_t i = 0; i < 5; ++i) EXPECT_EQ(qt[11+i], q[3][i]);

  std::vector< std::vector<int> > h = Enge::GetHIndex(4);
  int ht[] = {  // test h
    1, 4, /**/ 6, 2, 1, /**/ 4, 1, 0, 1, /**/ 3, 0, 2, /**/ 1, 0, 0, 0, 1};
  std::vector< std::vector<int> > htv(5);
  htv[0] = std::vector<int>(&ht[0], &ht[2]);
  htv[1] = std::vector<int>(&ht[2], &ht[5]);
  htv[2] = std::vector<int>(&ht[5], &ht[9]);
  htv[3] = std::vector<int>(&ht[9], &ht[12]);
  htv[4] = std::vector<int>(&ht[12], &ht[17]);
  ASSERT_EQ(h.size(), htv.size());
  for (size_t i = 0; i < h.size(); ++i)
    for (size_t j = 0; j < htv.size(); ++j)
      if (STLUtils::IterableEquality(htv[j], h[i]))
        htv.erase(htv.begin()+j);
  ASSERT_EQ(htv.size(), size_t(0));
}

// check that we calculate d^n/dx^n h correctly - calculate explicitly h for a
// values and then compare numerical and analytical derivatives
TEST_F(EngeTest, HNTest) {
  for (double x = -10.; x < 11; x++) {
    double y = 0.;
    for (size_t i = 0; i < _a.size(); i++)
      y += _a[i]*pow(x/_lambda, i);
    EXPECT_NEAR(_enge.HN(x, 0), y, 1e-6);
  }
  test_deriv(_enge, &Enge::HN, 1e-3, -10., 10., 1e-6, 9);
}

// check that we calculate d^n/dx^n g correctly - calculate explicitly g for a
// few x values and then compare numerical and analytical derivatives
TEST_F(EngeTest, GNTest) {
  for (double x = -10; x < 11; x++) {
    EXPECT_DOUBLE_EQ(_enge.GN(x, 0), 1+exp(_enge.HN(x, 0)));
  }
  test_deriv(_enge, &Enge::GN, 1e-6, -10., 10., 1e-6, 9);
}

// check that we calculate d^n/dx^n enge correctly - calculate explicitly g for
// a few x values and then compare numerical and analytical derivatives
TEST_F(EngeTest, GetEngeTest) {
  for (double x = -10; x < 11; x++)
    EXPECT_DOUBLE_EQ(_enge.GetEnge(x, 0), 1./(_enge.GN(x, 0)));
  test_deriv(_enge, &Enge::GetEnge, 1e-9, -10., 10., 1e-6, 9);
}

// double enge gives us a double ended enge function - test function value and
// derivatives
TEST_F(EngeTest, GetDoubleEngeTest) { // difference at order 1e-15
  for (double x = -10; x < 11; x++)
    EXPECT_NEAR(_enge.GetDoubleEnge(-x, 0),
            (_enge.GetEnge(x-_x0, 0)+_enge.GetEnge(-x-_x0, 0))-1., 1e-12 ) << x;
  test_deriv(_enge, &Enge::GetDoubleEnge, 1e-6, -10., 10., 1e-3, 9);
}

////////// TANH //////////////

class TanhTest : public ::testing::Test {
  protected:
    TanhTest() :_x0(11.2439), _lambda(2.27567) {
      _d_tanh = Tanh(_x0, _lambda, 10);
    }
    virtual ~TanhTest() {}
    virtual void SetUp()    {}
    virtual void TearDown() {}
    Tanh _d_tanh, _d_tanh_zero;
    double _x0, _lambda;
};

// check that values are set correctly
TEST_F(TanhTest, TanhConstructorTest) {
  EXPECT_EQ(_d_tanh_zero.GetLambda(), 0.);
  EXPECT_EQ(_d_tanh_zero.GetX0(), 0);

  EXPECT_EQ(_d_tanh.GetLambda(), _lambda);
  EXPECT_EQ(_d_tanh.GetX0(), _x0);
}

// check that Set functions set correctly
TEST_F(TanhTest, TanhMutatorAccessorTest) {
  _d_tanh_zero.SetLambda(_lambda);
  _d_tanh_zero.SetX0(_x0);
  EXPECT_EQ(_d_tanh_zero.GetLambda(), _lambda);
  EXPECT_EQ(_d_tanh_zero.GetX0(), _x0);
}

// check that we calculate the diff indices okay (compare with some indices
// calculated by hand)
TEST_F(TanhTest, TanhGetTanhDiffIndicesTest) {
  Tanh::SetTanhDiffIndices(1);
  int tdi1[6] = {-6, 4, /**/ 8, 2, /**/ -2, 0};
  std::vector< std::vector<int> > tdi2 = Tanh::GetTanhDiffIndices(3);
  ASSERT_EQ(size_t(3), tdi2.size());
  for (size_t i = 0; i < tdi2.size(); ++i) {
    for (size_t j = 0; j < 2; ++j)
      ASSERT_EQ(tdi2[i][j], tdi1[i*2+j]);
  }
}

// tanh + derivatives
TEST_F(TanhTest, GetTanhTest) {
  for (double x = -10.; x < 11; x++)
    ASSERT_NEAR(_d_tanh.GetTanh(x, 0), tanh((x+_x0)/_lambda), 1e-9);
  test_deriv(_d_tanh, &Tanh::GetTanh, 1e-6, -10., 10., 1e-6, 6);
}

// neg tanh + derivatives (tanh(-x+x0))
TEST_F(TanhTest, GetNegTanhTest) {
  for (double x = -10.; x < 11; x++)
    ASSERT_NEAR(_d_tanh.GetNegTanh(x, 0), tanh((x-_x0)/_lambda), 1e-9);
  test_deriv(_d_tanh, &Tanh::GetNegTanh, 1e-6, -10., 10., 1e-6, 6);
}

// sum of tanh and neg tanh
TEST_F(TanhTest, GetDoubleTanhTest) {
  for (double x = -20.; x < 21; x++)
    ASSERT_NEAR(2.*_d_tanh.GetDoubleTanh(x, 0),
              tanh((x+_x0)/_lambda)-tanh((x-_x0)/_lambda), 1e-12);
  test_deriv(_d_tanh, &Tanh::GetTanh, 1e-6, -100., 100., 1e-6, 6);
}
}



