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
Test to run against all maus data in the ref_data directory
"""

import unittest
import glob
import os 

from physics_model_test import plotter
from physics_model_test import runner
from physics_model_test import geometry

class MaterialsAndSteppingTest(unittest.TestCase): # pylint: disable=R0904
    """
    Test multiple scattering and energy loss in a number of different
    circumstances
    """

    def test_all(self):
        """
        Test all examples in ref_data directory with names like maus.*.dat
        """
        reference_data = glob.glob(geometry.ref_data("MUSCAT_data.dat"))
        test_data = glob.glob(geometry.ref_data("maus.0.3.3.ref_data.dat"))
        (passes, fails, warns) = (0, 0, 0)
        for test_in in reference_data:
            test_name = os.path.split(test_in)[-1]
            test_name = test_name.replace('ref', 'test')
            test_out = geometry.temp(test_name)
            (my_p, my_f, my_w) = \
                             runner.code_comparison_test(test_in, test_out)
            passes += my_p
            fails += my_f
            warns += my_w
            test_data += [test_out]
        print "==========================="
        print "|| Materials and Stepping ||"
        print "==========================="
        print "   ", passes, "passes"
        print "   ", fails, "fails"
        print "   ", warns, "warns"
        self. assertEqual(fails, 0)
        plotter.plot(reference_data+test_data)

if __name__ == "__main__":
    unittest.main()

