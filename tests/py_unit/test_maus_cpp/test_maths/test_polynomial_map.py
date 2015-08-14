import random
import unittest
import maus_cpp.polynomial_map

class PolynomialMapTestCase(unittest.TestCase):
    def setUp(self):
        self.coefficients = [
            [1., 2., 3.],
            [4., 5., 6.],
        ]

    def test_init(self):
        my_map = maus_cpp.polynomial_map.PolynomialMap(2, self.coefficients)
        self.assertEqual(my_map.get_coefficients_as_matrix(), self.coefficients)
        try:
          maus_cpp.polynomial_map.PolynomialMap(2, "hello")
          self.assertFalse(True, msg="Should have thrown")
        except TypeError:
          pass
        try:
          maus_cpp.polynomial_map.PolynomialMap(2, [])
          self.assertFalse(True, msg="Should have thrown, no rows")
        except ValueError:
          pass
        try:
          maus_cpp.polynomial_map.PolynomialMap(2, [[]])
          self.assertFalse(True, msg="Should have thrown, no columns")
        except ValueError:
          pass
        try:
          maus_cpp.polynomial_map.PolynomialMap(2, [[1.], [1., 2.]])
          self.assertFalse(True, msg="Should have thrown, not rectangular")
        except ValueError:
          pass
        try:
          maus_cpp.polynomial_map.PolynomialMap(2, [["a"]])
          self.assertFalse(True, msg="Should have thrown, wrong type")
        except TypeError:
          pass
        maus_cpp.polynomial_map.PolynomialMap(1, [[1.]])

    def test_evaluate(self):
        my_map = maus_cpp.polynomial_map.PolynomialMap(2, self.coefficients)
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

    def test_least_squares_fit(self):
        my_map = maus_cpp.polynomial_map.PolynomialMap(2, self.coefficients)
        points = [[random.uniform(-5, 5), random.uniform(-5, 5)] for i in range(100)]
        values = [my_map.evaluate(a_point) for a_point in points]
        my_map_2 = maus_cpp.polynomial_map.PolynomialMap.least_squares_fit(points, values, 1)
        ref_coeffs = my_map.get_coefficients_as_matrix()
        test_coeffs = my_map_2.get_coefficients_as_matrix()
        for i in range(2):
            for j in range(3):
                self.assertAlmostEqual(ref_coeffs[i][j], test_coeffs[i][j])

    def test_least_squares_fit_bad_input(self):
        points = [[random.uniform(-5, 5), random.uniform(-5, 5)] for i in range(100)]
        values = [[random.uniform(-5, 5), random.uniform(-5, 5)] for i in range(100)]
        maus_cpp.polynomial_map.PolynomialMap.least_squares_fit(points, values, 1)
        try:
            maus_cpp.polynomial_map.PolynomialMap.least_squares_fit(points, values, -1)
            self.assertFalse(True, msg="Should have thrown, bad order")
        except ValueError:
            pass
        values = [[random.uniform(-5, 5), random.uniform(-5, 5)] for i in range(10)]
        try:
            maus_cpp.polynomial_map.PolynomialMap.least_squares_fit(points, values, 1)
            self.assertFalse(True, msg="Should have thrown, misaligned")
        except ValueError:
            pass
        try:
            maus_cpp.polynomial_map.PolynomialMap.least_squares_fit(points, values, "moose")
            self.assertFalse(True, msg="Should have thrown, bad type")
        except TypeError:
            pass
        values = [[random.uniform(-5, 5)] for i in range(100)]
        try:
            maus_cpp.polynomial_map.PolynomialMap.least_squares_fit(points, values, 1)
            self.assertFalse(True, msg="Should have thrown, misaligned")
        except ValueError:
            pass
        values = [[random.uniform(-5, 5), "cheese"] for i in range(100)]
        try:
            maus_cpp.polynomial_map.PolynomialMap.least_squares_fit(points, values, 1)
            self.assertFalse(True, msg="Should have thrown, not a float")
        except ValueError:
            pass
        values = [i for i in range(100)]
        try:
            maus_cpp.polynomial_map.PolynomialMap.least_squares_fit(points, values, 1)
            self.assertFalse(True, msg="Should have thrown, not a list")
        except ValueError:
            pass
        values = "icecream"
        try:
            maus_cpp.polynomial_map.PolynomialMap.least_squares_fit(points, values, 1)
            self.assertFalse(True, msg="Should have thrown, not a list")
        except ValueError:
            pass

    def mice_mc(self, tku_data):
        coefficients = [
            [0.,  2.0, 1.0,  0.,   0.0,],
            [0., -1.0, 1.0,  0.,   0.0,],
            [0.,  0.,  0.,   2.0, -1.0,],
            [0.,  0.,  0.,   1.0,  1.0,],
        ]
        my_map = maus_cpp.polynomial_map.PolynomialMap(4, coefficients)
        tkd_data = [my_map.evaluate(point) for point in tku_data]
        return tkd_data

    def test_least_squares_fit_four_dim(self):
        # tku_data is a list of data, each element being a list like [x, px, y, py]
        tku_data = [None]*100
        for i in range(100):
            x = random.uniform(-100., 100.)
            y = random.uniform(-100., 100.)
            px = random.uniform(-100., 100.)
            py = random.uniform(-100., 100.)
            tku_data[i] = [x, px, y, py]
        
        # this is like - we run the tracking, and extract x, px, y, py data as per the tku_data
        tkd_data = self.mice_mc(tku_data)
        # now we calculate the transfer matrix, by means of least squares fit...
        transfer_matrix = maus_cpp.polynomial_map.PolynomialMap.least_squares_fit(tku_data, tkd_data, 1)
        # and then print it out in a pretty way
        print "Calculated transfer matrix"
        for row in transfer_matrix.get_coefficients_as_matrix():
            for element in row:
                print str(round(element, 2)).rjust(10), 
            print

if __name__ == "__main__":
    unittest.main()
