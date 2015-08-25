#  This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
# 
#  MAUS is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
# 
#  MAUS is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with MAUS.  If not, see <http://www.gnu.org/licenses/>.

"""
test for maus_cpp.polynomial_map
"""

import random
import unittest
import maus_cpp.polynomial_map
from maus_cpp.polynomial_map import PolynomialMap

class PolynomialMapTestCase(unittest.TestCase): # pylint: disable = R0904
    """
    test for maus_cpp.polynomial_map
    """
    def setUp(self): # pylint: disable = C0103
        """
        test setUp
        """
        self.coefficients = [
            [1., 2., 3.],
            [4., 5., 6.],
        ]

    def test_init(self):
        """
        test for maus_cpp.polynomial_map.PolynomialMap.__init__
        """
        my_map = PolynomialMap(2, self.coefficients)
        self.assertEqual(my_map.get_coefficients_as_matrix(), self.coefficients)
        try:
            PolynomialMap(2, "hello")
            self.assertFalse(True, msg="Should have thrown")
        except TypeError:
            pass
        try:
            PolynomialMap(2, [])
            self.assertFalse(True, msg="Should have thrown, no rows")
        except ValueError:
            pass
        try:
            PolynomialMap(2, [[]])
            self.assertFalse(True, msg="Should have thrown, no columns")
        except ValueError:
            pass
        try:
            PolynomialMap(2, [[1.], [1., 2.]])
            self.assertFalse(True, msg="Should have thrown, not rectangular")
        except ValueError:
            pass
        try:
            PolynomialMap(2, [["a"]])
            self.assertFalse(True, msg="Should have thrown, wrong type")
        except TypeError:
            pass
        PolynomialMap(1, [[1.]])

    def test_evaluate(self):
        """
        test for maus_cpp.polynomial_map.PolynomialMap.evaluate
        """
        my_map = PolynomialMap(2, self.coefficients)
        value = my_map.evaluate([-1., -2.])
        self.assertEqual(value, [1.+2.*-1.+3.*-2., 4.+5.*-1.+6.*-2.])
        try:
            my_map.evaluate("hello")
            self.assertFalse(True, msg="Should have thrown, wrong type")
        except TypeError:
            pass
        try:
            my_map.evaluate([1.])
            self.assertFalse(True, msg="Should have thrown, wrong dimension")
        except TypeError:
            pass
        try:
            my_map.evaluate([1., "hello"])
            self.assertFalse(True, msg="Should have thrown, wrong type")
        except TypeError:
            pass

    def test_lsf(self):
        """
        test for maus_cpp.polynomial_map.PolynomialMap.least_squares_fit
        """
        my_map = PolynomialMap(2, self.coefficients)
        points = [[random.uniform(-5, 5), random.uniform(-5, 5)] \
                                                            for i in range(100)]
        values = [my_map.evaluate(a_point) for a_point in points]
        my_map_2 = PolynomialMap.least_squares_fit(points, values, 1)
        ref_coeffs = my_map.get_coefficients_as_matrix()
        test_coeffs = my_map_2.get_coefficients_as_matrix()
        for i in range(2):
            for j in range(3):
                self.assertAlmostEqual(ref_coeffs[i][j], test_coeffs[i][j])

    def test_lsf_bad_input(self):
        """
        test for maus_cpp.polynomial_map.PolynomialMap.least_squares_fit inputs
        """
        points = [[random.uniform(-5, 5), random.uniform(-5, 5)] \
                                                            for i in range(100)]
        values = [[random.uniform(-5, 5), random.uniform(-5, 5)] \
                                                            for i in range(100)]
        maus_cpp.polynomial_map.PolynomialMap.least_squares_fit(points,
                                                                values,
                                                                1)
        try:
            PolynomialMap.least_squares_fit(points, values, -1)
            self.assertFalse(True, msg="Should have thrown, bad order")
        except ValueError:
            pass
        values = [[random.uniform(-5, 5), random.uniform(-5, 5)] \
                                                             for i in range(10)]
        try:
            PolynomialMap.least_squares_fit(points, values, 1)
            self.assertFalse(True, msg="Should have thrown, misaligned")
        except ValueError:
            pass
        try:
            PolynomialMap.least_squares_fit(points, values, "moose")
            self.assertFalse(True, msg="Should have thrown, bad type")
        except TypeError:
            pass
        values = [[random.uniform(-5, 5)] for i in range(100)]
        try:
            PolynomialMap.least_squares_fit(points, values, 1)
            self.assertFalse(True, msg="Should have thrown, misaligned")
        except ValueError:
            pass
        values = [[random.uniform(-5, 5), "cheese"] for i in range(100)]
        try:
            PolynomialMap.least_squares_fit(points, values, 1)
            self.assertFalse(True, msg="Should have thrown, not a float")
        except ValueError:
            pass
        values = [i for i in range(100)]
        try:
            PolynomialMap.least_squares_fit(points, values, 1)
            self.assertFalse(True, msg="Should have thrown, not a list")
        except ValueError:
            pass
        values = "icecream"
        try:
            PolynomialMap.least_squares_fit(points, values, 1)
            self.assertFalse(True, msg="Should have thrown, not a list")
        except ValueError:
            pass

    @classmethod
    def mice_mc(cls, tku_data, ):
        """
        toy mc function
        """
        my_map = PolynomialMap(4, cls.coefficients_4d)
        tkd_data = [my_map.evaluate(point) for point in tku_data]
        return tkd_data

    def test_lsf_four_dim(self):
        """
        test for maus_cpp.polynomial_map.PolynomialMap.least_squares_fit with
        4D input
        """
        # tku_data is a list of data, each element being a list like
        # [x, px, y, py]
        tku_data = [None]*100
        for i in range(100):
            x_var = random.uniform(-100., 100.)
            y_var = random.uniform(-100., 100.)
            px_var = random.uniform(-100., 100.)
            py_var = random.uniform(-100., 100.)
            tku_data[i] = [x_var, px_var, y_var, py_var]
        
        # this is like - we run the tracking, and extract x, px, y, py data
        # as per the tku_data
        tkd_data = self.mice_mc(tku_data)
        # now we calculate the transfer matrix, by means of least squares fit...
        fitted_map = PolynomialMap.least_squares_fit(tku_data, tkd_data, 1)
        matrix = fitted_map.get_coefficients_as_matrix()
        self.assertEqual(len(matrix), len(self.coefficients_4d))
        for i in range(len(matrix)):
            self.assertEqual(len(matrix[i]), len(self.coefficients_4d[i]))
            for j in range(len(matrix[i])):
                self.assertLess(abs(matrix[i][j] - self.coefficients_4d[i][j]),
                                1e-3)


    coefficients_4d = [
            [0.,  2.0, 1.0,  0.,   0.0,],
            [0., -1.0, 1.0,  0.,   0.0,],
            [0.,  0.,  0.,   2.0, -1.0,],
            [0.,  0.,  0.,   1.0,  1.0,],
    ]

if __name__ == "__main__":
    unittest.main()
