# This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
#
# MAUS is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# MAUS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with MAUS.  If not, see <http://www.gnu.org/licenses/>.

"""Tests for MapCppTOFOfflineDigits"""

# pylint: disable = C0103

import os
import md5
import unittest
import json
from Configuration import Configuration
from _InputCppDAQOfflineData import InputCppDAQOfflineData

class InputCppDAQOfflineDataTestCase(unittest.TestCase): #pylint:disable=R0904
    """Tests for InputCppDAQOfflineData"""
    @classmethod
    def setUpClass(self): # pylint: disable = C0103, C0202
        """Sets a mapper and configuration"""
        print "SETUPCLASS"
        if not os.environ.get("MAUS_ROOT_DIR"):
            raise Exception('InitializeFail', 'MAUS_ROOT_DIR unset!')
        # Set our data path & filename
        # It would be nicer to test with a smaller data file!
        self._datapath = '%s/src/input/InputCppDAQData' % \
                            os.environ.get("MAUS_ROOT_DIR")
        self._datafile = '02873'
        config = Configuration().getConfigJSON()
        config_json = json.loads(config)
        config_json["daq_data_path"] = self._datapath
        config_json["daq_data_file"] = self._datafile
        self._config = json.dumps(config_json)
        self.mapper = None

    def test_init(self):
        """Check birth with default configuration"""
        self.mapper = InputCppDAQOfflineData()
        self.mapper.birth( self._config)
        # Check re-init without closing fails
        try:
            self.mapper.birth( self._config )
            self.assertTrue(False, msg="Should have raised ValueError")
        except ValueError:
            pass
        self.mapper.death()

    def test_single(self):
        """Test a single event"""
        self.mapper = InputCppDAQOfflineData(self._datapath, \
                                       self._datafile)
        self.mapper.birth(self._config)
        # Get a single event and check it's the right size
        data = self.mapper.emitter().next()
        # Data shold be 108 (first event is start of burst)
        self.assertEqual(len(data), 108)
        self.mapper.death()

    def test_multi(self):
        """Test reading the whole file"""
        self.mapper = InputCppDAQOfflineData(self._datapath, \
                                       self._datafile)
        self.mapper.birth(self._config)
        event_count = 0

        # We can try md5'ing the whole dataset
        digester = md5.new()

        for i in self.mapper.emitter():
            digester.update(i) #pylint: disable=E1101
            event_count = event_count + 1

        # We should now have processed 26 events
        self.assertEqual(event_count, 26)

        # Check the md5 sum matches the expected value
        # changed checksum to reflect the run_num addition
        # changed checksum from ca8fb803f65c2ef93327379bee9399d0
        # to reflect new changes due to the cpp data - Dec 1,2013
        self.assertEqual(digester.hexdigest(), \
                         '08bdfba7e5cad1b8da503742c545a7c8')

        self.mapper.death()

    @classmethod
    def tearDownClass(self): # pylint: disable = C0103, C0202
        """Check that we can death() MapCppTOFDigits"""
        pass

if __name__ == '__main__':
    unittest.main()
