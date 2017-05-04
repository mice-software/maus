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
        # setup data files for StepI and StepIV
        # set the checksum and event count accordingly
        self._datafile = '05466'
        self._numevents = 12
        self._checksum = 'f226ea9b823996f653a73fd9e969b2f2'
        # self._checksum = 'f699f0d81aee1f64a2f1cec7968b9289'
        # note that the StepIV file is garbage data as of now
        # this will have to be updated - DR, March 13, 2015
        if os.environ['MAUS_UNPACKER_VERSION'] == "StepIV":
            self._datafile = '06008'
            self._numevents = 22
            self._checksum = '155b3b02e36ea80c0b0dcfad3be7027d'
        config = Configuration().getConfigJSON()
        config_json = json.loads(config)
        config_json["daq_data_path"] = self._datapath
        config_json["daq_data_file"] = self._datafile
        config_json["DAQ_cabling_by"] = "date"
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
        data = self.mapper.emitter().next() # pylint:disable=W0612
        # Data should be 108 (first event is start of burst)
        # self.assertEqual(len(data), 108)
        self.mapper.death()

    def test_multi(self):
        """Test reading the whole file"""
        self.mapper = InputCppDAQOfflineData(self._datapath, \
                                       self._datafile)
        self.mapper.birth(self._config)
        event_count = 0

        # We can try md5'ing the whole dataset
        digester = md5.new() #pylint: disable=W0612

        for i in self.mapper.emitter(): #pylint: disable=W0612
            # digester.update(i) #pylint: disable=E1101, W0612
            event_count = event_count + 1

        print event_count
        # We should now have processed 26 events
        #self.assertEqual(event_count, 26)
        self.assertEqual(event_count, self._numevents)

        # Check the md5 sum matches the expected value
        # changed checksum to reflect the run_num addition
        # changed checksum from ca8fb803f65c2ef93327379bee9399d0
        # to reflect new changes due to the cpp data - Dec 1,2013
        # changed checksum from 08bdfba7e5cad1b8da503742c545a7c8
        # to reflect changes from addition of raw tracker data - 16/05/2014
        #self.assertEqual(digester.hexdigest(), \
        #                 '2ca9328c6bf981fb242b3d985d226125')
        #self.assertEqual(digester.hexdigest(), \
        #                 self._checksum)

        self.mapper.death()

    @classmethod
    def tearDownClass(self): # pylint: disable = C0103, C0202
        """Check that we can death() MapCppTOFDigits"""
        pass

if __name__ == '__main__':
    unittest.main()
