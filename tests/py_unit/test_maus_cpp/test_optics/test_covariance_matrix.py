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
from maus_cpp.covariance_matrix import CovarianceMatrix

class OpticsModelTestCase(unittest.TestCase): # pylint: disable=R0904
    """Test maus_cpp.optics_model"""
    def setUp(self):
        """does nothing"""
        pass

    def test_init(self):
        """Test maus_cpp.optics_model.__init__() and deallocation"""
        test_data = [[j*i for j in range(1, 7)] for i in range(1, 7)]
        test_array = numpy.array(test_data)
        cm1 = CovarianceMatrix()
        cm2 = CovarianceMatrix(test_array)
        cm3 = CovarianceMatrix(matrix=test_array)
        try:
            CovarianceMatrix(1)
            self.assertTrue(False, "Should have thrown a RuntimeError")
        except RuntimeError:
            pass
        try:
            CovarianceMatrix(numpy.array([[1.]]))
            self.assertTrue(False, "Should have thrown a RuntimeError")
        except RuntimeError:
            pass
        try:
            test_data[1][1] = 'a'
            CovarianceMatrix(test_data)
            self.assertTrue(False, "Should have thrown a RuntimeError")
        except RuntimeError:
            pass

        # no test for positive definite (there should be); leave this here as a
        # reminder...
        test_data = [[-j*i for j in range(1, 7)] for i in range(1, 7)]
        test_array = numpy.array(test_data)
        CovarianceMatrix(test_array)
        print cm1, cm2, cm3

    def _test_create_from_twiss_parameters(self):
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
                       emittance_l=1., beta_l=10.)
        cm2 = maus_cpp.covariance_matrix.create_from_twiss_parameters \
                      (mass=105.658, momentum=200.,
                       emittance_x=6., beta_x=333.,
                       emittance_y=6., beta_y=333.,
                       emittance_l=1., beta_l=10.,
                       alpha_x=0., alpha_y=0., alpha_l=0.,
                       dispersion_x=0., dispersion_prime_x=0.,
                       dispersion_y=0., dispersion_prime_y=0.)

        cm3 = maus_cpp.covariance_matrix.create_from_twiss_parameters \
                      (mass=105.658, momentum=200.,
                       emittance_x=6., beta_x=333.,
                       emittance_y=6., beta_y=333.,
                       emittance_l=1., beta_l=10.,
                       alpha_x=1., alpha_y=1., alpha_l=1.,
                       dispersion_x=1., dispersion_prime_x=1.,
                       dispersion_y=1., dispersion_prime_y=1.)
        cm4 = maus_cpp.covariance_matrix.create_from_twiss_parameters \
                      (105.658, 200.,
                       6., 333., 6., 333., 1., 10.,
                       1., 1., 1., 1., 1., 1., 1.)
        print cm1, cm2, cm3, cm4
            
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
        print cm1, cm2, cm3, cm4


if __name__ == "__main__":
    unittest.main()


