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
Test maus_cpp.globals
"""

import json
import StringIO
import unittest

import Configuration
import maus_cpp.globals

class GlobalsTestCase(unittest.TestCase): # pylint: disable=R0904
    """Test maus_cpp.globals"""

    def setUp(self): # pylint: disable=C0103
        """Set up test"""
        self.test_config = ""
        if maus_cpp.globals.has_instance():
            self.test_config = maus_cpp.globals.get_configuration_cards()
        config_options = StringIO.StringIO(unicode("""
simulation_geometry_filename = "Test.dat"
reconstruction_geometry_filename = "Test.dat"
        """))
        self.config = Configuration.Configuration().getConfigJSON(
                                                         config_options, False)

    def tearDown(self): # pylint: disable = C0103
        """Reset the globals"""
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        if self.test_config != "":
            maus_cpp.globals.birth(self.test_config)

    def test_birth(self):
        """Test maus_cpp.globals.birth() and maus_cpp.globals.death()"""
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        self.assertRaises(RuntimeError, maus_cpp.globals.death, ())
        self.assertRaises(TypeError, maus_cpp.globals.birth, ())
        self.assertRaises(RuntimeError, maus_cpp.globals.birth, (""))
        maus_cpp.globals.birth(self.config)  # and now don't raise an error
        maus_cpp.globals.death()
        self.assertRaises(RuntimeError, maus_cpp.globals.death, ())

    def test_has_instance(self):
        """Test maus_cpp.globals.has_instance()"""
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        self.assertEqual(maus_cpp.globals.has_instance(), 0)
        maus_cpp.globals.birth(self.config)
        self.assertEqual(maus_cpp.globals.has_instance(), 1)
        maus_cpp.globals.death()
        self.assertEqual(maus_cpp.globals.has_instance(), 0)
        maus_cpp.globals.birth(self.config)
        self.assertEqual(maus_cpp.globals.has_instance(), 1)
        maus_cpp.globals.death()
        self.assertEqual(maus_cpp.globals.has_instance(), 0)

    def test_get_configuration_cards(self):
        """Test maus_cpp.globals.get_configuration_cards()"""
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        self.assertRaises(
             RuntimeError, maus_cpp.globals.get_configuration_cards, ())
        maus_cpp.globals.birth(self.config)
        json_string = maus_cpp.globals.get_configuration_cards()
        self.assertEqual(json.loads(json_string), json.loads(self.config))
        maus_cpp.globals.death()

    def test_version_number(self):
        """Test maus_cpp.globals.get_version_number()"""
        # doesnt need globals initialised
        version = maus_cpp.globals.get_version_number().split('.')
        self.assertEqual(len(version), 3)
        for i in range(3):
            int(version[i])
        maus_cpp.globals.birth(self.config)
        dc_version = json.loads(self.config)["maus_version"]
        dc_version = dc_version.split(' ')[-1]
        self.assertEqual(maus_cpp.globals.get_version_number(), dc_version)
        maus_cpp.globals.death()
        
if __name__ == "__main__":
    unittest.main()

