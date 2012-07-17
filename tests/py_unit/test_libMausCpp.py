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

"""
Test libMausCpp
"""

import StringIO
import json
import unittest

import Configuration
import libMausCpp

class LibMausCppTestCase(unittest.TestCase):
    """Test libMausCpp"""

    def setUp(self):
        """Set up test"""
        config_options = StringIO.StringIO(unicode("""
simulation_geometry_filename = "Test.dat"
reconstruction_geometry_filename = "Test.dat"
        """))
        self.config = Configuration.Configuration().getConfigJSON(
                                                         config_options, False)

    def test_initialise(self):
        """Test libMausCpp.initialise() and libMausCpp.destruct()"""
        self.assertRaises(RuntimeError, libMausCpp.destruct, ())
        self.assertRaises(TypeError, libMausCpp.initialise, ())
        self.assertRaises(RuntimeError, libMausCpp.initialise, (""))
        libMausCpp.initialise(self.config)  # and now don't raise an error
        libMausCpp.destruct()
        self.assertRaises(RuntimeError, libMausCpp.destruct, ())

    def test_has_instance(self):
        """Test libMausCpp.has_instance()"""
        self.assertEqual(libMausCpp.has_instance(), 0)
        libMausCpp.initialise(self.config)
        self.assertEqual(libMausCpp.has_instance(), 1)
        libMausCpp.destruct()
        self.assertEqual(libMausCpp.has_instance(), 0)
        libMausCpp.initialise(self.config)
        self.assertEqual(libMausCpp.has_instance(), 1)
        libMausCpp.destruct()
        self.assertEqual(libMausCpp.has_instance(), 0)

    def test_get_field_value(self):
        """Test libMausCpp.Field.get_field_value(...)"""
        libMausCpp.initialise(self.config)
        for x in range(10):
            field_value = libMausCpp.Field.get_field_value(x, x, x, x)
            for i in range(6):
                self.assertAlmostEqual(field_value[i], 0., 1e-12)
        libMausCpp.destruct()

if __name__ == "__main__":
    unittest.main()

