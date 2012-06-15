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
Test for MapCppSingleStationRecon
"""
# pylint: disable=C0103
# pylint: disable=R0904

import json
import unittest
import os
from Configuration import Configuration

from MapCppSingleStationRecon import MapCppSingleStationRecon

# Disable: Too many public methods
# pylint: disable-msg=R0904
# Disable: Invalid name
# pylint: disable-msg=C0103
# Disable: Class method should have 'cls' as first argument
# pylint: disable-msg=C0202

class MapCppSingleStationReconTestCase(unittest.TestCase):
    """ The MapCppSingleStationRecon test.
        Tests to be added here.
    """
    @classmethod
    def setUpClass(self):
        """ Class Initializer.
            The set up is called before each test function
            is called.
        """
        self.mapper = MapCppSingleStationRecon()
        conf = Configuration()
        # Test whether the configuration files were loaded correctly at birth
        success = self.mapper.birth(conf.getConfigJSON())
        if not success:
            raise Exception('InitializeFail', 'Could not start worker')


    def test_death(self):
        """ Test to make sure death occurs """
        self.assertTrue(self.mapper.death())

    def test_process(self):
        """ Test of the process function """
        root_dir = os.environ.get("MAUS_ROOT_DIR")
        assert root_dir != None
        assert os.path.isdir(root_dir)
        _filename = \
        '%s/src/map/MapCppSingleStationRecon/lab7_unpacked' % root_dir
        assert os.path.isfile(_filename)
        _file = open(_filename, 'r')
        # File is open.
        # Spill 1 is corrupted.
        spill_1 = _file.readline().rstrip()
        output_1 = self.mapper.process(spill_1)
        self.assertTrue("errors" in json.loads(output_1))
        # Close file.
        _file.close()

    @classmethod
    def tear_down_class(self):
        """___"""
        success = self.mapper.death()
        if not success:
            raise Exception('InitializeFail', 'Could not start worker')
        self.mapper = None

if __name__ == '__main__':
    unittest.main()
