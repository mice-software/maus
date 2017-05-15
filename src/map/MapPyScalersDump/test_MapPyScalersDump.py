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

"""Tests for MapPyScalersDump"""

#pylint: disable = C0103

import os
import unittest
import json

from Configuration import Configuration

import MAUS

class MapPyScalersDumpTestCase(unittest.TestCase): # pylint: disable = R0904
    """Tests for MapPyScalersDump"""
    @classmethod
    def setUpClass(cls):  #pylint: disable = C0103
        """Construct for MapPyScalersDump """
        if not os.environ.get("MAUS_ROOT_DIR"):
            raise Exception('InitializeFail', 'MAUS_ROOT_DIR unset!')
        cls.mapper = MAUS.MapPyScalersDump()
        # Set our data path & filename
        # It would be nicer to test with a smaller data file!
        cls._datapath = '%s/src/input/InputCppDAQData' % \
                            os.environ.get("MAUS_ROOT_DIR")
        cls._datafile = '06008'
        cls._c = Configuration()

    def test_something(self):
        """ Check against different issues"""
        inputter = MAUS.InputCppDAQOfflineData(self._datapath, self._datafile)
        conf_json = json.loads(self._c.getConfigJSON())
        conf_json["DAQ_cabling_by"] = "date"
        inputter.birth(json.dumps(conf_json))

        success = self.mapper.birth("{}")
        self.assertFalse(success)

        success = self.mapper.add("")
        self.assertFalse(success)

        success = self.mapper.add("{}")
        self.assertFalse(success)

        success = self.mapper.dump()
        self.assertTrue(success)

        result = self.mapper.process("{}")
        self.assertEqual(len(result), 2)

    @classmethod
    def tearDownClass(cls): #pylint: disable = C0103
        success = cls.mapper.death()
        if not success:
            raise Exception('InitializeFail', 'Could not start worker')
        cls.mapper = None

if __name__ == '__main__':
    unittest.main()
