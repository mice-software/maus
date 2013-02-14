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
import json
import md5
import unittest
from Configuration import Configuration
from InputCppDAQOnlineData import InputCppDAQOnlineData #pylint: disable=F0401

class InputCppDAQOnlineDataTestCase(unittest.TestCase): #pylint:disable=R0904
    """Tests for InputCppDAQOnlineData"""
    @classmethod
    def setUpClass(self): # pylint: disable = C0103, C0202
        """Sets a mapper and configuration"""
        self._c = Configuration()
        self.mapper = None
        if not os.environ.get("MAUS_ROOT_DIR"):
            raise Exception('InitializeFail', 'MAUS_ROOT_DIR unset!')
        # Set our data path & filename
        # It would be nicer to test with a smaller data file!
        datapath = '%s/src/input/InputCppDAQData/' % \
                            os.environ.get("MAUS_ROOT_DIR")
        datafile = '02873.003'
        self.mon =  datapath + datafile
        # """ Get the default configuration """
        self._test_configuration = Configuration().getConfigJSON()
        test_conf_json = json.loads(self._test_configuration)
        test_conf_json['DAQ_monitor_name'] = 'MICE_Online_Test'
        self._test_configuration = json.dumps(test_conf_json)


    def test_single(self):
        """Test a single event"""
        self.mapper = InputCppDAQOnlineData()
        self.assertTrue(self.mapper.birth( self._test_configuration ))
        self.mapper.setMonitorSrc(self.mon)
        # Get a single event and check it's the right size
        self.assertTrue(self.mapper.readNextEvent())
        data = self.mapper.getCurEvent()
        # Data shold be 80 (first event is start of burst)
        self.assertEqual(len(data), 96)
        self.assertTrue(self.mapper.death())
        return

    def test_multi(self):
        """Test reading the whole file"""
        self.mapper = InputCppDAQOnlineData()
        self.assertTrue(self.mapper.birth( self._test_configuration ))
        self.mapper.setMonitorSrc(self.mon)
        event_count = 0

        # We can try md5'ing the whole dataset
        digester = md5.new()

        for i in self.mapper.emitter():
            digester.update(i) # pylint:disable=E1101
            event_count = event_count + 1

        # We should now have processed 26 events
        self.assertEqual(event_count, 26)

        # Check the md5 sum matches the expected value
        # changed checksum to reflect the run_num addition
        self.assertEqual(digester.hexdigest(), \
                         'ca8fb803f65c2ef93327379bee9399d0')

        self.assertTrue(self.mapper.death())

    @classmethod
    def tearDownClass(self): # pylint: disable = C0103, C0202
        """Check that we can death()"""
        pass

if __name__ == '__main__':
    unittest.main()
