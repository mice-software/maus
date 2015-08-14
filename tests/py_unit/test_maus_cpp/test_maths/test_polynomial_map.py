import unittest
import maus_cpp.polynomial_map

class PolynomialMapTestCase(unittest.TestCase):
    def test_init(self):
        my_map = maus_cpp.polynomial_map.PolynomialMap(1, [[1.]])
        print my_map.get_coefficients_as_matrix()
        my_map.set_coefficients(2, [[3., 4.], [5., 6.]])
        print my_map.get_coefficients_as_matrix()



if __name__ == "__main__":
    unittest.main()
