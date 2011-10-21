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

"""Tests for MapCppTOFDigits"""

# pylint: disable = C0103

import os
import json
import unittest
from Configuration import Configuration
import MAUS

class MapCppTOFDigitsTestCase(unittest.TestCase): # pylint: disable = R0904
    """Tests for MapCppTOFDigits"""
    @classmethod
    def setUpClass(cls): # pylint: disable = C0103
        """Sets a mapper and configuration"""
        cls.mapper = MAUS.MapCppTOFDigits()
        cls.c = Configuration()

    def test_empty(self):
        """Check can handle empty spill"""
        result = self.mapper.birth("")
        self.assertFalse(result)
        result = self.mapper.process("")
        doc = json.loads(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("bad_json_document" in doc["errors"])

    def test_init(self):
        """Check birth with default configuration"""
        success = self.mapper.birth(self. c.getConfigJSON())
        self.assertTrue(success)

    def test_no_data(self):
        """Check that nothing happens in absence of data"""
        test1 = ('%s/src/map/MapCppTOFDigits/noDataTest.txt' % 
                 os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test1,'r')
        data = fin.read()
        # test with no data.
        result = self.mapper.process(data)
        spill_out = json.loads(result)
        self.assertFalse('digits' in spill_out)

    def __test_tof0_digits(self, spill_in, spill_out):
        """Test tof0 digits"""
        n_part_events = len(spill_out['digits']['tof0'])
        self.assertEqual(n_part_events, 2)
        n_digits_part_ev0 = len(spill_out['digits']['tof0'][0])
        self.assertEqual(n_digits_part_ev0, 3)
        self.assertFalse(spill_out['digits']['tof0'][1])
        # test the creation of the digit
        digit0_part_ev0_tof0 = spill_out['digits']['tof0'][0][0]
        l_time = spill_in['daq_data']['tof0'][0]['V1290'][0]['leading_time']
        charge_mm = spill_in['daq_data']['tof0'][0]['V1724'][0]['charge_mm']
        trig = spill_in['daq_data']['trigger'][0]['V1290'][1]['leading_time']
        trig_req = spill_in['daq_data'] \
                              ['trigger_request'][0]['V1290'][1]['leading_time']
        self.assertEqual(l_time, digit0_part_ev0_tof0 ['leading_time'])
        self.assertEqual(charge_mm, digit0_part_ev0_tof0 ['charge_mm'])
        self.assertEqual(trig, digit0_part_ev0_tof0 ['trigger_leading_time'])
        self.assertEqual(trig_req, digit0_part_ev0_tof0
                                               ['trigger_request_leading_time'])

    def __test_tof1_digits(self, spill_out):
        """Test tof1 digits"""
        n_part_events = len(spill_out['digits']['tof1'])
        self.assertEqual(n_part_events, 2)
        n_digits_part_ev0_tof1 = len(spill_out['digits']['tof1'][0])
        self.assertEqual(n_digits_part_ev0_tof1, 3)
        n_digits_part_ev1_tof1 = len(spill_out['digits']['tof1'][1])
        self.assertEqual(n_digits_part_ev1_tof1, 3)
        digit2_part_ev1_tof1 = spill_out['digits']['tof1'][1][2]
        self.assertFalse('charge_mm' in digit2_part_ev1_tof1)
        self.assertFalse('charge_pm' in digit2_part_ev1_tof1)

    def __test_tof2_digits(self, spill_out):
        """Test tof2 digits"""
        n_part_events = len(spill_out['digits']['tof2'])
        self.assertEqual(n_part_events, 2)
        self.assertFalse(spill_out['digits']['tof2'][0])
        self.assertFalse(spill_out['digits']['tof2'][1])

    def test_process(self):
        """Test MapCppTOFDigits process method"""
        test2 = ('%s/src/map/MapCppTOFDigits/processTest.txt' % 
                 os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test2,'r')
        data = fin.read()
        # test with some crazy events.
        result = self.mapper.process(data)
        spill_in = json.loads(data)
        spill_out = json.loads(result)

        # test the outputs
        self.__test_tof0_digits(spill_in, spill_out)
        self.__test_tof1_digits(spill_out)
        self.__test_tof2_digits(spill_out)

    @classmethod
    def tearDownClass(cls): # pylint: disable = C0103
        """Check that we can death() MapCppTOFDigits"""
        success = cls.mapper.death()
        if not success:
            raise Exception('InitializeFail', 'Could not start worker')
        cls.mapper = None

if __name__ == '__main__':
    unittest.main()
