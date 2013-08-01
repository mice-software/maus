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
Test maus_cpp.phase_space_vector
"""

import unittest

import numpy

import maus_cpp
from maus_cpp.phase_space_vector import PhaseSpaceVector
from maus_cpp.phase_space_vector import create_from_coordinates

class PhaseSpaceVectorTestCase(unittest.TestCase): # pylint: disable=R0904
    """Test maus_cpp.phase_space_vector"""

    def test_init(self):
        """Test maus_cpp.phase_space_vector.init()"""
        psv = maus_cpp.phase_space_vector.PhaseSpaceVector()

    def test_create_from_coordinates(self):
        """Test maus_cpp.phase_space_vector.create_from_coordinates()"""
        psv = maus_cpp.phase_space_vector.create_from_coordinates \
                                     (t=1, energy=2., x=3., px=4., y=5., py=6.)
        self.assertAlmostEqual(psv.get_t(), 1.)
        self.assertAlmostEqual(psv.get_energy(), 2.)
        self.assertAlmostEqual(psv.get_x(), 3.)
        self.assertAlmostEqual(psv.get_px(), 4.)
        self.assertAlmostEqual(psv.get_y(), 5.)
        self.assertAlmostEqual(psv.get_py(), 6.)
        try:
            psv = create_from_coordinates(t=1, energy=2., x=3., px=4., y=5.)
            self.assertTrue(False, "Expected type error")
        except TypeError:
            pass
        try:
            psv = create_from_coordinates(t=1, energy=2., x=3., px=4., py=6.)
            self.assertTrue(False, "Expected type error")
        except TypeError:
            pass
        try:
            psv = create_from_coordinates(t=1, energy=2., x=3., y=5., py=6.)
            self.assertTrue(False, "Expected type error")
        except TypeError:
            pass
        try:
            psv = create_from_coordinates(t=1, energy=2., px=4., y=5., py=6.)
            self.assertTrue(False, "Expected type error")
        except TypeError:
            pass
        try:
            psv = create_from_coordinates(t=1, x=3., px=4., y=5., py=6.)
            self.assertTrue(False, "Expected type error")
        except TypeError:
            pass
        try:
            psv = create_from_coordinates(energy=2., x=3., px=4., y=5., py=6.)
            self.assertTrue(False, "Expected type error")
        except TypeError:
            pass

    def test_get_set(self):
        """Test maus_cpp.phase_space_vector get_<> and set_<>"""
        psv = create_from_coordinates(t=1, energy=2., x=3., px=4., y=5., py=6.)
        test_value = 1.
        method_list = [(psv.get_t,  psv.set_t),
                       (psv.get_energy, psv.set_energy),
                       (psv.get_x,  psv.set_x),
                       (psv.get_px, psv.set_px),
                       (psv.get_y,  psv.set_y),
                       (psv.get_py, psv.set_py)]
        for getter, setter in method_list:
            self.assertAlmostEqual(getter(bob="nonsense"), test_value)
            setter(test_value+10.)
            self.assertAlmostEqual(getter(), test_value+10.)
            setter(value=test_value+10.)
            try:
                setter(value=test_value+10., bob="nonsense")
                self.assertTrue(False, "Should have thrown an exception")
            except TypeError:
                pass
            test_value += 1.

    def test_repr(self):
        """Test maus_cpp.PhaseSpaceVector.__repr__ and __str__"""
        psv = create_from_coordinates(t=1, energy=2., x=3., px=4., y=5., py=6.)
        psv_as_list = eval(psv.__repr__())
        for i, value in enumerate(psv_as_list):
            self.assertAlmostEqual(value, float(i+1))
        psv_as_list = eval(psv.__str__())
        for i, value in enumerate(psv_as_list):
            self.assertAlmostEqual(value, float(i+1))

if __name__ == "__main__":
    unittest.main()


