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

"""Tests for MapCppKLDigits"""

# pylint: disable = C0103

import os
import json
import unittest
from Configuration import Configuration
import MAUS

class MapCppKLDigitsTestCase(unittest.TestCase): # pylint: disable = R0904
    """Tests for MapCppKLDigits"""
    @classmethod
    def setUpClass(cls): # pylint: disable = C0103
        """Sets a mapper and configuration"""
        cls.mapper = MAUS.MapCppKLDigits()
        cls.c = Configuration()

    def test_empty(self):
        """Check can handle empty configuration"""
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
        test1 = ('%s/src/map/MapCppKLDigits/noDataTest.txt' % 
                 os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test1,'r')
        data = fin.read()
        # test with no data.
        result = self.mapper.process(data)
        spill_out = json.loads(result)
        self.assertFalse('digits' in spill_out)

    def __test_process_kl_digits(self, spill_in, spill_out):
        """Test kl digits"""
        n_digits_part_ev0 = len\
              (spill_out['recon_events'][0]['kl_event']['kl_digits']['kl'])
        self.assertEqual(n_digits_part_ev0, 4)
        n_digits_part_ev1 = len\
              (spill_out['recon_events'][1]['kl_event']['kl_digits']['kl'])
        self.assertEqual(n_digits_part_ev1, 2)
        n_digits_part_ev2 = len\
              (spill_out['recon_events'][2]['kl_event']['kl_digits']['kl'])
        self.assertEqual(n_digits_part_ev2, 0)
        # test the creation of the digit
        digit0_part_ev0_kl = \
              spill_out['recon_events'][0]['kl_event']['kl_digits']['kl'][0]
        charge_mm = spill_in['daq_data']['kl'][0]['V1724'][0]['charge_mm']
        self.assertEqual(charge_mm, digit0_part_ev0_kl ['charge_mm'])

        self.assertFalse(spill_out\
                         ['recon_events'][2]['kl_event']['kl_digits']['kl'])

    def test_process(self):
        """Test MapCppKLDigits process method"""
        test2 = ('%s/src/map/MapCppKLDigits/kl_digit_test.txt' % 
                 os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test2,'r')
        data = fin.read()
        # test with some crazy events.
        result = self.mapper.process(data)
        spill_in = json.loads(data)
        spill_out = json.loads(result)

        # test the outputs
        self.__test_process_kl_digits(spill_in, spill_out)

    @classmethod
    def tearDownClass(cls): # pylint: disable = C0103
        """Check that we can death() MapCppKLDigits"""
        success = cls.mapper.death()
        if not success:
            raise Exception('InitializeFail', 'Could not start worker')
        cls.mapper = None

if __name__ == '__main__':
    unittest.main()
