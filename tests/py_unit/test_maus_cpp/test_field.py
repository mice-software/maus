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
Test maus_cpp.field
"""

import StringIO
import unittest

import Configuration
import maus_cpp.globals
import maus_cpp.field

class FieldTestCase(unittest.TestCase): # pylint: disable=R0904
    """Test maus_cpp.field"""

    def setUp(self): # pylint: disable=C0103
        """Set up test"""
        config_options = StringIO.StringIO(unicode("""
simulation_geometry_filename = "Test.dat"
reconstruction_geometry_filename = "Test.dat"
        """))
        self.config = Configuration.Configuration().getConfigJSON(
                                                         config_options, False)

    def test_get_field_value(self):
        """Test maus_cpp.Field.get_field_value(...)"""
        maus_cpp.globals.birth(self.config)
        for x_pos in range(10):
            field_value = maus_cpp.field.get_field_value\
                                                    (x_pos, x_pos, x_pos, x_pos)
            for i in range(6):
                self.assertAlmostEqual(field_value[i], 0., 1e-12)
        maus_cpp.globals.death()

if __name__ == "__main__":
    unittest.main()


