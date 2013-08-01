#  This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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

# pylint: disable=C0103

"""
Test maus_cpp.optics_model
"""

import unittest

import numpy

import maus_cpp
from maus_cpp import covariance_matrix
from maus_cpp.covariance_matrix import CovarianceMatrix
from xboa.Bunch import Bunch

class OpticsModelTestCase(unittest.TestCase): # pylint: disable=R0904
    """Test maus_cpp.optics_model"""
    def setUp(self):
        """does nothing"""
        pass

    def test_init(self):
        """Test maus_cpp.optics_model.__init__() and deallocation"""
        cov = CovarianceMatrix()
        for j in range(1, 7):
            for i in range(1, 7):
                self.assertEqual(0., cov.get_element(i, j))
 
    def test_covariance_matrix(self):
        """Test maus_cpp.optics_model.__init__() and deallocation"""
        test_data = [[j*i for j in range(1, 7)] for i in range(1, 7)]
        test_array = numpy.array(test_data)
        try:
            cm1 = covariance_matrix.create_from_matrix()
            self.assertTrue(False, "Should have thrown a TypeError")
        except TypeError:
            pass
        cm2 = covariance_matrix.create_from_matrix(test_array)
        cm3 = covariance_matrix.create_from_matrix(matrix=test_array)
        try:
            covariance_matrix.create_from_matrix(1)
            self.assertTrue(False, "Should have thrown a RuntimeError")
        except RuntimeError:
            pass
        try:
            covariance_matrix.create_from_matrix(numpy.array([[1.]]))
            self.assertTrue(False, "Should have thrown a RuntimeError")
        except RuntimeError:
            pass
        try:
            test_data[1][1] = 'a'
            covariance_matrix.create_from_matrix(test_data)
            self.assertTrue(False, "Should have thrown a RuntimeError")
        except RuntimeError:
            pass

        # no test for positive definite (there should be); leave this here as a
        # reminder...
        test_data = [[float(-j*i) for j in range(1, 7)] for i in range(1, 7)]
        test_array = numpy.array(test_data)
        cov = covariance_matrix.create_from_matrix(test_array)
        for j in range(1, 7):
            for i in range(1, 7):
                self.assertEqual(test_data[i-1][j-1], cov.get_element(i, j))

    def test_get_element(self):
        """Test covariance matrix get_element function"""
        test_data = [[float(-j*i) for j in range(1, 7)] for i in range(1, 7)]
        test_array = numpy.array(test_data)
        cov = covariance_matrix.create_from_matrix(test_array)
        for j in range(1, 7):
            for i in range(1, 7):
                self.assertEqual(test_data[i-1][j-1], cov.get_element(i, j))

        # check access by value name
        cov.get_element(row=1, column=1)
        try:
            cov.get_element(row=1)
            self.assertTrue(False, "column is mandatory")
        except TypeError:
            pass
        try:
            cov.get_element(column=1)
            self.assertTrue(False, "row is mandatory")
        except TypeError:
            pass

        # check bounds
        try:
            cov.get_element(0, 1)
            self.assertTrue(False, "Should throw when out of bounds")
        except IndexError:
            pass
        try:
            cov.get_element(1, 0)
            self.assertTrue(False, "Should throw when out of bounds")
        except IndexError:
            pass
        try:
            cov.get_element(6, 7)
            self.assertTrue(False, "Should throw when out of bounds")
        except IndexError:
            pass
        try:
            cov.get_element(7, 6)
            self.assertTrue(False, "Should throw when out of bounds")
        except IndexError:
            pass


    def test_set_element(self):
        """Test covariance matrix set_element function"""
        test_data = [[float(-j*i) for j in range(1, 7)] for i in range(1, 7)]
        cov = CovarianceMatrix()
        for j in range(1, 7):
            for i in range(1, 7):
                cov.set_element(i, j, test_data[i-1][j-1])
                self.assertEqual(test_data[i-1][j-1], cov.get_element(i, j))
        # check set by value name
        cov.set_element(row=1, column=1, value=0.)
        try:
            cov.set_element(row=1, column=1)
            self.assertTrue(False, "value is mandatory")
        except TypeError:
            pass
        try:
            cov.set_element(row=1, value=1)
            self.assertTrue(False, "row is mandatory")
        except TypeError:
            pass
        try:
            cov.set_element(column=1, value=1)
            self.assertTrue(False, "row is mandatory")
        except TypeError:
            pass


        try:
            cov.set_element(1, 0, 0)
            self.assertTrue(False, "Should throw when out of bounds")
        except IndexError:
            pass
        try:
            cov.set_element(0, 1, 0)
            self.assertTrue(False, "Should throw when out of bounds")
        except IndexError:
            pass
        try:
            cov.set_element(6, 7, 0)
            self.assertTrue(False, "Should throw when out of bounds")
        except IndexError:
            pass
        try:
            cov.set_element(7, 6, 0)
            self.assertTrue(False, "Should throw when out of bounds")
        except IndexError:
            pass



    def test_create_from_twiss_parameters(self):
        """Test maus_cpp.covariance_matrix.create_from_penn_parameters"""
        try:
            maus_cpp.covariance_matrix.create_from_twiss_parameters()
            self.assertTrue(False, "Should have thrown a TypeError")
        except TypeError:
            pass
        cm1 = maus_cpp.covariance_matrix.create_from_twiss_parameters \
                      (mass=105.658, momentum=200.,
                       emittance_x=6., beta_x=333.,
                       emittance_y=6., beta_y=333.,
                       emittance_l=100., beta_l=10.)
        cm2 = maus_cpp.covariance_matrix.create_from_twiss_parameters \
                      (mass=105.658, momentum=200.,
                       emittance_x=6., beta_x=333.,
                       emittance_y=6., beta_y=333.,
                       emittance_l=100., beta_l=10.,
                       alpha_x=0., alpha_y=0., alpha_l=0.,
                       dispersion_x=0., dispersion_prime_x=0.,
                       dispersion_y=0., dispersion_prime_y=0.)
        for i in range(1, 7):
            for j in range(i, 7):
                self.assertEqual(cm1.get_element(i, j), cm2.get_element(i, j))
 
        cm3 = maus_cpp.covariance_matrix.create_from_twiss_parameters \
                      (mass=105.658, momentum=200.,
                       emittance_x=6., beta_x=333.,
                       emittance_y=5., beta_y=250.,
                       emittance_l=1., beta_l=10.,
                       alpha_x=1., alpha_y=2., alpha_l=1.,
                       dispersion_x=1., dispersion_prime_x=1.,
                       dispersion_y=3., dispersion_prime_y=3.)
        cm4 = maus_cpp.covariance_matrix.create_from_twiss_parameters \
                      (105.658, 200.,
                       6., 333., 5., 250., 1., 10.,
                       1., 2., 1., 1., 1., 3., 3.)
        for i in range(1, 7):
            for j in range(i, 7):
                self.assertEqual(cm3.get_element(i, j), cm4.get_element(i, j))
        bunch = Bunch.build_ellipse_2d(10., 1., 1., 200., 105.658, False)
        for i in range(0, 2):
            for j in range(0, 2):
                self.assertLess(2.*abs(bunch[i, j]-cm4.get_element(i+1, j+1))/ \
                                   abs(bunch[i, j]+cm4.get_element(i+1, j+1)),
                                   1.e-3)
        bunch = Bunch.build_ellipse_2d(333., 1., 6., 200., 105.658, False)
        for i in range(2, 4):
            for j in range(2, 4):
                a = bunch[i-2, j-2]
                b = cm4.get_element(i+1, j+1)
                self.assertLess(2.*abs(a-b)/abs(a+b), 1.e-3)
        bunch = Bunch.build_ellipse_2d(250., 2., 5., 200., 105.658, False)
        for i in range(4, 6):
            for j in range(4, 6):
                a = bunch[i-4, j-4]
                b = cm4.get_element(i+1, j+1)
                self.assertLess(2.*abs(a-b)/abs(a+b), 1.e-3)
            
    def test_create_from_penn_parameters(self):
        """Test maus_cpp.covariance_matrix.create_from_penn_parameters"""
        try:
            maus_cpp.covariance_matrix.create_from_penn_parameters()
            self.assertTrue(False, "Should have thrown a TypeError")
        except TypeError:
            pass
        cm1 = maus_cpp.covariance_matrix.create_from_penn_parameters \
                      (mass=105.658, momentum=200., emittance_t=6., beta_t=333.,
                       emittance_l=1., beta_l=10.)
        cm2 = maus_cpp.covariance_matrix.create_from_penn_parameters \
                      (mass=105.658, momentum=200., emittance_t=6., beta_t=333.,
                       emittance_l=1., beta_l=10., alpha_t=0., alpha_l=0.,
                       charge=1., bz=0., ltwiddle=0., dispersion_x=0.,
                       dispersion_prime_x=0., dispersion_y=0.,
                       dispersion_prime_y=0.)
        for i in range(1, 7):
            for j in range(i, 7):
                self.assertEqual(cm1.get_element(i, j), cm2.get_element(i, j))

        cm3 = maus_cpp.covariance_matrix.create_from_penn_parameters \
                      (mass=105.658, momentum=200., emittance_t=6., beta_t=333.,
                       emittance_l=1., beta_l=10., alpha_t=1., alpha_l=1.,
                       charge=-1., bz=4.e-3, ltwiddle=1., dispersion_x=1.,
                       dispersion_prime_x=1., dispersion_y=1.,
                       dispersion_prime_y=1.)
        cm4 = maus_cpp.covariance_matrix.create_from_penn_parameters \
                      (105.658, 200., 6., 333.,
                       1., 10., 1., 1.,
                       -1., 4.e-3, 1., 1., 1., 1., 1.)
        for i in range(1, 7):
            for j in range(i, 7):
                self.assertEqual(cm3.get_element(i, j), cm4.get_element(i, j))
        bunch = Bunch.build_ellipse_2d(10., 1., 1., 200., 105.658, False)
        for i in range(0, 2):
            for j in range(0, 2):
                self.assertLess(2.*abs(bunch[i, j]-cm4.get_element(i+1, j+1))/ \
                                   abs(bunch[i, j]+cm4.get_element(i+1, j+1)),
                                   1.e-3)
        bunch = Bunch.build_penn_ellipse(6., 105.658, 333.,
                                         1., 200., 1., 4.e-3, -1.)
        for i in range(0, 4):
            for j in range(0, 4):
                self.assertAlmostEqual(bunch[i, j], cm4.get_element(i+3, j+3))

    def test_repr(self):
        """Test maus_cpp.covariance_matrix.__repr__()"""
        ref_data = [[float(-j*i) for j in range(1, 7)] for i in range(1, 7)]
        ref_array = numpy.array(ref_data)
        cov = covariance_matrix.create_from_matrix(ref_array)
        test_array = eval(repr(cov))
        for i, x_array in enumerate(test_array):
            for j, x in enumerate(x_array):
               self.assertAlmostEqual(x, ref_data[i][j])
                

if __name__ == "__main__":
    unittest.main()


